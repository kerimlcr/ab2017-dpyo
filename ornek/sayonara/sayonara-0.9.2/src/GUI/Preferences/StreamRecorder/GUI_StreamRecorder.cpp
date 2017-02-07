/* GUI_StreamRecorder.cpp

 * Copyright (C) 2012-2016 Lucio Carreras  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * May 13, 2012 
 *
 */

#include "GUI_StreamRecorder.h"
#include "GUI/Helper/Message/Message.h"
#include "Database/DatabaseConnector.h"

#include <QFileDialog>
#include <QDir>

GUI_StreamRecorder::GUI_StreamRecorder(QWidget* parent) :
	PreferenceWidgetInterface(parent),
	Ui::GUI_StreamRecorder()
{

}


GUI_StreamRecorder::~GUI_StreamRecorder() {

}

void GUI_StreamRecorder::init_ui()
{
	setup_parent(this);

	revert();

	connect(cb_activate, &QCheckBox::toggled, this, &GUI_StreamRecorder::sl_cb_activate_toggled);
	connect(cb_create_session_path, &QCheckBox::toggled, this, &GUI_StreamRecorder::sl_cb_create_session_path_toggled);
	connect(btn_path, &QPushButton::clicked, this, &GUI_StreamRecorder::sl_btn_path_clicked);
}


void GUI_StreamRecorder::language_changed() {

	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
	PreferenceWidgetInterface::language_changed();

}


void GUI_StreamRecorder::sl_cb_activate_toggled(bool b) {
	_is_active = b;
	_settings->set(Set::Engine_SR_Active, b);

	le_path->setEnabled(b);
	btn_path->setEnabled(b);
	cb_create_session_path->setEnabled(b);
}


void GUI_StreamRecorder::sl_cb_create_session_path_toggled(bool b) {
    _is_create_session_path = b;
	_settings->set(Set::Engine_SR_SessionPath, b);
}


void GUI_StreamRecorder::sl_btn_path_clicked() {

	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose target directory"), _path, QFileDialog::ShowDirsOnly);
	if(dir.size() > 0) {
		_path = dir;
		_settings->set(Set::Engine_SR_Path, _path);
		le_path->setText(_path);
	}
}


void GUI_StreamRecorder::commit() {

	_settings->set(Set::Engine_SR_Active, cb_activate->isChecked());

	if(!le_path->isEnabled()){
		return;
	}

	QString str = le_path->text();
    if(!QFile::exists(str)) {
        bool create_success = QDir::root().mkpath(str);
        if(!create_success) {
			QString sr_path = _settings->get(Set::Engine_SR_Path);
			le_path->setText(sr_path);

			Message::warning(str + tr(" could not be created\nPlease choose another folder"), tr("Stream recorder"));
        }
    }

	_settings->set(Set::Engine_SR_Path, str);

    _path = str;
}

void GUI_StreamRecorder::revert(){

	_path = _settings->get(Set::Engine_SR_Path);
	_is_active = _settings->get(Set::Engine_SR_Active);
	_is_create_session_path = _settings->get(Set::Engine_SR_SessionPath);

	le_path->setText(_path);
	cb_activate->setChecked(_is_active);
	cb_create_session_path->setChecked(_is_create_session_path);

	cb_create_session_path->setEnabled(_is_active);
	btn_path->setEnabled(_is_active);
	le_path->setEnabled(_is_active);
}


void GUI_StreamRecorder::record_button_toggled(bool b) {

	btn_path->setEnabled(!b);
	cb_activate->setEnabled(!b);
	cb_create_session_path->setEnabled(!b);
	le_path->setEnabled(!b);
}


QString GUI_StreamRecorder::get_action_name() const
{
	return tr("Stream recorder");
}
