/* GUIRemoteControl.cpp

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
 * Sep 3, 2012 
 *
 */


#include "GUI_RemoteControl.h"
#include "Helper/Helper.h"


GUI_RemoteControl::GUI_RemoteControl(QWidget* parent) :
	PreferenceWidgetInterface(parent),
	Ui::GUI_RemoteControl()
{

}

GUI_RemoteControl::~GUI_RemoteControl() {

}

void GUI_RemoteControl::init_ui()
{
	setup_parent(this);

	revert();

	connect(cb_activate, &QCheckBox::toggled, this, &GUI_RemoteControl::active_toggled);
	connect(sb_port, spinbox_value_changed_int, this, &GUI_RemoteControl::port_changed);
}


void GUI_RemoteControl::language_changed() {

	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);

	PreferenceWidgetInterface::language_changed();
}

void GUI_RemoteControl::commit(){

	_settings->set(Set::Remote_Active, cb_activate->isChecked());
	_settings->set(Set::Remote_Port, sb_port->value());
}

void GUI_RemoteControl::revert(){
	cb_activate->setChecked(_settings->get(Set::Remote_Active));
	sb_port->setValue(_settings->get(Set::Remote_Port));
	refresh_url();
}


QString GUI_RemoteControl::get_action_name() const
{
	return tr("Remote control");
}



void GUI_RemoteControl::active_toggled(bool b)
{
	Q_UNUSED(b)
	refresh_url();
}

void GUI_RemoteControl::port_changed(int port)
{
	Q_UNUSED(port)
	refresh_url();
}


QString GUI_RemoteControl::get_url_string()
{
	int port = sb_port->value();
	QStringList ips = Helper::get_ip_addresses();

	QStringList ret;
	for(const QString& ip : ips){
		QString str = QString("http://") + ip + ":" + QString::number(port);
		ret << str;
	}

	return ret.join("; ");
}

void GUI_RemoteControl::refresh_url()
{
	bool active = cb_activate->isChecked();

	le_url->setVisible(active);
	lab_url->setVisible(active);
	le_url->setText(get_url_string());
}



