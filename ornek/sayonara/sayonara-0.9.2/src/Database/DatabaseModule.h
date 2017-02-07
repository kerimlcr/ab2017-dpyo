/* DatabaseModule.h */

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



#ifndef DATABASEMODULE_H
#define DATABASEMODULE_H


#include <QSqlDatabase>
/*#include "Database/SayonaraQuery.h"
#include "Components/Library/Sorting.h"
#include "Helper/Filter.h"*/


#define DB_TRY_OPEN(db)		if (!this -> db.isOpen()) \
	this -> db.open()

#define DB_RETURN_NOT_OPEN_VOID(db) DB_TRY_OPEN(db); \
	if (!this -> db.isOpen()) \
	return

#define DB_RETURN_NOT_OPEN_INT(db) DB_TRY_OPEN(db); \
	if (!this -> db.isOpen()) \
	return -1

#define DB_RETURN_NOT_OPEN_BOOL(db) DB_TRY_OPEN(db); \
	if (!this -> db.isOpen()) \
	return false

#define DB_RETURN_NOT_OPEN_STRING(db) DB_TRY_OPEN(db); \
	if(!this->db.isOpen()) \
	return ""

class SayonaraQuery;
struct Filter;
class DatabaseModule
{

protected:

	QSqlDatabase _db;
	quint8 _module_db_id;


public:
	DatabaseModule(QSqlDatabase db, quint8 db_id);

};

#endif // DATABASEMODULE_H
