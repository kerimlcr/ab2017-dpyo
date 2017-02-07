/* DatabaseBookmarks.h */

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



#ifndef DATABASEBOOKMARKS_H
#define DATABASEBOOKMARKS_H

#include "Database/DatabaseModule.h"
#include <QMap>

class DatabaseBookmarks : private DatabaseModule
{
public:
	DatabaseBookmarks(QSqlDatabase db, quint8 db_id);

	bool searchBookmarks(int track_id, QMap<quint32, QString>& bookmarks);
	bool insertBookmark(int track_id, quint32 time, QString text);
	bool removeBookmark(int track_id, quint32 time);
	bool removeAllBookmarks(int track_id);

};

#endif // DATABASEBOOKMARKS_H
