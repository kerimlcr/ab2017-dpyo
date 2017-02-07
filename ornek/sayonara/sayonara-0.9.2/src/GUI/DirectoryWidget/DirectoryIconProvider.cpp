/* IconProvider.cpp */

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



#include "DirectoryIconProvider.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Settings/Settings.h"

#include "GUI/Helper/GUI_Helper.h"

IconProvider::IconProvider() :
	QFileIconProvider()
{
	_settings = Settings::getInstance();

	_folder_icon.addPixmap(GUI::get_pixmap("folder"), QIcon::Normal, QIcon::Off);
	_folder_icon.addPixmap(GUI::get_pixmap("folder_open"), QIcon::Normal, QIcon::On);
}

IconProvider::~IconProvider() {}

QIcon IconProvider::icon(IconType type) const {

	return QFileIconProvider::icon(type);
}

QIcon IconProvider::icon(const QFileInfo &info) const {

	if(_settings->get(Set::Player_Style) == 0){
		return QFileIconProvider::icon(info);
	}

	if(info.isDir()){
		return _folder_icon;
	}

	if(info.isFile() && Helper::File::is_playlistfile(info.filePath())){
		return GUI::get_icon("playlistfile");
	}

	return QFileIconProvider::icon(info);
}
