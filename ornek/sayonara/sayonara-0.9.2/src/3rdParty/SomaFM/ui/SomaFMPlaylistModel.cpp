/* SomaFMPlaylistModel.cpp */

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



/* SomaFMPlaylistModel.cpp */

#include "SomaFMPlaylistModel.h"
#include "GUI/Helper/CustomMimeData.h"

SomaFMPlaylistModel::SomaFMPlaylistModel(QObject* parent) :
    QStringListModel(parent)
{

}


void SomaFMPlaylistModel::setStation(const SomaFMStation& station)
{
    _station = station;

    QStringList urls = station.get_urls();
    QStringList entries;

    for(QString& url : urls){
	    SomaFMStation::UrlType type = station.get_url_type(url);
	    if(type == SomaFMStation::UrlType::MP3){
		    entries << station.get_name() + " (mp3)";
	    }

	    else if(type == SomaFMStation::UrlType::AAC){
		    entries << station.get_name() + " (aac)";
	    }

	    else{
		    entries << url;
	    }
    }

    this->setStringList(entries);
}

QMimeData* SomaFMPlaylistModel::mimeData(const QModelIndexList& indexes) const
{
    if(indexes.isEmpty()){
	return nullptr;
    }

    int row = indexes[0].row();

    QStringList urls = _station.get_urls();
	if(!between(row, urls)){
		return nullptr;
    }

    QUrl url( urls[row] );

    QMimeData* mime_data = new QMimeData();
	CoverLocation location = _station.get_cover_location();

    mime_data->setUrls({url});
	mime_data->setText(location.search_url());

    return mime_data;
}
