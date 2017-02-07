/* StreamParser.cpp */

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


#include "StreamParser.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/Parser/PlaylistParser.h"
#include "Helper/Parser/PodcastParser.h"
#include "Helper/UrlHelper.h"

StreamParser::StreamParser(const QString& station_name, QObject* parent) : 
	QObject(parent)
{
	_station_name = station_name;
}

void StreamParser::parse_streams(const QStringList& urls)
{
	_v_md.clear();
	_urls = urls;
	parse_next();
}

void StreamParser::parse_stream(const QString& url)
{
	parse_streams( {url} );
}

bool StreamParser::parse_next(){

	if(_urls.isEmpty()){
		emit sig_finished(_v_md.size() > 0);
		return false;
	}

	QString url = _urls.takeFirst();
	AsyncWebAccess* awa = new AsyncWebAccess(this);
	awa->set_behavior(AsyncWebAccess::Behavior::AsSayonara);
	connect(awa, &AsyncWebAccess::sig_finished, this, &StreamParser::awa_finished);
	awa->run(url, 3000);

	return true;
}


void StreamParser::awa_finished(bool success)
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());

	if(!success){
		sp_log(Log::Warning) << "Stream error: "<< awa->get_url();

		awa->deleteLater();

		if(!_stream_buffer.isEmpty()){
			QString new_station = _stream_buffer.takeFirst();
			sp_log(Log::Debug) << "Try out another one: "<< new_station;
			parse_stream(new_station);
		}

		else {
			parse_next();
		}

		return;
	}

	_stream_buffer.clear();
	_last_url = awa->get_url();

	QByteArray data = awa->get_data();
	MetaDataList v_md;

	awa->deleteLater();

	/** Let's look what's inside data **/
	if(!data.isEmpty()){

		v_md = parse_content(data);
		if(v_md.isEmpty()){
			parse_next();
			return;
		}
	}

	/** This is an ordianry stream and is tagged lateri **/
	else {
		MetaData md;
		v_md << md;
	}

	for(MetaData& md : v_md){
		tag_metadata(md, _last_url);
		if(!_cover_url.isEmpty()){
			md.cover_download_url = _cover_url;
		}
	}

	_v_md << v_md;

	parse_next();
}


MetaDataList StreamParser::parse_content(const QByteArray& data){

	MetaDataList v_md;

	/** 1. try if podcast file **/
	PodcastParser::parse_podcast_xml_file_content(data, v_md);

	/** 2. try if playlist file **/
	if(v_md.isEmpty()){
		QString filename = write_playlist_file(data);
		PlaylistParser::parse_playlist(filename, v_md);
		QFile::remove(filename);
	}

	/** 3. search for a playlist file on website **/
	if(v_md.isEmpty()){

		_stream_buffer = search_for_playlist_files(data);
		if(_stream_buffer.isEmpty()){
			return MetaDataList();
		}

		QString playlist_file = _stream_buffer.takeFirst();
		sp_log(Log::Debug) << "try out " << playlist_file;
		parse_stream(playlist_file);

		return MetaDataList();
	}

	return v_md;
}

void StreamParser::tag_metadata(MetaData &md, const QString& stream_url) const
{

	if(_station_name.isEmpty()){
		md.album = stream_url;
		if(md.title.isEmpty()){
			md.title = tr("Radio");
		}
	}

	else{
		md.album = _station_name;
		if(md.title.isEmpty()){
			md.title = _station_name;
		}
	}

	if(md.artist.isEmpty()){
		md.artist = stream_url;
	}

	if(md.filepath().isEmpty()){
		md.set_filepath(stream_url);
	}
}


QString StreamParser::write_playlist_file(const QByteArray& data) const
{
	QString filename, extension;

	extension = Helper::File::get_file_extension(_last_url);
	filename = Helper::get_sayonara_path() + QDir::separator() + "tmp_playlist";

	if(!extension.isEmpty()){
		filename += "." + extension;
	}

	Helper::File::write_file(data, filename);
	return filename;
}


QStringList StreamParser::search_for_playlist_files(const QByteArray& data) const
{

	QStringList playlist_strings;
	QString base_url = Helper::Url::get_base_url(_last_url);

	QRegExp re("href=\"([^<]+\\.(pls|m3u|asx))\"");
	re.setMinimal(true);

	QString utf8_data = QString::fromUtf8(data);
	int idx = re.indexIn(utf8_data);

	while(idx > 0){

		QString playlist = re.cap(1);
		if(!playlist.startsWith("http")){
			playlist = base_url + "/" + playlist;
		}

		sp_log(Log::Debug) << "Found a playlist on website: " << playlist;
		playlist_strings << playlist;

		idx = re.indexIn(utf8_data, idx+1);
	}

	sp_log(Log::Debug) << "Found " << playlist_strings.size() << " playlists on website: ";
	return playlist_strings;
}

MetaDataList StreamParser::get_metadata() const
{
	return _v_md;
}

void StreamParser::set_cover_url(const QString& url)
{
	_cover_url = url;

	if(!_v_md.isEmpty()){
	
		for(MetaData& md : _v_md){
			md.cover_download_url = url;
		}
	}
}
