/* StreamServer.cpp */

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

#include "StreamServer.h"
#include "Helper/Helper.h"

#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Components/Engine/EngineHandler.h"
#include "GUI/Helper/Message/GlobalMessage.h"

#include <QHostAddress>

StreamServer::StreamServer(QObject* parent) : 
	QThread(parent),
	SayonaraClass()
{
	PlayManager* play_manager;
	EngineHandler* engine;

	_server = nullptr;
	_asking = false;

	create_server();

	play_manager = PlayManager::getInstance();
	engine = EngineHandler::getInstance();

	connect(play_manager, &PlayManager::sig_track_changed, this, &StreamServer::track_changed);

	connect(engine, &EngineHandler::destroyed, this, &StreamServer::stop);

	REGISTER_LISTENER(Set::Broadcast_Active, _sl_active_changed);
	REGISTER_LISTENER_NO_CALL(Set::Broadcast_Port, _sl_port_changed);

	REGISTER_LISTENER(SetNoDB::MP3enc_found, _sl_mp3_enc_found);
}

StreamServer::~StreamServer(){

	server_close();
	disconnect_all();
	delete _server;
	_server = nullptr;
	Helper::sleep_ms(500);
}

void StreamServer::create_server(){
	if(_server != nullptr){
		delete _server;
	}

	_server = new QTcpServer();
	_server->setMaxPendingConnections(10);

	connect(_server, &QTcpServer::newConnection, this, &StreamServer::new_client_request);
	connect(_server, &QTcpServer::destroyed, this, &StreamServer::server_destroyed);

	if( !_server->isListening() ){
		listen_for_connection();
	}
}

void StreamServer::server_destroyed(){
	sp_log(Log::Info) << "Server destroyed";
}

void StreamServer::run(){

	emit sig_can_listen(_server->isListening());

	forever{

		if(!_mp3_enc_available){
			Helper::sleep_ms (100);
		}

		else if(!_settings->get(Set::Broadcast_Active)){
			Helper::sleep_ms(100);
		}

		else if(_server && _server->isListening()){
			Helper::sleep_ms(100);
		}

		if(_server == nullptr){
			break;
		}

		Helper::sleep_ms(250);
	}

	sp_log(Log::Info) << "Radio station: Bye";
}

bool StreamServer::listen_for_connection(){

	bool success;
	int port;

	if(!_server){
		sp_log(Log::Error) << "Server socket invalid";
		return false;
	}

	port = _settings->get(Set::Broadcast_Port);
	success = _server->listen(QHostAddress::Any, port);

	if(!success){
		sp_log(Log::Error) << "Cannot listen on port " << port;
		sp_log(Log::Error) << _server->errorString();

		_server->close();
	}

	else{
		sp_log(Log::Info) << "Listening on port " << port;
	}

	return success;
}

// either show a popup dialog or accept directly
void StreamServer::new_client_request(){

	QTcpSocket* pending_socket;
	QString pending_ip;

	pending_socket = _server->nextPendingConnection();

	if(!pending_socket) {
		return;
	}

	pending_ip = pending_socket->peerAddress().toString();

	if(_discmissed_ips.contains(pending_ip)){
		reject_client(pending_socket, pending_ip);
		_discmissed_ips.removeOne(pending_ip);
		return;
	}

	_pending << QPair<QTcpSocket*, QString>(pending_socket, pending_ip);

	if(_asking){
		return;
	}

	_asking = true;

	do{

		pending_socket = _pending[0].first;
		pending_ip = _pending[0].second;

		if( _settings->get(Set::Broadcast_Prompt) ){

			if(!_allowed_ips.contains(pending_ip)){


				QString question = tr("%1 wants to listen to your music.\nOK?").arg(pending_ip);
				GlobalMessage::Answer answer = GlobalMessage::getInstance()->question(question);
				if(answer==GlobalMessage::Answer::Yes){
					accept_client(pending_socket, pending_ip);
				}
				else{
					reject_client(pending_socket, pending_ip);
				}
			}

			else{
				accept_client(pending_socket, pending_ip);
			}
		}

		else{
			accept_client(pending_socket, pending_ip);
		}

		_pending.pop_front();

	} while(_pending.size() > 0);

	_asking = false;
}

// every kind of request will land here or in reject client.
// so one client will be accepted multiple times until he will be able
// to listen to music
void StreamServer::accept_client(QTcpSocket* socket, const QString& ip){

	StreamWriterPtr sw;

	if(!_allowed_ips.contains(ip)){
		_allowed_ips << ip;
	}

	sp_log(Log::Info) << "New client request from " << ip << " (" << _lst_sw.size() << ")";

	sw = StreamWriterPtr(new StreamWriter(socket, ip, _cur_track));

	connect(sw.get(), &StreamWriter::sig_disconnected, this, &StreamServer::disconnected);
	connect(sw.get(), &StreamWriter::sig_new_connection, this, &StreamServer::new_connection);

	_lst_sw << sw;

	emit sig_new_connection(ip);
}

void StreamServer::reject_client(QTcpSocket* socket, const QString& ip){
	Q_UNUSED(socket);
	Q_UNUSED(ip);
}


// this finally is the new connection when asking for sound
void StreamServer::new_connection(const QString& ip){
	Q_UNUSED(ip)
}



void StreamServer::track_changed(const MetaData& md){
	_cur_track = md;
	for(StreamWriterPtr sw : _lst_sw){
		sw->change_track(md);
	}
}


void StreamServer::server_close(){
	
	if(_server){
		_server->close();
		sp_log(Log::Info) << "Server closed..";
	}
}

// when user forbids further streaming
void StreamServer::dismiss(int idx){

	if( idx >= _lst_sw.size() ) {
		return;
	}

	StreamWriterPtr sw = _lst_sw[idx];
	_discmissed_ips << sw->get_ip();
	_allowed_ips.removeOne(sw->get_ip());

	sw->dismiss();
}

// real socket disconnect (if no further sending is possible)
void StreamServer::disconnect(StreamWriterPtr sw){
	sw->disconnect();
}

void StreamServer::disconnect_all(){

	for(StreamWriterPtr sw : _lst_sw){
		sw->disconnect();
	}

	_lst_sw.clear();
}

// the client disconnected itself
void StreamServer::disconnected(StreamWriter* sw){

	if(!sw) {
		return;
	}

	QString ip = sw->get_ip();
	emit sig_connection_closed(ip);

	// remove the item, garbage collector deletes that item
	for(auto it=_lst_sw.begin(); it != _lst_sw.end(); it++){
		if(sw == it->get()){
			_lst_sw.erase(it);
			break;
		}
	}
}


// this happens when the user tries to look for the codec again
void StreamServer::retry(){

	bool success;

	if(!_mp3_enc_available){
		return;
	}

	if(!_settings->get(Set::Broadcast_Active)){
		return;
	}

	create_server();
	if(_server->isListening()){
		success = true;
	}

	else{
		success = listen_for_connection();
	}

	emit sig_can_listen(success);
}


// this is a final stop. Class is destroyed afterwards
void StreamServer::stop(){

	server_close();
	disconnect_all();

	if(_server){
		delete _server;
	}
	_server = nullptr;
}


void StreamServer::_sl_mp3_enc_found(){

	_mp3_enc_available = _settings->get(SetNoDB::MP3enc_found);

	if(!_mp3_enc_available){
		stop();
	}
}


void StreamServer::_sl_port_changed(){

	stop();

	create_server();

	emit sig_can_listen(true);
}


void StreamServer::_sl_active_changed(){

	bool active = _settings->get(Set::Broadcast_Active);

	if(!_mp3_enc_available){
		_server->close();
		return;
	}

	if(!active) {
		_server->close();
		return;
	}

	create_server();
	active = _server->isListening();

	if( !active){
		active = listen_for_connection();
		emit sig_can_listen(active);
	}

	if(active){
		this->start();
	}
}
