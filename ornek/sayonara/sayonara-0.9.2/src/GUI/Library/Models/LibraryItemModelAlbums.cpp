/* LibraryItemModelAlbums.cpp */

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
 * LibraryItemModelAlbums.cpp
 *
 *  Created on: Apr 26, 2011
 *      Author: Lucio Carreras
 */

#include "LibraryItemModel.h"
#include "LibraryItemModelAlbums.h"
#include "GUI/Library/Helper/ColumnHeader.h"
#include "GUI/Helper/GUI_Helper.h"
#include "Helper/Helper.h"
#include "Helper/Settings/Settings.h"
#include "Helper/LibrarySearchMode.h"

LibraryItemModelAlbums::LibraryItemModelAlbums() :
	LibraryItemModel()
{
	_pm_single = GUI::get_pixmap("cd.png", QSize(14, 14));
	_pm_multi = GUI::get_pixmap("cds.png", QSize(16, 16));
}

LibraryItemModelAlbums::~LibraryItemModelAlbums() {

}

int LibraryItemModelAlbums::get_id_by_row(int row)
{
	if(row < 0 || row >= _albums.size()){
		return -1;
	}

	else {
		return _albums[row].id;
	}
}

QString LibraryItemModelAlbums::get_string(int row) const
{
	if(row < 0 || row >= _albums.size()){
		return QString();
	}

	else {
		return _albums[row].name;
	}
}



QVariant LibraryItemModelAlbums::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= _albums.size())
		return QVariant();

	int row = index.row();
	int col = index.column();

	const Album& album = _albums[row];

	if(role == Qt::TextAlignmentRole ){
		int alignment = Qt::AlignVCenter;
		switch(col)
		{
			case COL_ALBUM_NAME:
				alignment |= Qt::AlignLeft;
				break;
			default:
				alignment |= Qt::AlignRight;

		}

		return alignment;
	}

	else if(role == Qt::TextColorRole){
		if(col == COL_ALBUM_MULTI_DISC){
			return QColor(0, 0, 0);
		}
	}

	else if(role == Qt::DecorationRole){
		if(col == COL_ALBUM_MULTI_DISC){
			if(album.discnumbers.size() > 1){
				return _pm_multi;
			}
			return _pm_single;
		}
	}

	else if(role == Qt::DisplayRole || role==Qt::EditRole) {

		switch(col) {

			case COL_ALBUM_N_SONGS:
				return QString::number(album.num_songs) + " " + tr("tracks");
			case COL_ALBUM_YEAR:
				if(album.year == 0){
					return tr("None");
				}
				return album.year;
			case COL_ALBUM_NAME:
				return album.name;
			case COL_ALBUM_DURATION:
				return Helper::cvt_ms_to_string(album.length_sec * 1000, true, false);
			case COL_ALBUM_RATING:
				return album.rating;

			default: return QVariant();
		}
	}

	return QVariant();
}

bool LibraryItemModelAlbums::setData(const QModelIndex & index, const QVariant & value, int role)
{

	if(!index.isValid()){
		return false;
	}

	if (role == Qt::EditRole || role == Qt::DisplayRole) {

		int row = index.row();
		int col = index.column();

		if(col == COL_ALBUM_RATING) {
			_albums[row].rating = value.toInt();
		}

		else {
			bool success = Album::fromVariant(value, _albums[row]);

			if( !success ) {
				return false;
			}
		}

		emit dataChanged(index, this->index(row, _header_names.size() - 1));

		return true;
	}

	return false;
}

bool LibraryItemModelAlbums::setData(const QModelIndex& index, const AlbumList& albums, int role)
{

	if(!index.isValid()){
		return false;
	}

	if (role == Qt::EditRole || role == Qt::DisplayRole) {

		int row = index.row();

		_albums = albums;

		emit dataChanged(index, this->index(row + albums.size() - 1, _header_names.size() - 1));

		return true;
	}

	return false;
}


Qt::ItemFlags LibraryItemModelAlbums::flags(const QModelIndex & index) const
{
	if (!index.isValid()) {
		return Qt::ItemIsEnabled;
	}

	int col = index.column();
	if(col == COL_ALBUM_RATING){
		return (QAbstractItemModel::flags(index) | Qt::ItemIsEditable);
	}

	return QAbstractItemModel::flags(index);
}


void LibraryItemModelAlbums::sort(int column, Qt::SortOrder order) {

	Q_UNUSED(column);
	Q_UNUSED(order);
}

QModelIndex LibraryItemModelAlbums::getFirstRowIndexOf(QString substr) {

	if(_albums.isEmpty()) {
		return this->index(-1, -1);
	}

	else{
		return getNextRowIndexOf(substr, 0);
	}
}

QModelIndex LibraryItemModelAlbums::getNextRowIndexOf(QString substr, int row, const QModelIndex& parent) {

	Q_UNUSED(parent)

	int len = _albums.size();
	if(len == 0)  {
		return this->index(-1, -1);
	}

	Settings* settings = Settings::getInstance();
	LibraryHelper::SearchModeMask mask = settings->get(Set::Lib_SearchMode);
	substr = LibraryHelper::convert_search_string(substr, mask);

	for(int i=0; i<len; i++) {
		int row_idx = (i + row) % len;

		QString album_name = _albums[row_idx].name;
		album_name = LibraryHelper::convert_search_string(album_name, mask);

		if( album_name.contains(substr))
		{
			return this->index(row_idx, 0);
		}
	}

	return this->index(-1, -1);
}

QModelIndex LibraryItemModelAlbums::getPrevRowIndexOf(QString substr, int row, const QModelIndex& parent) {

	Q_UNUSED(parent)

	Settings* settings = Settings::getInstance();
	LibraryHelper::SearchModeMask mask = settings->get(Set::Lib_SearchMode);
	substr = LibraryHelper::convert_search_string(substr, mask);

	int len = _albums.size();
	if(len < row){
		row = len - 1;
	}

	for(int i=0; i<len; i++) {
		int row_idx;

		if(row - i < 0){
			row = len - 1;
		}

		row_idx = (row-i) % len;

		QString album_name = _albums[row_idx].name;
		album_name = LibraryHelper::convert_search_string(album_name, mask);

		if( album_name.contains(substr))
		{
			return this->index(row_idx, 0);
		}
	}

	return this->index(-1, -1);
}
