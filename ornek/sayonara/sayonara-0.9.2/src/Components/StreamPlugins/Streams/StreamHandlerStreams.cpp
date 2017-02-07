/* StreamHandlerStreams.cpp */

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



#include "StreamHandlerStreams.h"
#include "Database/DatabaseConnector.h"

StreamHandlerStreams::StreamHandlerStreams(QObject* parent) :
	AbstractStreamHandler(parent)
{

}

StreamHandlerStreams::~StreamHandlerStreams(){

}

bool StreamHandlerStreams::get_all_streams(StreamMap& streams){
	return _db->getAllStreams(streams);
}

bool StreamHandlerStreams::add_stream(const QString& station_name, const QString& url){
	return _db->addStream(station_name, url);
}

bool StreamHandlerStreams::delete_stream(const QString& station_name){
	return _db->deleteStream(station_name);
}

bool StreamHandlerStreams::update_url(const QString& station_name, const QString& url){
	return _db->updateStreamUrl(station_name, url);
}

bool StreamHandlerStreams::rename_stream(const QString &station_name, const QString &url){
	return _db->renameStream(station_name, url);
}
