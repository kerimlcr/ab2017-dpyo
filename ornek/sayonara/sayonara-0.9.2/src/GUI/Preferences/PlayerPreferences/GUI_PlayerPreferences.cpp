/* GUI_PlayerPreferences.cpp */

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



#include "GUI_PlayerPreferences.h"


GUI_PlayerPreferences::GUI_PlayerPreferences(QWidget *parent) :
	PreferenceWidgetInterface(parent),
	Ui::GUI_PlayerPreferences()
{

}

GUI_PlayerPreferences::~GUI_PlayerPreferences()
{

}



void GUI_PlayerPreferences::init_ui()
{
	setup_parent(this);

	revert();

	connect(cb_show_tray_icon, &QCheckBox::toggled, this, &GUI_PlayerPreferences::show_tray_icon_toggled);
}

QString GUI_PlayerPreferences::get_action_name() const
{
	return tr("Player");
}


void GUI_PlayerPreferences::commit()
{
	bool show_tray_icon = _settings->get(Set::Player_ShowTrayIcon);

	_settings->set(Set::Player_Min2Tray, cb_close_to_tray->isChecked() && show_tray_icon);
	_settings->set(Set::Player_StartInTray, cb_start_in_tray->isChecked() && show_tray_icon);

	_settings->set(Set::Player_ShowTrayIcon, cb_show_tray_icon->isChecked());
	_settings->set(Set::Player_NotifyNewVersion, cb_update_notifications->isChecked());
}

void GUI_PlayerPreferences::revert()
{
	bool show_tray_icon = _settings->get(Set::Player_ShowTrayIcon);

	cb_start_in_tray->setChecked(_settings->get(Set::Player_StartInTray));
	cb_close_to_tray->setChecked(_settings->get(Set::Player_Min2Tray));
	cb_update_notifications->setChecked(_settings->get(Set::Player_NotifyNewVersion));
	cb_show_tray_icon->setChecked(_settings->get(Set::Player_ShowTrayIcon));

	show_tray_icon_toggled(show_tray_icon);
}

void GUI_PlayerPreferences::show_tray_icon_toggled(bool b)
{
	cb_start_in_tray->setEnabled(b);
	cb_close_to_tray->setEnabled(b);

	if(!b){
		_settings->set(Set::Player_Min2Tray, false);
	}
}

void GUI_PlayerPreferences::language_changed(){

	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);

	PreferenceWidgetInterface::language_changed();
}
