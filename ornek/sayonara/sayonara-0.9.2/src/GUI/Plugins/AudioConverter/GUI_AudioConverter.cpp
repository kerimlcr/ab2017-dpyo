/* GUI_AudioConverter.cpp */

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



#include "GUI_AudioConverter.h"
#include "GUI/Helper/Message/Message.h"

#include "Components/Engine/EngineHandler.h"

#include <QFileDialog>


GUI_AudioConverter::GUI_AudioConverter(QWidget *parent) :
	PlayerPluginInterface(parent),
	Ui::GUI_AudioConvert()
{
	_engine = EngineHandler::getInstance();
	_mp3_enc_available = true;
}


QString GUI_AudioConverter::get_name() const
{
	return "Audio Converter";
}

QString GUI_AudioConverter::get_display_name() const
{
	return tr("Audio Converter");
}

void GUI_AudioConverter::language_changed(){

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
}

void GUI_AudioConverter::init_ui()
{

	if(is_ui_initialized()){
		return;
	}

	setup_parent(this);

	LameBitrate br = (LameBitrate) _settings->get(Set::Engine_ConvertQuality);

	rb_cbr->setChecked(false);
	rb_vbr->setChecked(false);

	connect(rb_cbr, &QRadioButton::toggled, this, &GUI_AudioConverter::rb_cbr_toggled);
	connect(rb_vbr, &QRadioButton::toggled, this, &GUI_AudioConverter::rb_vbr_toggled);
	connect(cb_quality,	combo_current_index_changed_int, this, &GUI_AudioConverter::quality_changed);
	connect(cb_active, &QCheckBox::toggled, this, &GUI_AudioConverter::cb_active_toggled);

	int idx = -1;

	switch(br) {
		case LameBitrate_64:
		case LameBitrate_128:
		case LameBitrate_192:
		case LameBitrate_256:
		case LameBitrate_320:
			rb_cbr->setChecked(true);
			idx = cb_quality->findData((int) br);
			if(idx < 0 || idx >= cb_quality->count()) break;
			else cb_quality->setCurrentIndex(idx);
			break;

		default:
			rb_vbr->setChecked(true);
			idx = cb_quality->findData((int) br);
			if(idx < 0 || idx >= cb_quality->count()) break;
			else cb_quality->setCurrentIndex(idx);
			break;
	}

	REGISTER_LISTENER(SetNoDB::MP3enc_found, mp3_enc_found);
}


void GUI_AudioConverter::fill_cbr() {

	if(!is_ui_initialized()){
		return;
	}

	disconnect(cb_quality,	combo_current_index_changed_int, this, &GUI_AudioConverter::quality_changed);
	cb_quality->clear();

	cb_quality->addItem("64", LameBitrate_64);
	cb_quality->addItem("128", LameBitrate_128);
	cb_quality->addItem("192", LameBitrate_192);
	cb_quality->addItem("256", LameBitrate_256);
	cb_quality->addItem("320", LameBitrate_320);
	connect(cb_quality,	combo_current_index_changed_int, this, &GUI_AudioConverter::quality_changed);

	cb_quality->setCurrentIndex(2);
}

void GUI_AudioConverter::fill_vbr() {

	if(!is_ui_initialized()){
		return;
	}

	disconnect(cb_quality,	combo_current_index_changed_int, this, &GUI_AudioConverter::quality_changed);
	cb_quality->clear();

	cb_quality->addItem(tr("0 (Best)"), LameBitrate_var_0);
	cb_quality->addItem("1", LameBitrate_var_1);
	cb_quality->addItem("2", LameBitrate_var_2);
	cb_quality->addItem("3", LameBitrate_var_3);
	cb_quality->addItem("4", LameBitrate_var_4);
	cb_quality->addItem("5", LameBitrate_var_5);
	cb_quality->addItem("6", LameBitrate_var_6);
	cb_quality->addItem("7", LameBitrate_var_7);
	cb_quality->addItem("8", LameBitrate_var_8);
	cb_quality->addItem(tr("9 (Worst)"), LameBitrate_var_9);
	connect(cb_quality,	combo_current_index_changed_int, this, &GUI_AudioConverter::quality_changed);

	cb_quality->setCurrentIndex(2);
}


void GUI_AudioConverter::stopped(){

	if(!isVisible()) return;
	if(!cb_active->isChecked()) return;

	cb_active->setChecked( false );
}


void GUI_AudioConverter::rb_cbr_toggled(bool b) {

	if(!is_ui_initialized()){
		return;
	}

	if(!b) {
		return;
	}

	fill_cbr();
}

void GUI_AudioConverter::rb_vbr_toggled(bool b) {

	if(!is_ui_initialized()){
		return;
	}

	if(!b) {
		return;
	}

	fill_vbr();
}

void GUI_AudioConverter::pl_mode_backup(){
	_pl_mode = _settings->get(Set::PL_Mode);

	PlaylistMode new_mode;
		new_mode.setAppend(false, false);
		new_mode.setRep1(false, false);
		new_mode.setRepAll(false, false);
		new_mode.setShuffle(false, false);
		new_mode.setGapless(false, false);
		new_mode.setDynamic(false, false);

	_settings->set(Set::PL_Mode, new_mode);
}

void GUI_AudioConverter::pl_mode_restore(){
	_settings->set(Set::PL_Mode, _pl_mode);
}

void GUI_AudioConverter::cb_active_toggled(bool b) {

	if(!is_ui_initialized()){
		return;
	}

	if(!_mp3_enc_available){
		Message::warning(tr("Cannot find lame mp3 encoder"));
		disconnect(cb_active, &QCheckBox::toggled, this, &GUI_AudioConverter::cb_active_toggled);
		cb_active->setChecked(false);
		connect(cb_active, &QCheckBox::toggled, this, &GUI_AudioConverter::cb_active_toggled);
		return;
	}

	if(b) {

		QString cvt_target_path = _settings->get(Set::Engine_CovertTargetPath);
		QString dir = QFileDialog::getExistingDirectory(this, "Choose target directory", cvt_target_path);

		if(dir.size() > 0) {
			_settings->set(Set::Engine_CovertTargetPath, dir);
			pl_mode_backup();

			_engine->start_convert();
		}

		else {
			disconnect(cb_active, &QCheckBox::toggled, this, &GUI_AudioConverter::cb_active_toggled);
			cb_active->setChecked(false);
			connect(cb_active, &QCheckBox::toggled, this, &GUI_AudioConverter::cb_active_toggled);
		}
	}

	else {
		pl_mode_restore();
		_engine->end_convert();
	}
}

void GUI_AudioConverter::quality_changed(int index) {

	if(!is_ui_initialized()){
		return;
	}

	LameBitrate q = (LameBitrate) cb_quality->itemData(index).toInt();
	sp_log(Log::Info) << "Quality: " << q;
	_settings->set(Set::Engine_ConvertQuality, (int) q);
}

void GUI_AudioConverter::mp3_enc_found(){
	_mp3_enc_available = _settings->get(SetNoDB::MP3enc_found);
}
