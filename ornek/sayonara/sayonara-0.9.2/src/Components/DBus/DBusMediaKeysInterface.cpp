/* DBusMediaKeysInterface.cpp */

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



#include "DBusMediaKeysInterface.h"
#include "Helper/Logger/Logger.h"

#include <QKeyEvent>
#include <QCoreApplication>

DBusMediaKeysInterface::DBusMediaKeysInterface(QObject *parent) :
	QObject(parent)
{
	_parent = parent;
	_play_manager = PlayManager::getInstance();
	_initialized = false;
}


void DBusMediaKeysInterface::sl_media_key_pressed(const QString& program_name, const QString& key){

	Q_UNUSED(program_name)

	QKeyEvent *event = nullptr;

	if(key.compare("play", Qt::CaseInsensitive) == 0){
		event = new QKeyEvent ( QEvent::KeyPress, Qt::Key_MediaPlay, Qt::NoModifier);
		_play_manager->play_pause();
	}

	else if(key.compare("pause", Qt::CaseInsensitive) == 0){
		event = new QKeyEvent ( QEvent::KeyPress, Qt::Key_MediaPause, Qt::NoModifier);
		_play_manager->pause();
	}

	else if(key.compare("next", Qt::CaseInsensitive) == 0){
		event = new QKeyEvent ( QEvent::KeyPress, Qt::Key_MediaNext, Qt::NoModifier);
		_play_manager->next();
	}

	else if(key.compare("previous", Qt::CaseInsensitive) == 0){
		event = new QKeyEvent ( QEvent::KeyPress, Qt::Key_MediaPrevious, Qt::NoModifier);
		_play_manager->previous();
	}

	else if(key.contains("stop", Qt::CaseInsensitive)){
		event = new QKeyEvent (QEvent::KeyPress, Qt::Key_MediaStop, Qt::NoModifier);
		_play_manager->stop();
	}

	if(event && _parent){
		QCoreApplication::postEvent (_parent, event);
	}
}




void DBusMediaKeysInterface::sl_register_finished(QDBusPendingCallWatcher* watcher){

	QDBusMessage reply = watcher->reply();
	watcher->deleteLater();

	if (reply.type() == QDBusMessage::ErrorMessage) {
		sp_log(Log::Warning) << "DBus: Cannot grab media keys" << reply.errorName() << reply.errorMessage();
	}
}
