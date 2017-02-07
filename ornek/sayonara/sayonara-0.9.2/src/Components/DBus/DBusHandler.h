/* DBusHandler.h */

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


#include <QObject>
#include <QList>


#ifndef DBUSHANDLER_H
#define DBUSHANDLER_H

class DBusMediaKeysInterfaceMate;
class DBusMediaKeysInterfaceGnome;
class DBusNotifications;
class SayonaraMainWindow;

namespace DBusMPRIS {
	class MediaPlayer2;
}

class DBusHandler : private QObject
{

public:
	DBusHandler(SayonaraMainWindow* player, QObject* parent=nullptr);
	virtual ~DBusHandler();


private:
	DBusMPRIS::MediaPlayer2*		_dbus_mpris=nullptr;
	DBusMediaKeysInterfaceMate*		_dbus_mate=nullptr;
	DBusMediaKeysInterfaceGnome*	_dbus_gnome=nullptr;
	DBusNotifications*				_dbus_notifications=nullptr;
};

#endif // DBUSHANDLER_H
