/* AlternativeCoverItemModel.cpp */

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
 * AlternativeCoverItemModel.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: Lucio Carreras
 */

#include "AlternativeCoverItemModel.h"
#include "Components/Covers/CoverLocation.h"
#include "Helper/Logger/Logger.h"
#include "Helper/globals.h"

#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QPixmap>
#include <QIcon>

AlternativeCoverItemModel::AlternativeCoverItemModel(QObject* parent) : QAbstractTableModel(parent) {

	_pathlist.reserve(10);
}

AlternativeCoverItemModel::~AlternativeCoverItemModel() {

}

RowColumn AlternativeCoverItemModel::cvt_2_row_col(int idx) const {


	RowColumn p;

    if(idx < 0) {
        p.row = -1;
        p.col = -1;
        p.valid = false;
    }

	p.row = idx / columnCount();
	p.col = idx % columnCount();
    p.valid = true;

	return p;
}

int AlternativeCoverItemModel::cvt_2_idx(int row, int col) const {
    if(row < 0 || col < 0) return -1;

	return row * columnCount() + col;
}


int AlternativeCoverItemModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 2;

}
int AlternativeCoverItemModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 5;
}

QVariant AlternativeCoverItemModel::data(const QModelIndex &index, int role) const
{
	const int sz = 80;

	int lin_idx = this->cvt_2_idx(index.row(), index.column());
    if(lin_idx < 0) return QVariant();

	 if ( !index.isValid() || !between(lin_idx, _pathlist) ) {
         return QVariant();
     }

	 else if(role == Qt::UserRole){
		 return _pathlist[lin_idx];
	 }

	 else if(role == Qt::SizeHintRole){
		 return QSize(sz, sz);
	 }

	 return QVariant();
}


Qt::ItemFlags AlternativeCoverItemModel::flags(const QModelIndex &index) const{
	if (!index.isValid()){
		return Qt::ItemIsEnabled;
	}

	if(!between(index.row(), _pathlist)){
		return QAbstractItemModel::flags(index);
	}

	bool invalid = CoverLocation::isInvalidLocation(_pathlist[index.row()]);
	if(invalid){
		return (Qt::NoItemFlags);
	}

	return QAbstractItemModel::flags(index);

}

bool AlternativeCoverItemModel::setData(const QModelIndex &index, const QVariant &value, int role) {

	if (!index.isValid()){
		 return false;
	}

	int lin_idx = cvt_2_idx(index.row(), index.column());

    if(lin_idx >= _pathlist.size() || lin_idx < 0)
        return false;

	 if(role == Qt::DisplayRole) {

		_pathlist[lin_idx] = value.toString();

		 emit dataChanged(index, index);
		 return true;
	 }

	 return false;
}

bool AlternativeCoverItemModel::insertRows(int position, int rows, const QModelIndex &index) {
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position+rows-1);

	_pathlist.clear();
	QString invalid_path = CoverLocation::getInvalidLocation().cover_path();

	for(int i=0; i<rows; i++) {
		for(int j=0; j<columnCount(); j++) {
			_pathlist << invalid_path;
		}
	}

	endInsertRows();
	return true;

}
bool AlternativeCoverItemModel::removeRows(int position, int rows, const QModelIndex &index) {
	Q_UNUSED(index);

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 _pathlist.clear();

	 endRemoveRows();
	 return true;

}

bool AlternativeCoverItemModel::is_valid(int row, int col){

    int idx = cvt_2_idx(row, col);
    if(idx < 0) return false;

	return ( !CoverLocation::isInvalidLocation(_pathlist[ idx ]) );
}
