/* LocalLibraryMenu.cpp */

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

#include "LocalLibraryMenu.h"
#include "GUI/Helper/GUI_Helper.h"
#include "GUI/Helper/IconLoader/IconLoader.h"

LocalLibraryMenu::LocalLibraryMenu(QWidget* parent) :
	QMenu(parent),
	SayonaraClass()
{
	_icon_loader = IconLoader::getInstance();

	_reload_library_action = new QAction(GUI::get_icon("undo"), tr("Reload library"), this);
	_import_file_action = new QAction(GUI::get_icon("open"), tr("Import files"), this);
	_import_folder_action = new QAction(GUI::get_icon("open"), tr("Import directory"), this);
	_info_action = new QAction(GUI::get_icon("info"), tr("Info"), this);
	_library_path_action = new QAction(GUI::get_icon("folder"), tr("Library path"), this);


	_realtime_search_action = new QAction(QIcon(), tr("Live search"), this);
	_realtime_search_action->setCheckable(true);
	_realtime_search_action->setChecked(_settings->get(Set::Lib_LiveSearch));
	_auto_update = new QAction(QIcon(), tr("Auto update"), this);
	_auto_update->setCheckable(true);
	_auto_update->setChecked(_settings->get(Set::Lib_AutoUpdate));

	connect(_reload_library_action, &QAction::triggered, this, &LocalLibraryMenu::sig_reload_library);
	connect(_import_file_action, &QAction::triggered, this, &LocalLibraryMenu::sig_import_file);
	connect(_import_folder_action, &QAction::triggered, this, &LocalLibraryMenu::sig_import_folder);
	connect(_info_action, &QAction::triggered, this, &LocalLibraryMenu::sig_info);
	connect(_library_path_action, &QAction::triggered, this, &LocalLibraryMenu::sig_libpath_clicked);
	connect(_realtime_search_action, &QAction::triggered, this, &LocalLibraryMenu::realtime_search_changed);
	connect(_auto_update, &QAction::triggered, this, &LocalLibraryMenu::auto_update_changed);

	_actions <<_library_path_action <<
				this->addSeparator() <<

				_info_action <<
				this->addSeparator() <<
				_import_file_action <<
				_import_folder_action <<
				_reload_library_action <<
				this->addSeparator() <<
				_realtime_search_action <<
				_auto_update;

	this->addActions(_actions);

	REGISTER_LISTENER(Set::Player_Language, language_changed);
	REGISTER_LISTENER(Set::Player_Style, skin_changed);
}


LocalLibraryMenu::~LocalLibraryMenu()
{

}

void LocalLibraryMenu::language_changed()
{
	_reload_library_action->setText(tr("Reload library"));
	_import_file_action->setText(tr("Import files"));
	_import_folder_action->setText(tr("Import directory"));
	_info_action->setText(tr("Info"));
	_library_path_action->setText(tr("Library path"));
	_realtime_search_action->setText(tr("Live search"));
	_auto_update->setText(tr("Auto update"));
}

void LocalLibraryMenu::skin_changed()
{
	_reload_library_action->setIcon(_icon_loader->get_icon("view-refresh", "undo"));
	_import_file_action->setIcon(_icon_loader->get_icon("document-open", "open"));
	_import_folder_action->setIcon(_icon_loader->get_icon("document-open", "open"));
	_info_action->setIcon(_icon_loader->get_icon("dialog-information", "info"));
	_library_path_action->setIcon(_icon_loader->get_icon("folder", "folder"));
}


void LocalLibraryMenu::realtime_search_changed()
{
	_settings->set(Set::Lib_LiveSearch, _realtime_search_action->isChecked());
}

void LocalLibraryMenu::auto_update_changed()
{
	_settings->set(Set::Lib_AutoUpdate, _auto_update->isChecked());
}

