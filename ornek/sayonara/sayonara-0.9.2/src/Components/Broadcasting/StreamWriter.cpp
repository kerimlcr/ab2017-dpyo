/* StreamWriter.cpp */

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

/* This module is the interface between the parser and the data sender
 */

#include "StreamWriter.h"
#include "StreamDataSender.h"
#include "Components/Engine/EngineHandler.h"
#include "Helper/MetaData/MetaData.h"

// socket is the client socket
StreamWriter::StreamWriter(QTcpSocket* socket, const QString& ip, const MetaData& md) :
	RawSoundReceiverInterface(),
	SayonaraClass()
{
	_sender = new StreamDataSender(socket);
	_parser = new StreamHttpParser();
	_ip = ip;

	_engine = EngineHandler::getInstance();
	_send_data = false;
	_dismissed = false;

	_stream_title = md.title + " by " + md.artist;
	_socket = socket;

	_type = StreamWriter::Type::Undefined;

	if(_socket->bytesAvailable()){
		data_available();
	}

	connect(socket, &QTcpSocket::disconnected, this, &StreamWriter::socket_disconnected);
	connect(socket, &QTcpSocket::readyRead, this, &StreamWriter::data_available);
}

StreamWriter::~StreamWriter(){

	_engine->unregister_raw_sound_receiver(this);

	if(_parser){
		delete _parser; _parser = nullptr;
	}

	if(_sender){
		delete _sender; _sender = nullptr;
	}
}


QString StreamWriter::get_ip() const
{
	return _ip;
}


int StreamWriter::get_sd() const
{
	return _socket->socketDescriptor();
}


StreamHttpParser::HttpAnswer StreamWriter::parse_message(){

	StreamHttpParser::HttpAnswer status;
	status = _parser->parse(_socket->readAll());

	sp_log(Log::Debug) << "Parse message " << (int) status;

	return status;
}

void StreamWriter::new_audio_data(const uchar* data, quint64 size){

	if(!_send_data) {
		return;
	}

	if(_dismissed){
		_sender->send_trash();
		return;
	}

	if(_parser->is_icy()){
		_sender->send_icy_data(data, size, _stream_title);
	}

	else{
		_sender->send_data(data, size);
	}
}

bool StreamWriter::send_playlist(){
	return _sender->send_playlist(_parser->get_host(), _socket->localPort());
}

bool StreamWriter::send_favicon(){
	return _sender->send_favicon();
}

bool StreamWriter::send_metadata(){

	return _sender->send_metadata(_stream_title);
}

bool StreamWriter::send_bg(){
	return _sender->send_bg();
}

bool StreamWriter::send_html5(){

	return _sender->send_html5(_stream_title);
}

bool StreamWriter::send_header(bool reject){
	return _sender->send_header(reject, _parser->is_icy());
}



void StreamWriter::change_track(const MetaData& md){
	_stream_title = md.title + " by " + md.artist;
}

void StreamWriter::dismiss(){

	_engine->unregister_raw_sound_receiver(this);
	_dismissed = true;
}


void StreamWriter::disconnect(){
	
	dismiss();
	
	_socket->disconnectFromHost();
}


void StreamWriter::socket_disconnected(){

	_engine->unregister_raw_sound_receiver(this);
	emit sig_disconnected(this);
}


void StreamWriter::data_available(){

	StreamHttpParser::HttpAnswer answer = parse_message();
	QString ip = get_ip();
	bool success;
	bool close_connection = true;
	_type = StreamWriter::Type::Standard;

	switch(answer){

		case StreamHttpParser::HttpAnswer::Fail:
		case StreamHttpParser::HttpAnswer::Reject:

			_type = StreamWriter::Type::Invalid;
			//sp_log(Log::Debug) << "Rejected: " << _parser->get_user_agent() << ": " << get_ip();
			send_header(true);
			break;

		case StreamHttpParser::HttpAnswer::Ignore:
			//sp_log(Log::Debug) << "ignore...";
			break;

		case StreamHttpParser::HttpAnswer::Playlist:
			//sp_log(Log::Debug) << "Asked for playlist";
			send_playlist();
			break;

		case StreamHttpParser::HttpAnswer::HTML5:
			//sp_log(Log::Debug) << "Asked for html5";
			send_html5();

			break;

		case StreamHttpParser::HttpAnswer::BG:
			//sp_log(Log::Debug) << "Asked for background";
			send_bg();
			break;

		case StreamHttpParser::HttpAnswer::Favicon:
			//sp_log(Log::Debug) << "Asked for favicon";
			send_favicon();
			break;

		case StreamHttpParser::HttpAnswer::MetaData:
			//sp_log(Log::Debug) << "Asked for metadata";
			send_metadata();
			break;

		default:
			_type = StreamWriter::Type::Streaming;
			close_connection = false;
			//sp_log(Log::Debug) << "Accepted: " << _parser->get_user_agent() << ": " << ip;
			success = send_header(false);

			if(success){
				_send_data = true;
				_engine->register_raw_sound_receiver(this);
			}

			emit sig_new_connection(ip);
			break;
	}

	if(close_connection){
		_socket->close();
	}
}


StreamWriter::Type StreamWriter::get_type() const
{
	return _type;
}
