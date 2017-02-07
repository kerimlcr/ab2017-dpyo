/* DBusMediaKeysInterfaceGnome.cpp */

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



#include "DBusMediaKeysInterfaceGnome.h"

DBusMediaKeysInterfaceGnome::DBusMediaKeysInterfaceGnome(QObject *parent) :
	DBusMediaKeysInterface(parent)
{

	_media_key_interface = new OrgGnomeSettingsDaemonMediaKeysInterface(
				"org.gnome.SettingsDaemon",
				"/org/gnome/SettingsDaemon/MediaKeys",
				QDBusConnection::sessionBus(),
				this);



	if (!QDBusConnection::sessionBus().interface()->isServiceRegistered("org.gnome.SettingsDaemon"))
	{
		return;
	}

	sp_log(Log::Info) << "DBus: org.gnome.SettingsDaemon registered";
	QDBusPendingReply<> reply = _media_key_interface->GrabMediaPlayerKeys("sayonara", 0);
	QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(reply, this);

	connect(watcher, &QDBusPendingCallWatcher::finished,
			this, &DBusMediaKeysInterfaceGnome::sl_register_finished);

	_initialized = true;
}



void DBusMediaKeysInterfaceGnome::sl_register_finished(QDBusPendingCallWatcher* watcher){

	if(!_initialized){
		return;
	}

	DBusMediaKeysInterface::sl_register_finished(watcher);

	connect( _media_key_interface,
			 &OrgGnomeSettingsDaemonMediaKeysInterface::MediaPlayerKeyPressed,
			 this,
			 &DBusMediaKeysInterfaceGnome::sl_media_key_pressed
	);

}
