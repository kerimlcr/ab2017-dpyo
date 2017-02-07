/* SoundcloudLibraryContainer.cpp */

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



/* SoundcloudLibraryContainer.cpp */

#include "SoundcloudLibraryContainer.h"
#include "ui/GUI_SoundcloudLibrary.h"
#include "src/SoundcloudLibrary.h"


SoundcloudLibraryContainer::SoundcloudLibraryContainer(QObject *parent) :
	LibraryContainerInterface(parent)
{
	Q_INIT_RESOURCE(SoundcloudIcons);
}

QString SoundcloudLibraryContainer::get_name() const
{
	return "soundcloud";
}

QString SoundcloudLibraryContainer::get_display_name() const
{
	return tr("Soundcloud");
}

QIcon SoundcloudLibraryContainer::get_icon() const
{
	return QIcon(":/sc_icons/ui/icon.png");
}


QWidget* SoundcloudLibraryContainer::get_ui() const
{
	return static_cast<QWidget*>(ui);
}

QComboBox* SoundcloudLibraryContainer::get_libchooser()
{
	if(ui){
		return ui->get_libchooser();
	}

	return nullptr;
}

QMenu* SoundcloudLibraryContainer::get_menu()
{
	if(ui){
		return ui->get_menu();
	}

	return nullptr;
}

void SoundcloudLibraryContainer::init_ui()
{
	SoundcloudLibrary* library = new SoundcloudLibrary(this);
	ui = new GUI_SoundCloudLibrary(library);
}

