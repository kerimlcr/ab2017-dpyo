/* SomaFMLibrary.h */

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



/* SomaFMLibrary.h */

#ifndef SOMAFMLIBRARY_H
#define SOMAFMLIBRARY_H

#include <QObject>
#include <QStringList>
#include <QImage>
#include <QList>
#include <QMap>
#include <QSettings>

#include "SomaFMStation.h"

class SomaFMLibrary : public QObject
{

	Q_OBJECT

signals:
	void sig_stations_loaded(const QList<SomaFMStation>& stations);
	void sig_station_changed(const SomaFMStation& station);


public:
	SomaFMLibrary(QObject* parent=nullptr);
	virtual ~SomaFMLibrary();

	SomaFMStation get_station(const QString& name);
	void create_playlist_from_station(int idx);
	void create_playlist_from_playlist(int idx);
	void search_stations();
	void set_station_loved(const QString& station_name, bool loved);


private slots:
	void soma_website_fetched(bool success);
	void soma_playlist_content_fetched(bool success);
	void soma_station_playlists_fetched(bool success);


private:
	QMap<QString, SomaFMStation> 	_station_map;
	QString 						_requested_station;
	QSettings*						_qsettings=nullptr;

private:
	void sort_stations(QList<SomaFMStation>& stations);
};


#endif // SOMAFMLIBRARY_H
