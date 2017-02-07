/* GUI_Notifications.cpp */

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

#include "GUI_Notifications.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"

GUI_Notifications::GUI_Notifications(QWidget *parent) :
	PreferenceWidgetInterface(parent),
	Ui::GUI_Notifications()
{

}

GUI_Notifications::~GUI_Notifications() {
}

void GUI_Notifications::language_changed(){

	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);

	PreferenceWidgetInterface::language_changed();
}


void GUI_Notifications::notifications_changed(){

	NotificatonList notifications = _notification_handler->get_notificators();

	combo_notification->clear();
	for(const NotificationInterface* notification : notifications){
		combo_notification->addItem(notification->get_name());
	}

	combo_notification->setCurrentIndex(_notification_handler->get_cur_idx());
}


void GUI_Notifications::commit() {

	bool active =       cb_activate->isChecked();
	int timeout =       sb_timeout->value();
	QString cur_text =  combo_notification->currentText();
    
	_settings->set(Set::Notification_Name, cur_text);
	_settings->set(Set::Notification_Timeout, timeout);
	_settings->set(Set::Notification_Show, active);

	_notification_handler->notificator_changed(cur_text);
}

void GUI_Notifications::revert() {

	int timeout = _settings->get(Set::Notification_Timeout);
	int active = _settings->get(Set::Notification_Show);

	sb_timeout->setValue(timeout);
	cb_activate->setChecked(active);
}


QString GUI_Notifications::get_action_name() const
{
	return tr("Notifications");
}

void GUI_Notifications::init_ui()
{
	setup_parent(this);

	_notification_handler = NotificationHandler::getInstance();
	combo_notification->setItemDelegate(new ComboBoxDelegate(combo_notification));

	revert();

	notifications_changed();

	connect(_notification_handler,	&NotificationHandler::sig_notifications_changed,
			this,					&GUI_Notifications::notifications_changed);
}
