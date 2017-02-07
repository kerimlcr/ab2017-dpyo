/* SomaFMLibraryContainer.cpp */

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



/* SomaFMLibraryContainer.cpp */

#include "ui/SomaFMLibraryContainer.h"
#include "ui/GUI_SomaFM.h"


SomaFMLibraryContainer::SomaFMLibraryContainer(QObject* parent) :
	LibraryContainerInterface(parent)
{
	Q_INIT_RESOURCE(SomaFMIcons);
}

/*
SomaFMLibraryContainer::~SomeFMLibraryContainer(){
	ui->deleteLater();
}*/

QString SomaFMLibraryContainer::get_name() const
{
	return "SomaFM";
}

QString SomaFMLibraryContainer::get_display_name() const
{
	return "SomaFM";
}

QIcon SomaFMLibraryContainer::get_icon() const
{
	return QIcon(":/soma_icons/soma.png");
}

QWidget* SomaFMLibraryContainer::get_ui() const
{
	return ui;
}

QComboBox* SomaFMLibraryContainer::get_libchooser()
{
	return ui->get_libchooser();
}

QMenu* SomaFMLibraryContainer::get_menu()
{
	return nullptr;
}

void SomaFMLibraryContainer::init_ui()
{
	ui = new GUI_SomaFM(nullptr);
}

