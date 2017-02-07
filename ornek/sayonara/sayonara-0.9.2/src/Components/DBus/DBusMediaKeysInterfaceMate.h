/* DBusMediaKeysInterfaceMate.h */

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



#ifndef DBUSMEDIAKEYSINTERFACEMATE_H
#define DBUSMEDIAKEYSINTERFACEMATE_H

#include "DBusMediaKeysInterface.h"
#include "Components/DBus/mate_settings_daemon.h"
#include <QDBusPendingCallWatcher>

class DBusMediaKeysInterfaceMate : public DBusMediaKeysInterface
{
	Q_OBJECT

public:
	explicit DBusMediaKeysInterfaceMate(QObject *parent=nullptr);


private:


	OrgMateSettingsDaemonMediaKeysInterface* _media_key_interface=nullptr;


protected slots:
	void sl_register_finished(QDBusPendingCallWatcher*) override;
};

#endif // DBUSMEDIAKEYSINTERFACEMATE_H
