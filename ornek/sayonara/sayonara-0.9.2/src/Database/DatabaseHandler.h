/* DatabaseHandler.h */

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



#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include "Helper/globals.h"
#include "Database/LibraryDatabase.h"

class Album;
class MetaData;
class Artist;

class DB {
	SINGLETON(DB)

	public:
		static LibraryDatabase* getInstance(quint8 db_id);
		static LibraryDatabase* getInstance(const Album& album);
		static LibraryDatabase* getInstance(const MetaData& md);
		static LibraryDatabase* getInstance(const Artist& artist);

		LibraryDatabase* get(quint8 db_id);
		LibraryDatabase* get(const Album& album);
		LibraryDatabase* get(const MetaData& md);
		LibraryDatabase* get(const Artist& artist);
		LibraryDatabase* get_std();

		void add(LibraryDatabase* db);

	private:
		QMap<quint8, LibraryDatabase*> _dbs;

};

#endif // DATABASEHANDLER_H
