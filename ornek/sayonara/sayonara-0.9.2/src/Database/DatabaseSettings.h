/* DatabaseSettings.h */

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



#ifndef DATABASESETTINGS_H
#define DATABASESETTINGS_H

#include "Database/DatabaseModule.h"
#include <QVariant>

class DatabaseSettings : private DatabaseModule
{

public:
	DatabaseSettings(QSqlDatabase db, quint8 db_id);


	bool load_setting(QString key, QString& val);
	bool store_setting(QString key, const QVariant& val);

	bool load_settings();
	bool store_settings();

};

#endif // DATABASESETTINGS_H
