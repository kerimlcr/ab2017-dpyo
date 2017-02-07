/* StreamDataSender.cpp */

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

/* This module is responsible for sending data
 * (stream data, http data) to the client
 * Most of the functions are called by Streamwriter
 */

#include "StreamDataSender.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Logger/Logger.h"

#include <QByteArray>

static char padding[256];

StreamDataSender::StreamDataSender(QTcpSocket* socket)
{
	memset(padding, 0, 256);

	_sent_data_bytes = 0;
	_socket = socket;

	_header = QByteArray(
				"ICY 200 Ok\r\n"
				"icy-notice1:Bliblablupp\r\n"
				"icy-notice2:asdfasd\r\n"
				"icy-name:Sayonara Player Radio\r\n"
				"icy-genre:Angry songs\r\n"
				"icy-url:http://sayonara-player.com\r\n"
				"icy-pub:1\r\n"
				"icy-br:192\r\n"
				"Accept-Ranges:none\r\n"
				"content-type:audio/mpeg\r\n"
				"connection:keep-alive\r\n"
				);

	_icy_header = QByteArray(
				"ICY 200 Ok\r\n"
				"icy-notice1:Bliblablupp\r\n"
				"icy-notice2:asdfasd\r\n"
				"icy-name:Sayonara Player Radio\r\n"
				"icy-genre:Angry songs\r\n"
				"icy-url:http://sayonara-player.com\r\n"
				"icy-pub:1\r\n"
				"icy-br:192\r\n"
				"icy-metaint:8192\r\n"
				"Accept-Ranges:none\r\n"
				"content-type:audio/mpeg\r\n"
				"connection:keep-alive\r\n"
				);


	_reject_header = QByteArray("HTTP/1.1 501 Not Implemented\r\nConnection: close\r\n");

	_header.append("\r\n");
	_icy_header.append("\r\n");
	_reject_header.append("\r\n");
}

StreamDataSender::~StreamDataSender()
{

}


bool StreamDataSender::send_trash(){
	char single_byte = 0x00;
	qint64 n_bytes;

	n_bytes = _socket->write(&single_byte, 1);

	_socket->disconnectFromHost();
	_socket->close();

	return (n_bytes > 0);
}

bool StreamDataSender::send_data(const uchar* data, quint64 size){

	qint64 n_bytes;

	_sent_data_bytes = 0;

	n_bytes = _socket->write( (const char*) data, size);

	return (n_bytes > 0);
}


// [..........................................................] = buffer
// [  bytes_before        | icy_data | bytes_to_write ]

bool StreamDataSender::send_icy_data(const uchar* data, quint64 size, const QString& stream_title){

	qint64 n_bytes=0;
	qint64 bytes_to_write = 0;
	const int IcySize = 8192;

	if(_sent_data_bytes + size > IcySize){

		quint64 bytes_before = IcySize - _sent_data_bytes;

		if(bytes_before > 0){
			n_bytes = _socket->write( (const char*) data, bytes_before);
		}

		send_icy_metadata(stream_title);
		bytes_to_write = size - bytes_before;

		// this happens if size > 8192
		if(bytes_to_write > IcySize){
			n_bytes = _socket->write( (const char*) (data + bytes_before), IcySize);
			bytes_to_write = bytes_to_write - n_bytes;
		}

		else if(bytes_to_write > 0){
			n_bytes = _socket->write( (const char*) (data + bytes_before), bytes_to_write);
			bytes_to_write = 0;
		}

		// zero bytes left, so we start at zero again
		else{
			n_bytes = 0;
			bytes_to_write = 0;
		}

		_sent_data_bytes = n_bytes;
	}

	else{
		n_bytes = _socket->write( (const char*) data, size);
		_sent_data_bytes += n_bytes;
		if(_sent_data_bytes > IcySize){
			sp_log(Log::Debug) << "Something is wrong";
		}
	}

	if(bytes_to_write > 0){
		send_icy_data(data + IcySize, bytes_to_write, stream_title);
	}

	return (n_bytes > 0);
}


bool StreamDataSender::send_icy_metadata(const QString& stream_title){

	bool success;
	qint64 n_bytes=0;
	QByteArray metadata = QByteArray("StreamTitle='");
	int sz; // size of icy metadata
	int n_padding; // number of padding bytes

	metadata.append( stream_title.toLocal8Bit() );
	metadata.append("';");
	metadata.append("StreamUrl='http://sayonara-player.com';");

	sz = metadata.size();
	n_padding = ( (int)((sz + 15) / 16) * 16 - sz );

	metadata.append(padding, n_padding);
	metadata.prepend((char) (int)((sz + 15) / 16));

	n_bytes = _socket->write( metadata );

	success = (n_bytes > 0);

	return success;
}


bool StreamDataSender::send_header(bool reject, bool icy){
	qint64 n_bytes=0;

	if(reject){
		n_bytes = _socket->write( _reject_header );
	}

	else if(icy){
		n_bytes = _socket->write( _icy_header );
	}

	else{
		n_bytes = _socket->write( _header );
	}

	if(n_bytes <= 0){
		return false;
	}

	if(reject){
		return false;
	}

	return true;
}



bool StreamDataSender::send_html5(const QString& stream_title){

	int n_bytes;
	QByteArray html;
	QByteArray data;

	html = QByteArray() +
			"<!DOCTYPE html>"
			"<html>"
			"<head>"
			"<link rel=\"icon\" href=\"favicon.ico\" type=\"image/x-icon\" />"
			"<title>Sayonara Player Radio</title>"

			"<script>\n"

			"function loadXMLDoc()\n"
			"{\n"
			"  var xmlhttp;\n"
			"  if (window.XMLHttpRequest)\n"
			"  {// code for IE7+, Firefox, Chrome, Opera, Safari\n"
			"    xmlhttp=new XMLHttpRequest();\n"
			"  }\n"
			"  else\n"
			"  {// code for IE6, IE5\n"
			"    xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");\n"
			"  }\n"
			"  xmlhttp.onreadystatechange=function()\n"
			"  {\n"
			"    if (xmlhttp.readyState==4 && xmlhttp.status==200)\n"
			"    {\n"
			"      document.getElementById(\"metadata\").innerHTML=xmlhttp.responseText;\n"
			"    }\n"
			"  }\n"
			"  xmlhttp.open(\"GET\",\"metadata\",true);\n"
			"  xmlhttp.send();\n"
			"}\n"

			"function start(){\n"
			"}"
			"</script>"

			"</head>"
			"<body background=\"bg-checker.png\" onload=\"start()\" style=\"width: 400px; margin: 0 auto; text-align: center;\" >"

			"<h1 style=\"color: #f3841a; font-family: Fredoka One, lucida grande, tahoma, sans-serif; font-weight: 400;\">Sayonara Player Radio</h1>"
			"<audio id=\"player\" autoplay controls>"
			"<source src=\"track.mp3\" type=\"audio/mpeg\">"
			"Your browser does not support the audio element."
			"</audio><br /><br />"

			"<div id=\"metadata\" style=\"color: white;\">"
			+ stream_title.toLocal8Bit() +
			"</div><br />"

			"<div style=\"color: white;\">"
			"<button type=\"button\" onclick=\"loadXMLDoc()\" style=\"border: 1px solid #282828; background: #505050; padding: 4px; color: white;\">Refresh</button><br /><br />"
			"</div><br /><br />"
			"<div style=\"color: white;\">"
			"Stream by Lucio Carreras"
			"</div>"

			"</body>"
			"</html>";

	data = QByteArray("HTTP/1.1 200 OK\r\n"
					  "content-type: text/html\r\n"
					  "content-length: " + QString::number(html.size()).toLocal8Bit() +
					  "\r\nConnection: keep-alive\r\n\r\n" +
					  html
					  );

	n_bytes = _socket->write(data);

	return (n_bytes > 0);
}


bool StreamDataSender::send_bg(){

	bool success;
	int n_bytes;
	QByteArray html;
	QByteArray data;

	success = Helper::File::read_file_into_byte_arr( Helper::get_share_path() + "bg-checker.png", html );
	if(!success){
		return false;
	}

	data = QByteArray("HTTP/1.1 200 OK\r\n"
					  "content-type: image/png\r\n"
					  "content-length: " + QString::number(html.size()).toLocal8Bit() +
					  "\r\nConnection: close\r\n\r\n" +
					  html
					  );

	n_bytes = _socket->write(data);

	return (n_bytes > 0);
}


bool StreamDataSender::send_metadata(const QString& stream_title){

	int n_bytes;
	QByteArray html;
	QByteArray data;

	html = stream_title.toLocal8Bit();
	data = QByteArray("HTTP/1.1 200 OK\r\n"
					  "content-type: text/plain\r\n"
					  "content-length: " + QString::number(html.size()).toLocal8Bit() +
					  "\r\nConnection: close\r\n\r\n" +
					  html
					  );

	n_bytes = _socket->write(data);

	return (n_bytes > 0);
}


bool StreamDataSender::send_playlist(const QString& host, int port){

	qint64 n_bytes;
	QByteArray playlist;
	QByteArray data;

	playlist = QByteArray("#EXTM3U\n\n"
						  "#EXTINF:-1, Lucio Carreras - Sayonara Player Radio\n"
						  "http://" + host.toLocal8Bit() +
						  ":" + QString::number(port).toLocal8Bit() + "\n");

	data = QByteArray("HTTP/1.1 200 OK\r\n"
					  "content-type:audio/x-mpegurl\r\n"
					  "Content-Length: " +
					  QString::number(playlist.size()).toLocal8Bit() +
					  "\r\n"
					  "Connection: close\r\n\r\n" +
					  playlist
					  );

	n_bytes = _socket->write(data);

	return (n_bytes > 0);
}


bool StreamDataSender::send_favicon(){

	int n_bytes;
	bool success;
	QByteArray arr;
	QByteArray data;

	success = Helper::File::read_file_into_byte_arr( Helper::get_share_path() + "favicon.ico", arr );

	if(!success){
		return false;
	}

	data = QByteArray("HTTP/1.1 200 OK\r\n"
					  "content-type: image/x-icon\r\n"
					  "content-length: " + QString::number(arr.size()).toLocal8Bit() +
					  "\r\nConnection: close\r\n\r\n" +
					  arr
					  );


	n_bytes = _socket->write(data);

	return (n_bytes > 0);
}
