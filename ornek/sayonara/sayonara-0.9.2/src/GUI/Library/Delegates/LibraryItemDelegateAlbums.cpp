/* LibraryItemDelegateAlbums.cpp */

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
 * LibraryItemDelegateAlbums.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: Lucio Carreras
 */

#include "LibraryItemDelegateAlbums.h"
#include "GUI/Library/Views/LibraryView.h"
#include "GUI/Library/Models/LibraryItemModelAlbums.h"
#include "GUI/Helper/RatingLabel/RatingLabel.h"
#include "GUI/Helper/GUI_Helper.h"

#include <QLabel>
#include <QItemDelegate>
#include <QPainter>
#include <QTableView>
#include <QStyleOptionViewItem>


LibraryItemDelegateAlbums::LibraryItemDelegateAlbums(LibraryView* parent, bool enabled) :
	LibraryRatingDelegate(parent, enabled)
{
	_icon_single_album = GUI::get_pixmap("play", QSize(16, 16), false);
	_icon_multi_album = GUI::get_pixmap("sampler", QSize(16, 16), false);
}


LibraryItemDelegateAlbums::~LibraryItemDelegateAlbums() {

}


void LibraryItemDelegateAlbums::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if(!index.isValid()) return;

	int col = index.column();

	if(col != COL_ALBUM_RATING) {
		LibraryRatingDelegate::paint(painter, option, index);		
		return;
	}

	else if(col == COL_ALBUM_RATING) {
		RatingLabel label((QWidget*)_parent, true);
		label.set_rating(index.data().toInt());
		label.setGeometry(option.rect);

		painter->save();
		painter->translate(option.rect.left(), option.rect.top());
		label.render(painter);
		painter->restore();

	}   



}
