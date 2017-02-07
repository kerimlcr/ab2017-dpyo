/* LibraryItemModelTracks.h */

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
 * LibraryItemModelTracks.h
 *
 *  Created on: Apr 24, 2011
 *      Author: Lucio Carreras
 */

#ifndef LIBRARYITEMMODELTRACKS_H_
#define LIBRARYITEMMODELTRACKS_H_

#include "Helper/MetaData/MetaData.h"
#include "GUI/Library/Models/LibraryItemModel.h"

class LibraryItemModelTracks : public LibraryItemModel {

Q_OBJECT

public:
	LibraryItemModelTracks();

	virtual ~LibraryItemModelTracks();

	QVariant data(const QModelIndex &index, int role) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	bool setData(const QModelIndex &index, const QVariant &value, int role) override;
	bool setData(const QModelIndex &index, const MetaDataList &v_md, int role);

	virtual int get_id_by_row(int row) override;
	virtual QString get_string(int row) const override;

	virtual QModelIndex	getFirstRowIndexOf(QString substr) override;
	virtual QModelIndex	getPrevRowIndexOf(QString substr, int row, const QModelIndex& parent=QModelIndex()) override;
	virtual QModelIndex	getNextRowIndexOf(QString substr, int row, const QModelIndex& parent=QModelIndex()) override;



private:
	MetaDataList _tracks;

};

#endif /* LIBRARYITEMMODELTRACKS_H_ */
