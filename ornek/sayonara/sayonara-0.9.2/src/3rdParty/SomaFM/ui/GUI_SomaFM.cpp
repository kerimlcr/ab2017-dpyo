/* GUI_SomaFM.cpp */

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



/* GUI_SomaFM.cpp */

#include "ui/GUI_SomaFM.h"
#include "ui/SomaFMStationModel.h"
#include "ui/SomaFMPlaylistModel.h"

#include "Helper/Helper.h"
#include "GUI/Helper/GUI_Helper.h"
#include "GUI/Helper/Delegates/ListDelegate.h"
#include "Components/Covers/CoverLookup.h"

#include <QPixmap>
#include <QItemDelegate>

GUI_SomaFM::GUI_SomaFM(QWidget *parent) :
	SayonaraWidget(parent),
	Ui::GUI_SomaFM()
{
	setupUi(this);

	_library = new SomaFMLibrary(this);

	SomaFMStationModel* model_stations = new SomaFMStationModel(this);

	tv_stations->setModel(model_stations);
	tv_stations->setAbstractModel(model_stations);
	tv_stations->setItemDelegate(new QItemDelegate(tv_stations));
	tv_stations->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tv_stations->setEnabled(false);
	tv_stations->setColumnWidth(0, 20);

	lv_playlists->setModel(new SomaFMPlaylistModel());
	lv_playlists->setItemDelegate(new ListDelegate(lv_playlists));
	lv_playlists->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QPixmap logo = QPixmap(":/soma_icons/soma_logo.png")
		.scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	lab_image->setPixmap(logo);

	QString description = 
		"Listener-supported, commercial-free, underground/alternative radio<br /><br />" +
		Helper::create_link("https://somafm.com", this->is_dark(), "https://somafm.com");

	lab_description->setText(description);
	lab_donate->setText(Helper::create_link("https://somafm.com/support/", is_dark()));

	connect(_library, &SomaFMLibrary::sig_stations_loaded, this, &GUI_SomaFM::stations_loaded);
	connect(_library, &SomaFMLibrary::sig_station_changed, this, &GUI_SomaFM::station_changed);

	connect(tv_stations, &QListView::activated, this, &GUI_SomaFM::station_index_changed);
	connect(tv_stations, &QListView::clicked, this, &GUI_SomaFM::station_clicked);
	connect(tv_stations, &QListView::doubleClicked, this, &GUI_SomaFM::station_double_clicked);
	connect(tv_stations, &SearchableTableView::sig_selection_changed, this, &GUI_SomaFM::selection_changed);

	connect(lv_playlists, &QListView::doubleClicked, this, &GUI_SomaFM::playlist_double_clicked);
	connect(lv_playlists, &QListView::activated, this, &GUI_SomaFM::playlist_double_clicked);

	_library->search_stations();
}

GUI_SomaFM::~GUI_SomaFM()
{

}

QComboBox* GUI_SomaFM::get_libchooser() const
{
	return combo_lib_chooser;
}

void GUI_SomaFM::stations_loaded(const QList<SomaFMStation>& stations)
{
	sp_log(Log::Debug) << "Stations loaded";
	SomaFMStationModel* model = static_cast<SomaFMStationModel*>(tv_stations->model());
	model->set_stations(stations);

	tv_stations->setEnabled(true);
	tv_stations->setDragEnabled(true);
	tv_stations->setDragDropMode(QAbstractItemView::DragDrop);
}

void GUI_SomaFM::station_changed(const SomaFMStation& station)
{
	SomaFMStationModel* model = static_cast<SomaFMStationModel*>(tv_stations->model());
	model->replace_station(station);
}

void GUI_SomaFM::station_double_clicked(const QModelIndex& idx)
{
	_library->create_playlist_from_station(idx.row());
}

void GUI_SomaFM::selection_changed(const QModelIndexList& indexes){

	if(indexes.isEmpty()){
		return;
	}

	station_index_changed(indexes[0]);
}


SomaFMStation GUI_SomaFM::get_station(int row) const
{
	SomaFMStationModel* station_model = static_cast<SomaFMStationModel*>(tv_stations->model());
	QModelIndex idx = station_model->index(row, 1);
	QString station_name = station_model->data(idx).toString();

	return _library->get_station(station_name);
}

void GUI_SomaFM::station_clicked(const QModelIndex &idx){
	if(!idx.isValid()){
		return;
	}

	SomaFMStationModel* station_model = static_cast<SomaFMStationModel*>(tv_stations->model());
	if(!station_model->has_stations() && idx.column() == 0){
		station_model->set_waiting();
		_library->search_stations();

		return;
	}

	SomaFMStation station = get_station(idx.row());

	if(idx.column() == 0){
		_library->set_station_loved( station.get_name(), !station.is_loved());
	}

	station_index_changed(idx);
}

void GUI_SomaFM::station_index_changed(const QModelIndex& idx){

	if(!idx.isValid()){
		return;
	}

	SomaFMPlaylistModel* pl_model;
	SomaFMStation station = get_station(idx.row());

	pl_model = static_cast<SomaFMPlaylistModel*>(lv_playlists->model());
	pl_model->setStation(station);

	lab_description->setText(station.get_description());

	CoverLookup* cl = new CoverLookup(this);

	connect(cl, &CoverLookup::sig_cover_found, this, &GUI_SomaFM::cover_found);
	cl->fetch_cover(station.get_cover_location());
}


void GUI_SomaFM::playlist_double_clicked(const QModelIndex& idx)
{
	_library->create_playlist_from_playlist(idx.row());
}


void GUI_SomaFM::cover_found(const QString &cover_path){

	CoverLookup* cl = static_cast<CoverLookup*>(sender());
	if(CoverLocation::isInvalidLocation(cover_path)){
		return;
	}

	QPixmap pixmap = QPixmap(cover_path).scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	if(pixmap.isNull()){
		pixmap = QPixmap(":/soma_icons/soma_logo.png").scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	lab_image->setPixmap(pixmap);

	if(cl){
		cl->deleteLater();
	}
}


