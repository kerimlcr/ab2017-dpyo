/* Settings.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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


#ifndef Settings_H
#define Settings_H

#include "Helper/Settings/Setting.h"

/**
 * @brief The Settings class
 * @ingroup Settings
 */
class Settings : public QObject
{

	SINGLETON(Settings)

private:

	QString _db_file;
	QString _version;

	AbstrSetting* _settings[SK::Num_Setting_Keys + 1];


public:

	/* get all settings (used by database) */
	AbstrSetting**	get_settings();


	/* before you want to access a setting you have to register it */
	void register_setting(AbstrSetting* s);


	/* checks if all settings are registered */
	bool check_settings();


	/* get a setting, defined by a unique, REGISTERED key */
	template<typename T, SK::SettingKey S>
	const T& get(const SettingKey<T,S> k){
		Q_UNUSED(k);
		Setting<T>* s = (Setting<T>*) _settings[(int) S];
		return s->getValue();
	}


	/* set a setting, define by a unique, REGISTERED key */
	template<typename T, SK::SettingKey S>
	void set(SettingKey<T,S> key, const T& val){
		Q_UNUSED(key)
		Setting<T>* s = (Setting<T>*) _settings[(int) S];

		if( s->setValue(val)) {
			SettingNotifier< SettingKey<T, S> >* sn = SettingNotifier< SettingKey<T, S> >::getInstance();
			sn->val_changed();
		}
	}

	/* get a setting, defined by a unique, REGISTERED key */
	template<typename T, SK::SettingKey S>
	void shout(const SettingKey<T,S>& k){
		Q_UNUSED(k);
		SettingNotifier< SettingKey<T, S> >* sn = SettingNotifier< SettingKey<T, S> >::getInstance();
		sn->val_changed();
	}
};


#endif // Settings_H
