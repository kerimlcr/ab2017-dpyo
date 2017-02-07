
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




#include "DBusNotifications.h"
#include "Components/Covers/CoverLocation.h"

DBusNotifications::DBusNotifications(QObject* parent) :
	QObject(parent),
	NotificationInterface("DBus"),
	SayonaraClass()
{

	_interface = new OrgFreedesktopNotificationsInterface(
				QString("org.freedesktop.Notifications"),
				QString("/org/freedesktop/Notifications"),
				QDBusConnection::sessionBus(),
				parent
	);

	if (!QDBusConnection::sessionBus().interface()->isServiceRegistered("org.freedesktop.Notifications"))
	{
		sp_log(Log::Debug) << "DBus: Notification service is not registered";

	}

	else{
		sp_log(Log::Info) << "DBus: org.freedesktop.Notifications registered";
	}

	NotificationHandler::getInstance()->register_notificator(this);
}


DBusNotifications::~DBusNotifications(){

}



void DBusNotifications::notify(const QString& title, const QString& text, const QString& image_path){

	QVariantMap map;
	map.insert("action-icons", false);
	map.insert("desktop-entry", "/usr/share/applications/sayonara.desktop");
	map.insert("resident", false);
	map.insert("sound-file", QString());
	map.insert("sound-name", QString());
	map.insert("suppress-sound", true);
	map.insert("transient", false);
	map.insert("urgency", 1);

	_interface->Notify("Sayonara Player",
					   500,
					   image_path,
					   title,
					   text,
					   QStringList(),
					   map,
					   _settings->get(Set::Notification_Timeout)
	);
}

void DBusNotifications::notify(const MetaData& md){
	this->track_changed(md);
}


void DBusNotifications::track_changed(const MetaData& md){

	bool active = _settings->get(Set::Notification_Show);
	if(!active){
		return;
	}

	CoverLocation cl = CoverLocation::get_cover_location(md);
	QString cover_path = cl.cover_path();
	if(!QFile::exists(cover_path)){
		if(cl.local_paths().isEmpty()){
			cover_path = CoverLocation::getInvalidLocation().cover_path();
		}
		else{
			cover_path = cl.local_path(0);
		}
	}

	notify(md.title, "by " + md.artist, cover_path);
}
