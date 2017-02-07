/* LibraryDatabase.cpp */

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



#include "LibraryDatabase.h"

LibraryDatabase::LibraryDatabase(quint8 db_id, const QString& db_dir, const QString& db_name) :
	AbstractDatabase(db_id, db_dir, db_name),
	DatabaseAlbums(_database, db_id),
	DatabaseArtists(_database, db_id),
	DatabaseTracks(_database, db_id),
	DatabaseLibrary(_database, db_id)
{
	DB::getInstance()->add(this);
}
