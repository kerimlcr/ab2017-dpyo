/* GUI_LibraryPath.cpp */

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



#include "GUI_LibraryPath.h"

#include <QIcon>


GUI_LibraryPath::GUI_LibraryPath(QWidget *parent) :
	QWidget(parent),
	Ui::GUI_SetLibrary()
{
	setupUi(this);

	connect(btn_setLibrary, &QPushButton::clicked, this, &GUI_LibraryPath::btn_clicked);
}

void GUI_LibraryPath::language_changed() {
	retranslateUi(this);
}


void GUI_LibraryPath::btn_clicked() {

    emit sig_library_path_set();
}

void GUI_LibraryPath::resizeEvent(QResizeEvent* e) {

    e->accept();
}


