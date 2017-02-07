
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




#ifndef NOTIFICATIONHANDLER_H
#define NOTIFICATIONHANDLER_H

#include "NotificationInterface.h"
#include "Helper/globals.h"


class DummyNotificator : public NotificationInterface
{
public:
	DummyNotificator(const QString& name);
	virtual void notify(const MetaData& md);
	virtual void notify(const QString& title, const QString& message, const QString& image_path);
};

class NotificationHandler :
		public QObject
{

	Q_OBJECT
	SINGLETON_QOBJECT(NotificationHandler)

signals:
	// emitted when some AbstractNotifcator registered itself
	void sig_notifications_changed();

private:
	NotificatonList _notificators;
	int _cur_idx;

	NotificationInterface* get() const;

public:
	void register_notificator(NotificationInterface* notificator);
	void notificator_changed(const QString& name);

	int get_cur_idx() const;

	NotificatonList get_notificators() const;

	virtual void notify(const MetaData& md);
	virtual void notify(const QString& title, const QString& message, const QString& image_path=QString());

};



#endif // NOTIFICATIONHANDLER_H
