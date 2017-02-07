/* LocalLibraryContainer.cpp */

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



#include "LocalLibraryContainer.h"
#include "GUI/Library/GUI_LocalLibrary.h"
#include "GUI/Helper/GUI_Helper.h"

LocalLibraryContainer::LocalLibraryContainer(QObject* parent) :
	LibraryContainerInterface(parent)
{

}

QString LocalLibraryContainer::get_name() const
{
	return "local_library";
}

QString LocalLibraryContainer::get_display_name() const {
	return tr("Local Library");
}

QIcon LocalLibraryContainer::get_icon() const {
	return GUI::get_icon("append");
}

QWidget* LocalLibraryContainer::get_ui() const {
	return static_cast<QWidget*>(_ui);
}

QComboBox* LocalLibraryContainer::get_libchooser(){
	return _ui->get_libchooser();
}

QMenu*LocalLibraryContainer::get_menu()
{
	if(_ui){
		return _ui->get_menu();
	}

	return nullptr;
}

void LocalLibraryContainer::init_ui()
{
	_ui = new GUI_LocalLibrary();
}
