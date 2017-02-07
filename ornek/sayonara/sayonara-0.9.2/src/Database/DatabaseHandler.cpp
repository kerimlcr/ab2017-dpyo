/* DatabaseHandler.cpp */

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



#include "DatabaseHandler.h"
#include "DatabaseConnector.h"

#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/MetaData/Artist.h"


DB::DB()
{
}

DB::~DB(){

}

LibraryDatabase* DB::getInstance(quint8 db_id){
	return getInstance()->get(db_id);
}

LibraryDatabase* DB::getInstance(const Album& album){
	return getInstance()->get(album);
}

LibraryDatabase* DB::getInstance(const MetaData& md){
	return getInstance()->get(md);
}

LibraryDatabase* DB::getInstance(const Artist& artist){
	return getInstance()->get(artist);
}


void DB::add(LibraryDatabase *db){
	_dbs.insert(db->get_id(), db);
}

LibraryDatabase* DB::get(quint8 db_id){

	if(_dbs.size() == 0){
		sp_log(Log::Warning) << "There are no Databases available";
		return get_std();
	}

	if(!_dbs.contains(db_id)){
		sp_log(Log::Warning) << "Database " << (int) db_id << " is not available";
		return get_std();
	}

	return _dbs[db_id];
}

LibraryDatabase* DB::get(const Album& album){
	if(album.id < 0){
		return get(0);
	}

	return get(album.db_id);
}

LibraryDatabase* DB::get(const MetaData& md){
	if(md.id < 0){
		return get(0);
	}

	return get(md.db_id);
}

LibraryDatabase* DB::get(const Artist& artist){
	if(artist.id < 0){
		return get(0);
	}
	return get(artist.db_id);
}

LibraryDatabase* DB::get_std(){
	return DatabaseConnector::getInstance();
}
