/* LibraryItemDelegateArtists.cpp */

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


/*
 * LibraryItemDelegateArtists.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: Lucio Carreras
 */

#include "LibraryItemDelegateArtists.h"
#include "GUI/Helper/GUI_Helper.h"

#include <QLabel>
#include <QTableView>
#include <QItemDelegate>
#include <QPainter>


LibraryItemDelegateArtists::LibraryItemDelegateArtists(QTableView* parent) :
	QStyledItemDelegate(parent)
{

	_parent = parent;

	_icon_single_album = GUI::get_pixmap("play", QSize(16, 16), false);
	_icon_multi_album = GUI::get_pixmap("sampler", QSize(16, 16), false);

}

LibraryItemDelegateArtists::~LibraryItemDelegateArtists() {

}



