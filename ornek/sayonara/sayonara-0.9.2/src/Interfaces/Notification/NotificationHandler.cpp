
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


#include "NotificationHandler.h"
#include "Helper/Settings/Settings.h"
#include <algorithm>

NotificationHandler::NotificationHandler(QObject* parent) :
	QObject(parent)
{
	_cur_idx = -1;
}

NotificationHandler::~NotificationHandler(){

}

void NotificationHandler::notify(const MetaData& md){
	get()->notify(md);
}

void NotificationHandler::notify(const QString& title, const QString& message, const QString& image_path){
	get()->notify(title, message, image_path);
}


void NotificationHandler::register_notificator(NotificationInterface* notificator)
{
	_notificators << notificator;

	QString preferred = Settings::getInstance()->get(Set::Notification_Name);

	auto lambda = [preferred](NotificationInterface* n){
		return (n->get_name().compare(preferred, Qt::CaseInsensitive) == 0);
	};

	auto it = std::find_if(_notificators.begin(), _notificators.end(), lambda);
	_cur_idx = (it - _notificators.begin());

	if(_cur_idx >= _notificators.size()){
		_cur_idx = 0;
	}

	emit sig_notifications_changed();
}


void NotificationHandler::notificator_changed(const QString& name){

	_cur_idx = -1;
	int i = 0;

	for(NotificationInterface* n : _notificators){

		if(n->get_name().compare(name, Qt::CaseInsensitive) == 0){
			_cur_idx = i;
			break;
		}

		i++;
	}
}

NotificationInterface* NotificationHandler::get() const
{
	if(_cur_idx < 0){
		static DummyNotificator dummy("Dummy");
		return &dummy;
	}

	return _notificators[_cur_idx];
}


NotificatonList NotificationHandler::get_notificators() const
{
	return _notificators;
}

int NotificationHandler::get_cur_idx() const
{
	return _cur_idx;
}


DummyNotificator::DummyNotificator(const QString& name) :
	NotificationInterface(name)
{

}

void DummyNotificator::notify(const MetaData &md){
	Q_UNUSED(md)
}

void DummyNotificator::notify(const QString &title, const QString &message, const QString &image_path)
{
	Q_UNUSED(title)
	Q_UNUSED(message)
	Q_UNUSED(image_path)
}
