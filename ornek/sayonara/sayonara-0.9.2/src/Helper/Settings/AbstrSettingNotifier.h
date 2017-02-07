/* AbstrSettingNotifier.h */

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



#ifndef ABSTRSETTINGNOTIFIER_H
#define ABSTRSETTINGNOTIFIER_H

#include <QObject>

/**
 * @brief The AbstrSettingNotifier class
 * The setting notifier emits a sig_value_changed whenever the value of\n
 * the underlying setting (defined by the SettingKey) has changed.\n
 * After the signal has been received the listener still can decide if\n
 * it's reading the new value or not\n\n
 * we need an abstract instance of the notifier because Qt \n
 * Qt does not allow that a template class (like SettingNotifier) \n
 * can be a QObject and therefore signals
 * @ingroup Settings
 */
class AbstrSettingNotifier : public QObject{

	Q_OBJECT

	signals:
		void sig_value_changed();

	protected:
		AbstrSettingNotifier(QObject* parent=nullptr);
		virtual ~AbstrSettingNotifier();
};



#endif // ABSTRSETTINGNOTIFIER_H
