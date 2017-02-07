/* SettingNotifier.h */

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



#ifndef SETTINGNOTIFIER_H
#define SETTINGNOTIFIER_H

#include <QObject>
#include "Helper/Settings/AbstrSettingNotifier.h"

/*	Connect a Setting to a private slot in a class that want to be notified
	whenever the setting changed:

	call: REGISTER_LISTENER(Set::LFM_Active, lfm_active_changed);
	where lfm_active_changed() is a private Slot in that class;
*/

#define __REGISTER_LISTENER(setting_key, fn) \
	SettingNotifier<setting_key##_t>* v_##fn = SettingNotifier<setting_key##_t>::getInstance();\
	connect(v_##fn, SIGNAL(sig_value_changed()), this, SLOT( fn() ))

#define REGISTER_LISTENER(setting_key, fn) \
	do { \
		__REGISTER_LISTENER(setting_key, fn); \
		fn(); \
	} while(0)

#define REGISTER_LISTENER_NO_CALL(setting_key, fn) \
	do { \
		__REGISTER_LISTENER(setting_key, fn); \
	} while(0)




/* A Setting notifier has to be a singleton */
template < typename T >
class SettingNotifier : public AbstrSettingNotifier {

	private:
		SettingNotifier( QObject* parent=0 ) : AbstrSettingNotifier(parent) {}
		SettingNotifier( const SettingNotifier& ){}

	public:
		virtual ~SettingNotifier(){}

		static SettingNotifier< T >* getInstance(){
			static SettingNotifier< T > inst;
			return &inst;
		}

		void val_changed(){
			emit sig_value_changed();
		}
};


#endif // SETTINGNOTIFIER_H
