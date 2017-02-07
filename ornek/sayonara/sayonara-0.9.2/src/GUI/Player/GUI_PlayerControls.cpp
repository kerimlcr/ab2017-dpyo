// GUI_PlayerButtons.cpp

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


#include "GUI_Player.h"
#include "GUI_TrayIcon.h"
#include "GUI/Helper/IconLoader/IconLoader.h"
#include "Helper/Helper.h"

#include <QFileDialog>
#include <QToolTip>

#include <algorithm>

/** PLAYER BUTTONS **/
void GUI_Player::playstate_changed(PlayManager::PlayState state)
{
	switch(state){
		case PlayManager::PlayState::Playing:
			played();
			break;
		case PlayManager::PlayState::Paused:
			paused();
			break;
		case PlayManager::PlayState::Stopped:
			stopped();
			break;

		default:
			return;
	}

	return;
}


void GUI_Player::play_clicked()
{
	_play_manager->play_pause();
}


void GUI_Player::played()
{
	btn_play->setIcon(_icon_loader->get_icon("media-playback-pause", "pause"));
}


void GUI_Player::paused()
{
	btn_play->setIcon(_icon_loader->get_icon("media-playback-start", "play"));
}


void GUI_Player::stop_clicked()
{
	_play_manager->stop();
}


void GUI_Player::stopped()
{
	setWindowTitle("Sayonara");

	btn_play->setIcon(_icon_loader->get_icon("media-playback-start", "play"));

	progress_widget->setCurrentIndex(0);

	lab_title->hide();
	lab_sayonara->show();

	lab_artist->hide();
	lab_writtenby->show();

	lab_album->hide();
	lab_version->show();

	lab_rating->hide();
	lab_copyright->show();

	sli_progress->setValue(0);
	sli_progress->setEnabled(false);

	curTime->setText("00:00");
	maxTime->setText("");

	set_standard_cover();

	if(btn_rec->isVisible() && btn_rec->isChecked()) {
		btn_rec->setChecked(false);
	}
}


void GUI_Player::prev_clicked()
{
	_play_manager->previous();
}


void GUI_Player::next_clicked()
{
	_play_manager->next();
}


void GUI_Player::rec_clicked(bool b)
{
    _play_manager->record(b);
}



void GUI_Player::buffering(int progress)
{
	if(progress >= 0 && progress < 100){
		progress_widget->setCurrentIndex(1);
		sli_buffer->setValue(progress);
	}

	else{
		progress_widget->setCurrentIndex(0);
	}
}


void GUI_Player::set_progress_tooltip(int val)
{
	int max = sli_progress->maximum();

	val = std::max(val, 0);
	val = std::min(max, val);

	double percent = (val * 1.0) / max;
	quint64 cur_pos_ms =  (quint64) (percent * _md.length_ms);
	QString cur_pos_string = Helper::cvt_ms_to_string(cur_pos_ms);

	QToolTip::showText( QCursor::pos(), cur_pos_string );
}


void GUI_Player::set_cur_pos_label(int val){

	int max = sli_progress->maximum();

	val = std::max(val, 0);
	val = std::min(max, val);

	double percent = (val * 1.0) / max;
	quint64 cur_pos_ms =  (quint64) (percent * _md.length_ms);
	QString cur_pos_string = Helper::cvt_ms_to_string(cur_pos_ms);

	curTime->setText(cur_pos_string);
}


void GUI_Player::set_total_time_label(qint64 total_time) {

	_md.length_ms = total_time;

	QString length_str;
	if(total_time > 0){
		length_str = Helper::cvt_ms_to_string(total_time, true);
	}

	maxTime->setText(length_str);
	sli_progress->setEnabled(total_time > 0);
}


void GUI_Player::set_file_info_label()
{
		QString rating_text;


	if(_md.bitrate / 1000 > 0){
		rating_text = QString::number(_md.bitrate / 1000) + " kBit/s";
	}

	if(_md.filesize > 0){
		if(!rating_text.isEmpty()){
			rating_text += ", ";
		}

		rating_text += QString::number( (double) (_md.filesize / 1024) / 1024.0, 'f', 2) + " MB";
	}

	if( (_settings->get(Set::Engine_Pitch) != 440) &&
		 _settings->get(Set::Engine_SpeedActive))
	{
		if(!rating_text.isEmpty()){
			rating_text += ", ";
		}

		rating_text += QString::number(_settings->get(Set::Engine_Pitch)) + "Hz";
	}

	lab_rating->setText(rating_text);
	lab_rating->setToolTip(rating_text);
}


void GUI_Player::jump_forward_ms()
{
	_play_manager->seek_rel_ms(10000);
}


void GUI_Player::jump_backward_ms()
{
	_play_manager->seek_rel_ms(-10000);
}


void GUI_Player::jump_forward()
{
	sli_progress->increment(50);
}


void GUI_Player::jump_backward()
{
	sli_progress->increment(-50);
}


void GUI_Player::seek(int val)
{
	val = std::max(val, 0);

	set_cur_pos_label(val);

	double percent = (val * 1.0) / sli_progress->maximum();
	_play_manager->seek_rel(percent);
}


void GUI_Player::set_cur_pos_ms(quint64 pos_ms)
{
	int max = sli_progress->maximum();
	int new_val;

	if ( _md.length_ms > 0 ) {
		new_val = ( pos_ms * max ) / (_md.length_ms);
	}

	else if(pos_ms > _md.length_ms) {
		new_val = 0;
    }

	else{
		return;
	}

	if(!sli_progress->is_busy()){
		QString cur_pos_string = Helper::cvt_ms_to_string(pos_ms);
		curTime->setText(cur_pos_string);
		sli_progress->setValue(new_val);
	}
}


void GUI_Player::volume_slider_moved(int val)
{
	_play_manager->set_volume(val);
}


void GUI_Player::volume_changed(int val)
{
	setup_volume_button(val);
	sli_volume->setValue(val);
}


void GUI_Player::change_volume_by_tick(int val)
{
	if(val > 0){
		increase_volume();
	}
	else{
		decrease_volume();
	}
}


void GUI_Player::increase_volume()
{
	_play_manager->volume_up();
}


void GUI_Player::decrease_volume()
{
	_play_manager->volume_down();
}


void GUI_Player::setup_volume_button(int percent)
{
	QString but_name = "vol_";
	QString but_std_name = "vol_";

    if (percent <= 1) {
		but_name += QString("mute_dark");
		but_std_name = QString("audio-volume-muted");
	}

	else if (percent < 40) {
		but_name += QString("1_dark");
		but_std_name = QString("audio-volume-low");
	}

	else if (percent < 80) {
		but_name += QString("2_dark");
		but_std_name = QString("audio-volume-medium");
	}

	else {
		but_name += QString("3_dark");
		but_std_name = QString("audio-volume-high");
	}

	btn_mute->setIcon( _icon_loader->get_icon(but_std_name, but_name));
}


void GUI_Player::mute_button_clicked()
{
	bool muted = _settings->get(Set::Engine_Mute);
	_play_manager->set_mute(!muted);
}


void GUI_Player::mute_changed(bool muted)
{
	int val;
	sli_volume->setDisabled(muted);

	if(muted){
		val = 0;
	}

	else {
		val = _settings->get(Set::Engine_Vol);
	}

	sli_volume->setValue(val);
	setup_volume_button(val);
}
