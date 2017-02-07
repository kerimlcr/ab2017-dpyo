/* SettingRegistry.h */

/* Copyright (C) 2011-2015  Lucio Carreras
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



#include "Helper/Settings/Settings.h"
#include "Helper/Settings/SettingKey.h"
#include "Helper/globals.h"

#include <type_traits>

class SettingRegistry
{
	SINGLETON(SettingRegistry)

private:
	Settings* 	_settings=nullptr;

	template<typename KEY, typename T>
	void register_setting(const KEY& key, const char* db_key, const T& default_value){

		typedef decltype(key.p) ValueTypePtr;
		typedef typename std::remove_pointer<ValueTypePtr>::type ValueType;
		auto setting = new Setting<ValueType>(key, db_key, default_value);

		_settings->register_setting( setting );
	} 

	template<typename KEY, typename T>
	void register_setting(const KEY& key, const T& default_value){

		typedef decltype(key.p) ValueTypePtr;
		typedef typename std::remove_pointer<ValueTypePtr>::type ValueType;
		auto setting = new Setting<ValueType>(key, default_value);

		_settings->register_setting( setting );
	}

public:
	bool init();
};

