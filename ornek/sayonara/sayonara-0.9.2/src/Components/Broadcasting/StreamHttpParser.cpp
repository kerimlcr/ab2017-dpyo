/* StreamHttpParser.cpp */

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



#include "StreamHttpParser.h"
#include "Helper/Logger/Logger.h"

#include <QRegExp>
#include <QStringList>

StreamHttpParser::StreamHttpParser()
{
	_icy = false;
	_host = QString();
	_user_agent = QString();
	_status = HttpAnswer::OK;
}

StreamHttpParser::~StreamHttpParser()
{

}


StreamHttpParser::HttpAnswer StreamHttpParser::parse(const QByteArray& data){

	bool get_playlist = false;
	bool get_received = false;
	bool get_mp3 = false;
	bool get_bg = false;
	bool get_favicon=false;
	bool get_metadata=false;
	bool icy=false;
	bool is_browser=false;

	QString qmsg(data);
	QStringList lst;

	_icy = false;
	_host = "";
	_user_agent ="";

	if(data.isEmpty()) {
		sp_log(Log::Error) << "Fail.. Cannot read from socket";
		return HttpAnswer::Fail;
	}

	lst = qmsg.split("\r\n");

	sp_log(Log::Debug) << qmsg;

	for(const QString& str : lst){

		QRegExp regex("(GET|HEAD)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_pl("(GET)(\\s|/)*(playlist.m3u)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_mp3("(GET)(\\s|/)*(track.mp3)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_bg("(GET)(\\s|/)*(bg-checker.png)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_favicon("(GET)(\\s|/)*(favicon.ico)(\\s|/)*HTTP", Qt::CaseInsensitive);
		QRegExp regex_metadata("(GET)(\\s|/)*(metadata)(\\s|/)*HTTP", Qt::CaseInsensitive);

		if(str.contains(regex)){
			get_received = true;
			continue;
		}

		if(str.contains(regex_metadata)){
			get_metadata = true;
		}

		if(str.contains(regex_favicon)){
			get_favicon = true;
			continue;
		}

		if(str.contains(regex_pl)){
			get_playlist = true;
			continue;
		}

		if(str.contains(regex_mp3)){
			get_mp3 = true;
			continue;
		}

		if(str.contains(regex_bg)){
			get_bg = true;
			continue;
		}

		if(str.toLower().contains("host:")){
			QStringList lst = str.split(":");
			if(lst.size() > 1){
				_host = lst[1].trimmed();
				//sp_log(Log::Info) << "Host = " << _host;

			}
		}

		if( str.contains("icy-metadata:", Qt::CaseInsensitive) ){
			if(str.contains(":1") || str.contains(": 1")){
				icy = true;
				continue;
			}
		}

		if(str.contains("user-agent", Qt::CaseInsensitive)){

			if(str.size() > 11){
				_user_agent = str.right( str.size() - 11).toLower();
				if( _user_agent.contains("firefox", Qt::CaseInsensitive) ||
					_user_agent.contains("mozilla", Qt::CaseInsensitive) ||
					_user_agent.contains("safari", Qt::CaseInsensitive) ||
					_user_agent.contains("internet explorer", Qt::CaseInsensitive) ||
					_user_agent.contains("opera", Qt::CaseInsensitive) ||
					_user_agent.contains("chrom", Qt::CaseInsensitive))
				{
						is_browser = true;
				}

				if(_user_agent.contains("sayonara", Qt::CaseInsensitive)){
					get_playlist = true;
					continue;
				}
			}
		}
	}

	if(is_browser && get_favicon && !_host.isEmpty()){
		return HttpAnswer::Favicon;
	}

	if(is_browser && get_bg && !_host.isEmpty()){
		return HttpAnswer::BG;
	}

	if(is_browser && get_metadata && !_host.isEmpty()){
		return HttpAnswer::MetaData;
	}

	if(is_browser && !get_mp3 && !_host.isEmpty()){
		return HttpAnswer::HTML5;
	}

	if(is_browser && get_mp3 && !_host.isEmpty()){
		return HttpAnswer::MP3;
	}

	if(get_playlist && !_host.isEmpty()){
		return HttpAnswer::Playlist;
	}

	if(get_received){
		_icy = icy;

		return HttpAnswer::OK;
	}

	return HttpAnswer::Fail;
}

bool StreamHttpParser::is_icy() const{
	return _icy;
}

QString StreamHttpParser::get_host() const{
	return _host;
}

QString StreamHttpParser::get_user_agent() const{
	return _user_agent;
}

