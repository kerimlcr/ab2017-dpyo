/* LibraryItemModel.cpp */

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


#include "LibraryItemModel.h"
#include "GUI/Library/Helper/ColumnHeader.h"
#include "GUI/Helper/CustomMimeData.h"

#include <algorithm>


LibraryItemModel::LibraryItemModel() {

	_n_cols = 0;
	_n_rows = 0;
}

LibraryItemModel::~LibraryItemModel() {

}


QVariant LibraryItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const{

	if (role != Qt::DisplayRole){
		return QVariant();
	}

	if(!between(section, _header_names)){
		return QVariant();
	}

	if (orientation == Qt::Horizontal){
		return _header_names[section];
	}

	return QVariant();
}


bool LibraryItemModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	Q_UNUSED(role)

	if(!between(section, _header_names)){
		return false;
	}

	if(orientation == Qt::Horizontal){
		_header_names[section] = value.toString();
		emit headerDataChanged(orientation, section, section);
	}

	return true;
}



int LibraryItemModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return _n_rows;
}


QStringList LibraryItemModel::get_header_names() const {
	return _header_names;
}

int LibraryItemModel::columnCount(const QModelIndex& parent) const{

	Q_UNUSED(parent);

	return _header_names.size();
}


bool LibraryItemModel::insertColumns(int position, int cols, const QModelIndex &index) {
	Q_UNUSED(index)

	beginInsertColumns(QModelIndex(), position, position+cols-1);

	for(int i=position; i<position+cols; i++) {
		_header_names.insert(i, "");
	}

	endInsertColumns();
	return true;
}


bool LibraryItemModel::removeColumns(int position, int cols, const QModelIndex &index) {
	Q_UNUSED(index)

	beginRemoveColumns(QModelIndex(), position, position+cols-1);

	for(int i=position; i<position+cols; i++) {
		_header_names.removeAt(position);
	}

	endRemoveColumns();
	return true;
}

bool LibraryItemModel::removeRows(int row, int count, const QModelIndex& index){
	Q_UNUSED(index)

	beginRemoveRows(QModelIndex(), row, row + count - 1);
	_n_rows -= count;
	for(int i=row; i<row + count; i++){
		_selections.remove( get_id_by_row(i) );
	}

	endRemoveRows();

	return true;
}

bool LibraryItemModel::insertRows(int row, int count, const QModelIndex& index)
{
	Q_UNUSED(index)
	beginInsertRows(QModelIndex(), row, row + count - 1);
	_n_rows += count;
	endInsertRows();

	return true;
}


QMap<QChar, QString> LibraryItemModel::getExtraTriggers() {
	QMap<QChar, QString> map;
	return map;
}

void LibraryItemModel::set_mimedata(const MetaDataList& v_md){
	_md_mimedata = v_md;
}

CustomMimeData* LibraryItemModel::get_mimedata(){
	CustomMimeData* mimedata = new CustomMimeData();
	QList<QUrl> urls;

	for(const MetaData& md : _md_mimedata){
		QUrl url(QString("file://") + md.filepath());
		urls << url;
	}

	mimedata->setMetaData(_md_mimedata);
	mimedata->setText("tracks");
	mimedata->setUrls(urls);

	return mimedata;
}


bool LibraryItemModel::has_selections(){
	return !(_selections.isEmpty());
}


void LibraryItemModel::add_selections(const SP::Set<int>& rows){

	std::for_each(rows.begin(), rows.end(), [=](int row){
		_selections.insert(get_id_by_row(row));
	});
}

void LibraryItemModel::add_selection(int row){
	_selections.insert(get_id_by_row(row));
}

bool LibraryItemModel::is_selected(int id)
{
	return _selections.contains(id);
}


void LibraryItemModel::remove_selection(int id){

	auto it = _selections.find(id);

	if(it != _selections.end()){
		_selections.erase(it);
	}
}

void LibraryItemModel::clear_selections(){
	_selections.clear();
}
