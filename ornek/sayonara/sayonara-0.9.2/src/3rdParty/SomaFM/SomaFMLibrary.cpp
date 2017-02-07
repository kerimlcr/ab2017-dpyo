/* SomaFMLibrary.cpp */

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

/* SomaFMLibrary.cpp */

#include "SomaFMLibrary.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Logger/Logger.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/Parser/StreamParser.h"
#include "Components/Playlist/PlaylistHandler.h"
#include "Helper/Parser/StreamParser.h"

#include <algorithm>

SomaFMLibrary::SomaFMLibrary(QObject* parent) :
	QObject(parent)
{
	QString path = Helper::File::clean_filename(Helper::get_sayonara_path() + "/somafm.ini");

	_qsettings = new QSettings(path, QSettings::IniFormat, this);
}

SomaFMLibrary::~SomaFMLibrary()
{
	_qsettings->deleteLater();
}


void SomaFMLibrary::search_stations()
{
	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished, this, &SomaFMLibrary::soma_website_fetched);

	awa->run("https://somafm.com/listen/");
}
	

SomaFMStation SomaFMLibrary::get_station(const QString& name)
{
	_requested_station = name;
	SomaFMStation station = _station_map[name];
	return station;
}


void SomaFMLibrary::soma_website_fetched(bool success)
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	QList<SomaFMStation> stations;

	if(!success){
		awa->deleteLater();
		emit sig_stations_loaded(stations);
		return;
	}

	QString content = QString::fromUtf8(awa->get_data());
	QStringList station_contents = content.split("<li");


	for(const QString& station_content : station_contents){

		SomaFMStation station(station_content);
		if(!station.is_valid()){
			continue;
		}

		QString station_name = station.get_name();

		bool loved = _qsettings->value(station_name, false).toBool();
			
		station.set_loved( loved );

		_station_map[station_name] = station;
		stations << station;
	}

	sort_stations(stations);
	emit sig_stations_loaded(stations);

	awa->deleteLater();
}

void SomaFMLibrary::create_playlist_from_station(int row)
{
	Q_UNUSED(row)

	SomaFMStation station = _station_map[_requested_station];
	StreamParser* parser = new StreamParser(station.get_name(), this);
	connect(parser, &StreamParser::sig_finished, this, &SomaFMLibrary::soma_station_playlists_fetched);
	parser->parse_streams(station.get_urls());
}

void SomaFMLibrary::soma_station_playlists_fetched(bool success)
{
	StreamParser* parser = dynamic_cast<StreamParser*>(sender());

	if(!success){
		parser->deleteLater();
		return;
	}

	MetaDataList v_md  = parser->get_metadata();
	SomaFMStation station = _station_map[_requested_station];
	QString cover_url = station.get_cover_location().search_url();

	for(auto it = v_md.begin(); it != v_md.end(); it++){
		it->cover_download_url = cover_url;
	}

	station.set_metadata(v_md);

	_station_map[_requested_station] = station;

	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->create_playlist(v_md,
						 station.get_name(),
						 true,
						 Playlist::Type::Stream);

	parser->deleteLater();
}


void SomaFMLibrary::create_playlist_from_playlist(int idx)
{
	SomaFMStation station = _station_map[_requested_station];
	QStringList urls = station.get_urls();

	if( !between(idx, urls)) {
		return;		
	}

	QString url = urls[idx];
	StreamParser* stream_parser = new StreamParser(station.get_name(), this);
	connect(stream_parser, &StreamParser::sig_finished, this, &SomaFMLibrary::soma_playlist_content_fetched);

	stream_parser->parse_stream(url);
}


void SomaFMLibrary::soma_playlist_content_fetched(bool success)
{
	StreamParser* parser = static_cast<StreamParser*>(sender());

	if(!success){
		parser->deleteLater();
		return;
	}

	MetaDataList v_md = parser->get_metadata();

	SomaFMStation station = _station_map[_requested_station];
	QString cover_url = station.get_cover_location().search_url();

	for(auto it = v_md.begin(); it != v_md.end(); it++){
		it->cover_download_url = cover_url;
	}

	station.set_metadata(v_md);

	_station_map[_requested_station] = station;

	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->create_playlist(v_md,
						 station.get_name(),
						 true,
						 Playlist::Type::Stream);

	parser->deleteLater();
}


void SomaFMLibrary::set_station_loved(const QString& station_name, bool loved)
{
	_station_map[station_name].set_loved(loved);
	_qsettings->setValue(station_name, loved);

	QList<SomaFMStation> stations;
	for(const QString& key : _station_map.keys()){
		if(key.isEmpty()){
			continue;
		}

		stations << _station_map[key];
	}

	sort_stations(stations);
	emit sig_stations_loaded(stations);
}


void SomaFMLibrary::sort_stations(QList<SomaFMStation>& stations)
{
	auto lambda = [](const SomaFMStation& s1, const SomaFMStation& s2){
		if(s1.is_loved() && !s2.is_loved()){
			return true;
		}

		else if(!s1.is_loved() && s2.is_loved()){
			return false;
		}

		return s1.get_name() < s2.get_name();
	};

	std::sort(stations.begin(), stations.end(), lambda);
}
