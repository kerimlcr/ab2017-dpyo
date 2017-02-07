/* PlayManager.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "PlayManager.h"
#include "Interfaces/Notification/NotificationHandler.h"

#include <QDateTime>
#include <QTime>

PlayManager::PlayManager(QObject* parent) :
	QObject(parent),
	SayonaraClass()
{

	_position_ms = 0;
	_cur_idx = -1;
	_playstate = PlayState::Stopped;

	bool load_playlist = (_settings->get(Set::PL_LoadSavedPlaylists) || _settings->get(Set::PL_LoadTemporaryPlaylists));
	bool load_last_track = _settings->get(Set::PL_LoadLastTrack);
	bool remember_last_time = _settings->get(Set::PL_RememberTime);

	if(load_playlist && load_last_track && remember_last_time){
		_initial_position_ms = _settings->get(Set::Engine_CurTrackPos_s) * 1000;
	}

	else{
		_initial_position_ms = 0;
	}

	stop();
}

PlayManager::~PlayManager()
{
	 _settings->set(Set::Engine_CurTrackPos_s, (int) (_position_ms / 1000));
}

PlayManager::PlayState PlayManager::get_play_state() const
{
	return _playstate;
}

quint64 PlayManager::get_cur_position_ms() const
{
	return _position_ms;
}

quint64 PlayManager::get_init_position_ms() const
{
	return _initial_position_ms;
}

quint64 PlayManager::get_duration_ms() const
{
	return _md.length_ms;
}

MetaData PlayManager::get_cur_track() const
{
	return _md;
}

int PlayManager::get_volume() const
{
	return _settings->get(Set::Engine_Vol);
}

bool PlayManager::get_mute() const
{
	return _settings->get(Set::Engine_Mute);
}

void PlayManager::play()
{
	if(_playstate == PlayManager::PlayState::Stopped && _cur_idx == -1){
		_playstate = PlayManager::PlayState::Playing;
		next();
		return;
	}

	_playstate = PlayManager::PlayState::Playing;

	emit sig_playstate_changed(_playstate);
}


void PlayManager::play_pause()
{
	if(_playstate == PlayManager::PlayState::Playing){
		pause();
	}

	else{
		play();
	}
}


void PlayManager::pause()
{
	if(_playstate == PlayManager::PlayState::Stopped){
		_playstate = PlayManager::PlayState::Paused;
		next();
		return;
	}

	_playstate = PlayManager::PlayState::Paused;

	emit sig_playstate_changed(_playstate);
}


void PlayManager::previous()
{
	emit sig_previous();
}


void PlayManager::next()
{
	emit sig_next();
}


void PlayManager::stop()
{
	_md = MetaData();
	_ring_buffer.clear();
	_cur_idx = -1;
	_playstate = PlayManager::PlayState::Stopped;
	emit sig_playstate_changed(_playstate);
}


void PlayManager::record(bool b)
{
    emit sig_record(b);
}

void PlayManager::seek_rel(double percent)
{
	emit sig_seeked_rel(percent);
}

void PlayManager::seek_rel_ms(qint64 ms)
{
	emit sig_seeked_rel_ms(ms);
}

void PlayManager::seek_abs_ms(quint64 ms)
{
	emit sig_seeked_abs_ms(ms);
}

void PlayManager::set_position_ms(quint64 ms)
{
	_position_ms = ms;

	if(_position_ms % 1000 == 0){
		_settings->set(Set::Engine_CurTrackPos_s, (int) (_position_ms / 1000));
	}

	emit sig_position_changed_ms(ms);
}

void PlayManager::duration_changed(quint64 duration_ms)
{
	_md.length_ms = duration_ms;
	emit sig_duration_changed(duration_ms);
}


void PlayManager::change_track(const MetaData& md, int playlist_idx)
{
	_md = md;
	_position_ms = 0;
	_cur_idx = playlist_idx;
	_ring_buffer.clear();

	// initial position is outdated now and never needed again
	if(_initial_position_ms > 0){
		int old_idx = _settings->get(Set::PL_LastTrack);
		if(old_idx != _cur_idx){
			_initial_position_ms = 0;
		}
	}

	// play or stop
	if(_cur_idx >= 0){

		emit sig_track_changed(_md);
		emit sig_track_idx_changed(_cur_idx);

		play();
	}

	else {
		sp_log(Log::Info) << "Playlist finished";
		emit sig_playlist_finished();
		stop();
	}

	// save last track
	if(md.db_id == 0){
		_settings->set(Set::PL_LastTrack, _cur_idx);
	}

	else{
		_settings->set(Set::PL_LastTrack, -1);
	}

	// show notification
	if(_settings->get(Set::Notification_Show)){
		if(_cur_idx > -1 && !_md.filepath().isEmpty()){
			NotificationHandler::getInstance()->notify(_md);
		}
	}
}


void PlayManager::set_track_ready()
{
	if(_initial_position_ms > 0){
		sp_log(Log::Debug) << "Track ready, " << (int) (_initial_position_ms / 1000);
		this->seek_abs_ms(_initial_position_ms);
		_initial_position_ms = 0;
	}
}

void PlayManager::buffering(int progress)
{
	emit sig_buffer(progress);
}


void PlayManager::volume_up()
{
	set_volume(_settings->get(Set::Engine_Vol) + 5);
}

void PlayManager::volume_down()
{
	set_volume(_settings->get(Set::Engine_Vol) - 5);
}

void PlayManager::set_volume(int vol)
{
	vol = std::min(vol, 100);
	vol = std::max(vol, 0);
	_settings->set(Set::Engine_Vol, vol);
	emit sig_volume_changed(vol);
}

void PlayManager::set_mute(bool b)
{
	_settings->set(Set::Engine_Mute, b);
	emit sig_mute_changed(b);
}

void PlayManager::change_duration(qint64 ms){
	_md.length_ms = ms;

	emit sig_duration_changed(ms);
}

void PlayManager::change_metadata(const MetaData& md)
{
	MetaData md_old = _md;
	_md = md;

	QString str = md.title + md.artist;
	bool has_data = _ring_buffer.has_item(str);

	if(!has_data){

		if(_settings->get(Set::Notification_Show)){
			NotificationHandler::getInstance()->notify(_md);
		}

		if( _ring_buffer.count() > 0 ){
			md_old.album = "";
			md_old.artist = "";
			md_old.is_disabled = true;
			md_old.set_filepath("");

			QDateTime date = QDateTime::currentDateTime();
			QTime time = date.time();
			md_old.length_ms = (time.hour() * 60 + time.minute()) * 1000;

			emit sig_www_track_finished(md_old);
		}

		_ring_buffer.insert(str);
	}

	emit sig_md_changed(md);
}
