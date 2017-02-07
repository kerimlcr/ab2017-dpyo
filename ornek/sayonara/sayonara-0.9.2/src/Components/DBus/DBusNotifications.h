
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




#ifndef DBUSNOTIFICATIONS_H
#define DBUSNOTIFICATIONS_H

#include "Interfaces/Notification/NotificationHandler.h"
#include "Components/DBus/Notifications.h"
#include "Components/PlayManager/PlayManager.h"
#include "Helper/globals.h"

class DBusNotifications :
		public QObject,
		public NotificationInterface,
		private SayonaraClass
{
	Q_OBJECT

private:
	OrgFreedesktopNotificationsInterface* _interface=nullptr;

public:
	DBusNotifications(QObject* parent=0);
	virtual ~DBusNotifications();

	void notify(const MetaData& md);
	void notify(const QString& title, const QString& text, const QString& image_path);

private slots:
	void track_changed(const MetaData& md);

};


#endif // DBUSNOTIFICATIONS_H
