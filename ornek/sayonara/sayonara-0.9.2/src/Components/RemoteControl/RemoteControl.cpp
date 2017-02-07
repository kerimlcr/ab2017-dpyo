/* RemoteControl.cpp */

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



#include "RemoteControl.h"
#include "Components/Covers/CoverLocation.h"
#include "Components/Playlist/PlaylistHandler.h"
#include "Components/PlayManager/PlayManager.h"

#include <QBuffer>
#include <QFile>
#include <QImage>

RemoteControl::RemoteControl(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{
	_server = new QTcpServer(this);

	if(_settings->get(Set::Remote_Active)){
		_server->listen(QHostAddress::Any, _settings->get(Set::Remote_Port));
	}

	connect(_server, &QTcpServer::newConnection, this, &RemoteControl::new_connection);

	_initialized = false;
}

RemoteControl::~RemoteControl()
{

}

void RemoteControl::init()
{

	if(_initialized){
		return;
	}

	_play_manager = PlayManager::getInstance();
	_plh = PlaylistHandler::getInstance();

	PlayManager* mgr = _play_manager;

   _fn_call_map["play"] =		[mgr](){mgr->play();};
   _fn_call_map["pause"] =		[mgr](){mgr->pause();};
   _fn_call_map["prev"] =		[mgr](){mgr->previous();};
   _fn_call_map["next"] =		[mgr](){mgr->next();};
   _fn_call_map["playpause"] =	[mgr](){mgr->play_pause();};
   _fn_call_map["stop"] =		[mgr](){mgr->stop();};
   _fn_call_map["volup"] =		[mgr](){mgr->volume_up();};
   _fn_call_map["voldown"] =	[mgr](){mgr->volume_down();};
   _fn_call_map["state"] =		std::bind(&RemoteControl::request_state, this);
   _fn_call_map["pl"] =			std::bind(&RemoteControl::write_playlist, this);
   _fn_call_map["curSong"] =	std::bind(&RemoteControl::write_cur_track, this);
   _fn_call_map["help"] =		std::bind(&RemoteControl::show_api, this);

   _fn_int_call_map["setvol"] =		std::bind(&RemoteControl::set_volume, this, placeholders::_1);
   _fn_int_call_map["seekrel"] =	std::bind(&RemoteControl::seek_rel, this, placeholders::_1);
   _fn_int_call_map["seekrels"] =	std::bind(&RemoteControl::seek_rel_ms, this, placeholders::_1);
   _fn_int_call_map["chtrk"] =		std::bind(&RemoteControl::change_track, this, placeholders::_1);

   REGISTER_LISTENER_NO_CALL(Set::Remote_Active, _sl_active_changed);
   REGISTER_LISTENER_NO_CALL(Set::Remote_Port, _sl_port_changed);
   REGISTER_LISTENER_NO_CALL(Set::Broadcast_Port, _sl_broadcast_changed);
   REGISTER_LISTENER_NO_CALL(Set::Broadcast_Active, _sl_broadcast_changed);

   _initialized = true;
}

bool RemoteControl::is_connected() const
{

	if(!_settings->get(Set::Remote_Active)){
		return false;
	}

	if(!_socket){
		return false;
	}

	if( !_socket->isOpen() ||
		!_socket->isValid() ||
		!_socket->isWritable())
	{
		return false;
	}

	return true;
}

void RemoteControl::new_connection(){

	if(!_initialized){
		init();
	}

	_socket = _server->nextPendingConnection();
	if(!_socket){
		return;
	}

	sp_log(Log::Debug) << "Got new connection";

	connect(_socket, &QTcpSocket::readyRead, this, &RemoteControl::new_request);
	connect(_socket, &QTcpSocket::disconnected, this, &RemoteControl::socket_disconnected);

	PlayManager* mgr = _play_manager;

	connect(mgr, &PlayManager::sig_position_changed_ms, this, &RemoteControl::pos_changed_ms);
	connect(mgr, &PlayManager::sig_track_changed, this, &RemoteControl::track_changed);
	connect(mgr, &PlayManager::sig_volume_changed, this, &RemoteControl::volume_changed);
	connect(mgr, &PlayManager::sig_playstate_changed, this, &RemoteControl::playstate_changed);
	connect(_plh, &PlaylistHandler::sig_playlist_created, this, &RemoteControl::playlist_changed);
}

void RemoteControl::socket_disconnected()
{
	PlayManager* mgr = _play_manager;

	disconnect(mgr, &PlayManager::sig_position_changed_ms, this, &RemoteControl::pos_changed_ms);
	disconnect(mgr, &PlayManager::sig_track_changed, this, &RemoteControl::track_changed);
	disconnect(mgr, &PlayManager::sig_volume_changed, this, &RemoteControl::volume_changed);
	disconnect(mgr, &PlayManager::sig_playstate_changed, this, &RemoteControl::playstate_changed);
	disconnect(_plh, &PlaylistHandler::sig_playlist_created, this, &RemoteControl::playlist_changed);

}

void RemoteControl::pos_changed_ms(const quint64 pos){

	Q_UNUSED(pos)
	write_cur_pos();
}

void RemoteControl::track_changed(const MetaData& md){
	Q_UNUSED(md)
	write_cur_track();
}

void RemoteControl::volume_changed(int vol){
	Q_UNUSED(vol)
	write_volume();
}


void RemoteControl::new_request(){

	QByteArray arr = _socket->readAll();
	arr = arr.left(arr.size() - 1);

	if(_fn_call_map.contains(arr)){
		auto fn = _fn_call_map[arr];
		fn();
		write_volume();
		write_cur_track();
		return;
	}


	int idx = arr.indexOf(' ');
	if(idx == -1){
		return;
	}

	QByteArray cmd = arr.left(idx);
	if(_fn_int_call_map.contains(cmd)){
		int val = extract_parameter_int(arr, cmd.size());
		RemoteFunctionInt fn = _fn_int_call_map[cmd];
		fn(val);
		return;
	}
}


int RemoteControl::extract_parameter_int(const QByteArray& data, int cmd_len){
	return data.right(data.size() - cmd_len - 1).toInt();
}




void RemoteControl::playstate_changed(PlayManager::PlayState playstate)
{
	Q_UNUSED(playstate)
	write_playstate();
}

void RemoteControl::playlist_changed(PlaylistConstPtr pl)
{
	Q_UNUSED(pl)
	write_playlist();
}

void RemoteControl::_sl_active_changed()
{

	bool active = _settings->get(Set::Remote_Active);

	if(!active){
		_socket->disconnectFromHost();
		_server->close();
	}

	else if(_server->isListening()){
		return;
	}

	else{
		_server->listen(QHostAddress::Any, _settings->get(Set::Remote_Port));
	}
}

void RemoteControl::_sl_port_changed(){
	int port = _settings->get(Set::Remote_Port);
	bool active = _settings->get(Set::Remote_Active);

	if(!active){
		return;
	}

	if(port != _socket->localPort()){
		_socket->disconnectFromHost();
		_server->close();
		_server->listen(QHostAddress::Any, port);
	}
}

void RemoteControl::_sl_broadcast_changed()
{
	if(!is_connected()){
		return;
	}

	write_broadcast_info();
}

void RemoteControl::set_volume(int vol){
	_play_manager->set_volume(vol);
}

void RemoteControl::seek_rel(int percent){
	percent = std::min(percent, 100);
	percent = std::max(percent, 0);
	_play_manager->seek_rel( percent / 100.0 );

}

void RemoteControl::seek_rel_ms(int pos_ms){
	_play_manager->seek_rel_ms( pos_ms );
}


void RemoteControl::change_track(int idx){
	_plh->change_track(idx - 1, _plh->get_active_idx());
}



void RemoteControl::write_cur_pos(){
	quint32 pos_sec = _play_manager->get_cur_position_ms() / 1000;
	write("curPos:" + QByteArray::number(pos_sec));
}

void RemoteControl::write_volume(){
	int vol = _play_manager->get_volume();
	write("vol:" + QByteArray::number(vol));
}

void RemoteControl::write_cur_track(){

	PlayManager::PlayState playstate = _play_manager->get_play_state();

	playstate_changed(playstate);

	if(playstate == PlayManager::PlayState::Stopped){
		return;
	}
	
	MetaData md = _play_manager->get_cur_track();

	PlaylistConstPtr pl = _plh->get_playlist_at(_plh->get_active_idx());
	int cur_track_idx = pl->get_cur_track_idx();


	sp_log(Log::Debug) << "Send cur track idx: " << cur_track_idx;

	write("curIdx:" + QString::number(cur_track_idx).toUtf8());
	write("title:" + md.title.toUtf8());
	write("artist:" + md.artist.toUtf8());
	write("album:" + md.album.toUtf8());
	write("totalPos:" + QString::number(md.length_ms / 1000).toUtf8());

	write_cover(md);
}


void RemoteControl::write_cover(){
	MetaData md = _play_manager->get_cur_track();
	write_cover(md);
}


void RemoteControl::write_cover(const MetaData& md){

	CoverLocation cl = CoverLocation::get_cover_location(md);
	QByteArray img_data;
	QStringList cover_paths;
	if(!cl.cover_path().isEmpty() && QFile::exists(cl.cover_path()) ){
		cover_paths << cl.cover_path();
	}

	if(!cl.local_paths().isEmpty() && QFile::exists(cl.local_path(0))){
		cover_paths << cl.local_path(0);
	}

	for(const QString& cover_path : cover_paths){
		QImage img(cover_path);

		if(!img.isNull()){

			QImage img_copy = img.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			img_data = QByteArray((char*) img_copy.bits(), img_copy.byteCount());
			QByteArray data = QByteArray("coverinfo:") +
					QByteArray::number(img_copy.width()) + ':' +
					QByteArray::number(img_copy.height()) + ':' +
					QByteArray::number(img_copy.format());

			write(data);
			write(img_data);
			break;
		}
	}
}



void RemoteControl::write_playstate()
{
	PlayManager::PlayState playstate = _play_manager->get_play_state();
	QByteArray playstate_str = "playstate:";

	switch(playstate){
		case PlayManager::PlayState::Playing:
			playstate_str += "playing";
			break;
		case PlayManager::PlayState::Paused:
			playstate_str += "paused";
			break;
		case PlayManager::PlayState::Stopped:
		default:
			playstate_str += "stopped";
			break;
	}

	write(playstate_str);
}



void RemoteControl::write_playlist(){

	QByteArray data;
	PlaylistConstPtr pl = _plh->get_playlist_at(_plh->get_active_idx());

	if(pl){
		int i=1;
		for(const MetaData& md : pl->get_playlist()){
			data += QByteArray::number(i) + '\t' +
					md.title.toUtf8() + '\t' +
					md.artist.toUtf8() + '\t' +
					md.album.toUtf8() + '\t' +
					QByteArray::number(md.length_ms / 1000) + '\t' +
					'\n';
			i++;
		}

		write("playlist:" + data);
	}
}


void RemoteControl::write_broadcast_info()
{
	QByteArray data;
	data = "broadcast:" +
			QByteArray::number(_settings->get(Set::Broadcast_Active)) + '\t' +
			QByteArray::number(_settings->get(Set::Broadcast_Port));

	sp_log(Log::Debug) << "Write broadcast " << data;

	write(data);
}



void RemoteControl::write(const QByteArray& data){

	if(!_socket){
		return;
	}

	QByteArray margins;
	margins.push_back((char) 0);
	margins.push_back((char) 1);
	margins.push_back((char) 0);
	margins.push_back((char) 1);

	_socket->write(data + margins);
	_socket->flush();
}


void RemoteControl::request_state()
{
	write_cur_pos();
	write_cur_track();
	write_volume();
	write_playstate();
	write_broadcast_info();
}


void RemoteControl::show_api(){

	if(!_socket || !_socket->isOpen()){
		return;
	}

	_socket->write("\n");

	for(const QByteArray& key : _fn_call_map.keys()){
		_socket->write(key + "\n");

	}

	_socket->write("\n");

	for(const QByteArray& key : _fn_int_call_map.keys()){
		_socket->write(key + "( value )\n");
	}

	_socket->write("\n");
}
