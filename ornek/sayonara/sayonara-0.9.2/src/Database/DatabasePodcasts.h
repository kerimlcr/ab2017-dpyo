/* DatabasePodcasts.h */

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



#ifndef DATABASEPODCASTS_H
#define DATABASEPODCASTS_H

#include "Database/DatabaseModule.h"
#include <QMap>

class DatabasePodcasts : private DatabaseModule
{
public:
	DatabasePodcasts(QSqlDatabase db, quint8 db_id);

	bool getAllPodcasts(QMap<QString, QString>& result);
	bool deletePodcast(const QString& name);
	bool addPodcast(const QString& name, const QString& url);
	bool renamePodcast(const QString& name, const QString& url);
	bool updatePodcastUrl(const QString& name, const QString& url);
};

#endif // DATABASEPODCASTS_H
