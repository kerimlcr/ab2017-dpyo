/* DBusMPRIS.cpp */

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




#include "DBusMPRIS.h"
#include "Components/DBus/org_mpris_media_player2_adaptor.h"
#include "Components/DBus/org_mpris_media_player2_player_adaptor.h"
#include "Components/Covers/CoverLocation.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"

#include "Helper/Random/RandomGenerator.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QStringList>
#include <QUrl>


DBusAdaptor::DBusAdaptor(QObject* parent) :
	QObject(parent)
{
		_play_manager = PlayManager::getInstance();
}

DBusAdaptor::~DBusAdaptor(){

}



void DBusAdaptor::create_message(QString name, QVariant val){

	QDBusMessage sig;
	QVariantMap map;
	QVariantList args;
	bool success;

	map.insert(name, val);
	args << _dbus_service << map << QStringList();

	// path, interface, name
	sig = QDBusMessage::createSignal(_object_path, _dbus_interface, "PropertiesChanged");
	sig.setArguments(args);

	success = QDBusConnection::sessionBus().send(sig);
	Q_UNUSED(success)
	/*QDBusError err = QDBusConnection::sessionBus().lastError();
	sp_log(Log::Debug) << "Send signal: " << name << ": " << success << ": " << err.message();*/


}



DBusMPRIS::MediaPlayer2::MediaPlayer2(SayonaraMainWindow* player, QObject *parent) :
	DBusAdaptor(parent),
	SayonaraClass()
{
	_initialized = false;
	_player = player;

	_object_path = "/org/mpris/MediaPlayer2";
	_dbus_service = "org.mpris.MediaPlayer2.Player";
	_service_name = "org.mpris.MediaPlayer2.sayonara";
	_dbus_interface = "org.freedesktop.DBus.Properties";
	_can_next = true;
	_can_previous = true;

	_pos = 0;
	_volume = 1.0;

	connect(_play_manager, &PlayManager::sig_playlist_changed,
			this, &DBusMPRIS::MediaPlayer2::playlist_len_changed);
	connect(_play_manager, &PlayManager::sig_playstate_changed,
			this, &DBusMPRIS::MediaPlayer2::playstate_changed);
	connect(_play_manager, &PlayManager::sig_track_changed,
			this, &DBusMPRIS::MediaPlayer2::track_changed);
	connect(_play_manager, &PlayManager::sig_track_idx_changed,
			this, &DBusMPRIS::MediaPlayer2::track_idx_changed);
	connect(_play_manager, &PlayManager::sig_position_changed_ms,
			this, &DBusMPRIS::MediaPlayer2::position_changed);
	connect(_play_manager, &PlayManager::sig_volume_changed,
			this, &DBusMPRIS::MediaPlayer2::volume_changed);

	if( _play_manager->get_play_state() == PlayManager::PlayState::Playing ||
		_play_manager->get_play_state() == PlayManager::PlayState::Paused)
	{
		track_changed(_play_manager->get_cur_track());
	}
}


DBusMPRIS::MediaPlayer2::~MediaPlayer2()
{
	QDBusConnection::sessionBus().unregisterObject(_object_path);
	QDBusConnection::sessionBus().unregisterService(_service_name);
}

void DBusMPRIS::MediaPlayer2::init(){

	if(_initialized){
		return;
	}

	new OrgMprisMediaPlayer2Adaptor(this);
	new OrgMprisMediaPlayer2PlayerAdaptor(this);

	if (!QDBusConnection::sessionBus().registerService(_service_name)) {
		sp_log(Log::Error)	<< "Failed to register "
							<< _service_name
							<< " on the session bus";

		_initialized = true;

		return;
	}

	sp_log(Log::Info) << "DBus: " << _service_name << " registered";

	QDBusConnection::sessionBus().registerObject(_object_path, this);
	create_message("DesktopEntry", QString("sayonara"));

	_initialized = true;
}


bool DBusMPRIS::MediaPlayer2::CanQuit()
{
	return true;
}

bool DBusMPRIS::MediaPlayer2::CanRaise()
{
	return true;
}

bool DBusMPRIS::MediaPlayer2::HasTrackList(){
	return false;
}

QString DBusMPRIS::MediaPlayer2::Identity(){
	return QString("Sayonara Player");
}

QString DBusMPRIS::MediaPlayer2::DesktopEntry(){
	return QString("sayonara");
}

QStringList DBusMPRIS::MediaPlayer2::SupportedUriSchemes(){
	QStringList uri_schemes;
	uri_schemes << "file"
				<< "http"
				<< "cdda"
				<< "smb"
				<< "sftp";

	return uri_schemes;
}

QStringList DBusMPRIS::MediaPlayer2::SupportedMimeTypes(){
	QStringList mimetypes;
	mimetypes   << "audio/mpeg"
				<< "audio/ogg";

	return mimetypes;
}

bool DBusMPRIS::MediaPlayer2::CanSetFullscreen(){
	return true;
}

bool DBusMPRIS::MediaPlayer2::Fullscreen(){
	return _settings->get(Set::Player_Fullscreen);
}


void DBusMPRIS::MediaPlayer2::SetFullscreen(bool b){
	_settings->set(Set::Player_Fullscreen, b);
}


void DBusMPRIS::MediaPlayer2::Quit(){
	_player->close();
}

void DBusMPRIS::MediaPlayer2::Raise(){
	sp_log(Log::Debug) << "Raise";
	_player->show();
	_player->raise();
	_player->show();
	_player->raise();

}


/*** mpris.mediaplayer2.player ***/

QString DBusMPRIS::MediaPlayer2::PlaybackStatus(){

	return _playback_status;
}

QString DBusMPRIS::MediaPlayer2::LoopStatus(){
	return "None";
}

double DBusMPRIS::MediaPlayer2::Rate(){
	return 1.0;
}

bool DBusMPRIS::MediaPlayer2::Shuffle(){
	return false;
}


QVariantMap DBusMPRIS::MediaPlayer2::Metadata(){

	QString cover_path;
	CoverLocation cl;

	QVariantMap map;
	QVariant v_object_path, v_length;


	int id = _md.id;
	if(id == -1){
		id = RandomGenerator::get_random_number(5000, 10000);
	}
	QDBusObjectPath object_path(QString("/org/sayonara/track") + QString::number(id));

	v_object_path.setValue<QDBusObjectPath>(object_path);
	v_length.setValue<qlonglong>(_md.length_ms * 1000);

	cl = CoverLocation::get_cover_location(_md);
	if(!cl.local_paths().isEmpty()){
		cover_path = cl.local_path(0);
	}

	else if(!cl.cover_path().isEmpty()){
		cover_path = cl.cover_path();
	}

	else{
		cover_path = CoverLocation::getInvalidLocation().cover_path();
	}

	map["mpris:trackid"] = v_object_path;
	map["mpris:length"] = v_length;
	map["xesam:title"] = _md.title;
	map["xesam:album"] = _md.album;
	map["xesam:artist"] = QStringList() << _md.artist;
	map["mpris:artUrl"] = QUrl::fromLocalFile(cover_path).toString();

	return map;
}

double DBusMPRIS::MediaPlayer2::Volume(){
	return _volume;
}

qint64 DBusMPRIS::MediaPlayer2::Position(){
	return _pos;
}


double DBusMPRIS::MediaPlayer2::MinimumRate(){
	return 1.0;
}

double DBusMPRIS::MediaPlayer2::MaximumRate(){
	return 1.0;
}

bool DBusMPRIS::MediaPlayer2::CanGoNext(){
	return _can_next;
}

bool DBusMPRIS::MediaPlayer2::CanGoPrevious(){
	return _can_previous;
}

bool DBusMPRIS::MediaPlayer2::CanPlay(){
	return true;
}

bool DBusMPRIS::MediaPlayer2::CanPause(){
	return true;
}

bool DBusMPRIS::MediaPlayer2::CanSeek(){
	return true;
}

bool DBusMPRIS::MediaPlayer2::CanControl(){
	return true;
}


void DBusMPRIS::MediaPlayer2::Next(){
	_play_manager->next();
}


void DBusMPRIS::MediaPlayer2::Previous(){

	_play_manager->previous();
}

void DBusMPRIS::MediaPlayer2::Pause(){
	_play_manager->pause();
}


void DBusMPRIS::MediaPlayer2::PlayPause(){
	_play_manager->play_pause();
}

void DBusMPRIS::MediaPlayer2::Stop(){
	_play_manager->stop();
}

void DBusMPRIS::MediaPlayer2::Play(){

	_playback_status = "Playing";
	_play_manager->play();
}

void DBusMPRIS::MediaPlayer2::Seek(qint64 offset){
	_play_manager->seek_rel_ms(offset / 1000);
}

void DBusMPRIS::MediaPlayer2::SetPosition(const QDBusObjectPath& track_id, qint64 position){
	Q_UNUSED(track_id)
	_play_manager->seek_abs_ms(position / 1000);
}

void DBusMPRIS::MediaPlayer2::OpenUri(const QString& uri){
	Q_UNUSED(uri)
}

void DBusMPRIS::MediaPlayer2::SetLoopStatus(QString status){
	Q_UNUSED(status)
}

void DBusMPRIS::MediaPlayer2::SetRate(double rate){
	Q_UNUSED(rate)
}

void DBusMPRIS::MediaPlayer2::SetShuffle(bool shuffle){
	Q_UNUSED(shuffle)
}

void DBusMPRIS::MediaPlayer2::SetVolume(double volume){

	_play_manager->set_volume((int) (volume * 100));
}


void DBusMPRIS::MediaPlayer2::volume_changed(int volume){
	if(!_initialized){
		init();
	}

	create_message("Volume", volume / 100.0);
}


void DBusMPRIS::MediaPlayer2::position_changed(quint64 pos){

	if(!_initialized){
		init();
	}
	qint64 new_pos = pos * 1000;
	qint64 difference = new_pos - _pos;

	if(difference < 0 || difference > 1000000){
		emit Seeked(new_pos);
	}

	_pos = new_pos;
}


void DBusMPRIS::MediaPlayer2::track_idx_changed(int idx){

	if(!_initialized){
		init();
	}
	_can_previous = (idx > 0);
	_can_next = (idx < _len_playlist - 1);

	create_message("CanGoNext", _can_next);
	create_message("CanGoPrevious", _can_previous);

	_cur_idx = idx;
}

void DBusMPRIS::MediaPlayer2::playlist_len_changed(int len){

	if(!_initialized){
		init();
	}
	_can_next = (_cur_idx < len - 1 && _cur_idx >= 0);

	create_message("CanGoNext", _can_next);

	_len_playlist = len;
}

void DBusMPRIS::MediaPlayer2::track_changed(const MetaData& md){

	_md = md;
	if(!_initialized){
		init();
	}

	QVariantMap map = Metadata();
	create_message("Metadata", map);
}

void DBusMPRIS::MediaPlayer2::playstate_changed(PlayManager::PlayState state){

	QString playback_status;
	if(!_initialized){
		init();
	}

	switch(state){
		case PlayManager::PlayState::Stopped:
			playback_status = "Stopped";
			break;
		case PlayManager::PlayState::Playing:
			playback_status = "Playing";
			break;
		case PlayManager::PlayState::Paused:
			playback_status = "Paused";
			break;
		default:
			playback_status = "Stopped";
			break;
	}

	_playback_status = playback_status;

	create_message("PlaybackStatus", playback_status);
}


