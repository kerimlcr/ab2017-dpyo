/* Setting.cpp */

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

#include "Setting.h"


AbstrSetting::AbstrSetting(){

}

AbstrSetting::AbstrSetting(const AbstrSetting& other){
	_key = other._key;
	_db_key = other._db_key;
}

AbstrSetting::AbstrSetting(SK::SettingKey key, const char* db_key){
	_key = key;
	_db_key = db_key;
}

AbstrSetting::~AbstrSetting(){

}

QString AbstrSetting::get_db_key() const
{
	return _db_key;
}

SK::SettingKey AbstrSetting::get_key() const
{
	return _key;
}


