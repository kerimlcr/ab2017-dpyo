/* LibraryItemModelArtists.h */

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
 * LibraryItemModelArtistts.h
 *
 *  Created on: Apr 26, 2011
 *      Author: Lucio Carreras
 */

#ifndef LIBRARYITEMMODELARTISTS_H_
#define LIBRARYITEMMODELARTISTS_H_

#include "LibraryItemModel.h"
#include "Helper/MetaData/Artist.h"

#include <QPixmap>

class ColumnHeader;
class LibraryItemModelArtists : public LibraryItemModel {

Q_OBJECT
public:
	LibraryItemModelArtists();
	virtual ~LibraryItemModelArtists();

	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::DisplayRole) override;
	bool setData(const QModelIndex &index, const ArtistList& artists, int role=Qt::DisplayRole);

	virtual int get_id_by_row(int row) override;
	virtual QString get_string(int row) const override;

	virtual QModelIndex	getFirstRowIndexOf(QString substr) override;
	virtual QModelIndex	getPrevRowIndexOf(QString substr, int row, const QModelIndex& parent=QModelIndex()) override;
	virtual QModelIndex	getNextRowIndexOf(QString substr, int row, const QModelIndex& parent=QModelIndex()) override;



private:
	ArtistList	_artists;
	QPixmap		_pm_multi;
	QPixmap		_pm_single;
};


#endif /* LIBRARYITEMMODELARTISTS_H_ */
