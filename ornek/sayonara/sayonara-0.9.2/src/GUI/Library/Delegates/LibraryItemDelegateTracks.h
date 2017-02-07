/* LibraryItemDelegateTracks.h */

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
 * ibraryItemDelegateTracks.h
 *
 *  Created on: Oct 28, 2011
 *      Author: Lucio Carreras
 */

#ifndef IBRARYITEMDELEGATETRACKS_H_
#define IBRARYITEMDELEGATETRACKS_H_

#include "GUI/Library/Delegates/LibraryRatingDelegate.h"


class LibraryItemDelegateTracks : public LibraryRatingDelegate {

public:
	LibraryItemDelegateTracks(LibraryView* parent, bool enabled);
	virtual ~LibraryItemDelegateTracks();

public:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif /* IBRARYITEMDELEGATETRACKS_H_ */
