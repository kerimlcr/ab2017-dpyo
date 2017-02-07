/* DatabaseStreams.h */

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



#ifndef DATABASESTREAMS_H
#define DATABASESTREAMS_H

#include "Database/DatabaseModule.h"
#include <QMap>

class DatabaseStreams : private DatabaseModule
{
public:
	DatabaseStreams(QSqlDatabase db, quint8 db_id);

	bool getAllStreams(QMap<QString, QString>& result);
	bool deleteStream(const QString& name);
	bool addStream(const QString& name, const QString& url);
	bool renameStream(const QString& name, const QString& url);
	bool updateStreamUrl(const QString& name, const QString& url);

};

#endif // DATABASESTREAMS_H
