/* SomaFMStationModel.cpp */

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



/* SomaFMStationModel.cpp */

#include "SomaFMStationModel.h"
#include "GUI/Helper/GUI_Helper.h"
#include "GUI/Helper/CustomMimeData.h"
#include "GUI/Helper/IconLoader/IconLoader.h"



#include <QUrl>

SomaFMStationModel::SomaFMStationModel(QObject *parent) :
	AbstractSearchTableModel(parent)
{
	_status = Status::Waiting;
}


int SomaFMStationModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)

	if(_stations.isEmpty()){
		return 1;
	}

	return _stations.size();
}

int SomaFMStationModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return 2;
}


QVariant SomaFMStationModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();
	int col = index.column();

	if(!index.isValid()){
		sp_log(Log::Debug) << "Index not valid";
		return QVariant();
	}

	if(role == Qt::TextAlignmentRole) {
		return (int)(Qt::AlignVCenter| Qt::AlignLeft);
	}

	if(row < 0 || row >= rowCount()) {
		return QVariant();
	}

	if(role == Qt::DecorationRole) {
		if(_status == Status::Waiting){
			return QVariant();
		}

		if(col == 1){
			return QVariant();
		}

		if(_status == Status::Error){
			return IconLoader::getInstance()->get_icon("view-refresh", "undo");
		}

		bool loved = _stations[row].is_loved();
		if(loved){
			return GUI::get_icon("star.png");
		}

		else{
			return GUI::get_icon("star_disabled.png");
		}
	}

	else if(role == Qt::DisplayRole && col == 1) {
		if(_stations.isEmpty()){
			if(_status == Status::Waiting){
				return tr("Initializing") + "...";
			}

			else if(_status == Status::Error){
				return tr("Cannot fetch stations");
			}

			return QVariant();
		}

		return _stations[row].get_name();
	}

	else if(role == Qt::WhatsThisRole) {
		if(_stations.isEmpty()){
			return QVariant();
		}

		return _stations[row].get_name();
	}

	return QVariant();
}



QModelIndex SomaFMStationModel::getFirstRowIndexOf(QString substr)
{
	return getNextRowIndexOf(substr, 0, QModelIndex());
}

QModelIndex SomaFMStationModel::getNextRowIndexOf(QString substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)
	for(int i=cur_row; i<_stations.size(); i++){

		QString name = _stations[i].get_name();
		QString desc = _stations[i].get_description();

		QString str = name + desc;

		if(str.contains(substr, Qt::CaseInsensitive)){
			return this->index(i, 0);
		}
	}

	return QModelIndex();

}

QModelIndex SomaFMStationModel::getPrevRowIndexOf(QString substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)
	for(int i=cur_row; i>=0; i--){

		QString name = _stations[i].get_name();
		QString desc = _stations[i].get_description();

		QString str = name + desc;

		if(str.contains(substr, Qt::CaseInsensitive)){
			return this->index(i, 0);
		}
	}

	return QModelIndex();
}

QMap<QChar, QString> SomaFMStationModel::getExtraTriggers()
{
	return QMap<QChar, QString>();
}

void SomaFMStationModel::set_stations(const QList<SomaFMStation>& stations)
{
	int n_stations = stations.size();

	if(n_stations == 0){
		_status = Status::Error;
		emit dataChanged( index(0,0), index(0, 1) );
		return;
	}

	_status = Status::OK;

	beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
	this->removeRows(0, this->rowCount());
	endRemoveRows();

	this->insertRows(0, n_stations);

	beginInsertRows(QModelIndex(), 0, n_stations - 1);
	_stations = stations;
	endInsertRows();

	emit dataChanged( index(0, 0), index(n_stations - 1, 1));
}

void SomaFMStationModel::replace_station(const SomaFMStation& station)
{
	for(int i=0; i<_stations.size(); i++){

		if(station.get_name() == _stations[i].get_name()){

			_stations[i] = station;

			emit dataChanged(this->index(i, 0), this->index(i, 1));
			return;
		}
	}
}

bool SomaFMStationModel::has_stations() const
{
	return (_stations.size() > 0);
}

void SomaFMStationModel::set_waiting(){
	_status = Status::Waiting;
	emit dataChanged( index(0,0), index(0, 1) );
}


QMimeData* SomaFMStationModel::mimeData(const QModelIndexList& indexes) const
{
	QList<QUrl> urls;
	QString cover_url;

	for(const QModelIndex& idx : indexes){
		if(idx.column() == 0){
			continue;
		}

		int row = idx.row();
		if(!between(row, _stations)){
			continue;
		}

		QStringList str_urls = _stations[row].get_urls();

		for(const QString& str_url : str_urls){
			urls << QUrl(str_url);
			cover_url = _stations[row].get_cover_location().search_url();
		}
	}

	QMimeData* mime_data = new QMimeData();
	mime_data->setText(cover_url);
	mime_data->setUrls(urls);

	return mime_data;
}


Qt::ItemFlags SomaFMStationModel::flags(const QModelIndex& index) const
{

	switch(_status){
		case Status::Waiting:
			return (Qt::NoItemFlags);
		case Status::Error:
			if(index.column() == 0){
				return Qt::ItemIsEnabled;
			}
			return (Qt::NoItemFlags);
		default:
			return (Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
	}
}
