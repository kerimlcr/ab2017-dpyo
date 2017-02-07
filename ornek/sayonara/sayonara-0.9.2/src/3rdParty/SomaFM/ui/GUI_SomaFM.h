/* GUI_SomaFM.h */

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



/* GUI_SomaFM.h */

#ifndef GUI_SOMAFM_H
#define GUI_SOMAFM_H


#include "SomaFMLibrary.h"
#include "SomaFMStation.h"

#include "3rdParty/SomaFM/ui_GUI_SomaFM.h"
#include "Components/Covers/CoverLocation.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"

#include <QItemSelection>

class SomaFMStationModel;

class GUI_SomaFM :
		public SayonaraWidget,
		protected Ui::GUI_SomaFM

{
	Q_OBJECT

private:
	SomaFMLibrary*	_library=nullptr;


public:
	explicit GUI_SomaFM(QWidget *parent = 0);
	~GUI_SomaFM();

	QComboBox*	get_libchooser() const;


private slots:
	void stations_loaded(const QList<SomaFMStation>& stations);
	void station_changed(const SomaFMStation& station);

	void station_double_clicked(const QModelIndex& idx);
	void station_clicked(const QModelIndex& idx);
	void station_index_changed(const QModelIndex& idx);
	void playlist_double_clicked(const QModelIndex& idx);
	void cover_found(const QString& cover_path);

	void selection_changed(const QModelIndexList& selected);

private:
	SomaFMStation get_station(int row) const;
};

#endif // GUI_SOMAFM_H
