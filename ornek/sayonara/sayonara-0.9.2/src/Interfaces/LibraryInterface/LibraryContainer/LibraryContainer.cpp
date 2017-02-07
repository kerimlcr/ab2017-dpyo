/* LibraryContainer.cpp */

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



#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"
#include "Helper/Settings/Settings.h"


LibraryContainerInterface::LibraryContainerInterface(QObject* parent) :
	QObject(parent)
{
	_initialized = false;
	_settings = Settings::getInstance();

	REGISTER_LISTENER(Set::Player_Language, language_changed);
}

LibraryContainerInterface::~LibraryContainerInterface()
{

}

QMenu* LibraryContainerInterface::get_menu()
{
	return nullptr;
}

void LibraryContainerInterface::set_menu_action(QAction* action)
{
	_action = action;
}

QAction* LibraryContainerInterface::get_menu_action() const
{
	return _action;
}


void LibraryContainerInterface::set_initialized(){
	_initialized = true;
}

bool LibraryContainerInterface::is_initialized() const
{
	return _initialized;
}


void LibraryContainerInterface::language_changed(){
	if(_action){
		_action->setText(this->get_display_name());
	}
}


