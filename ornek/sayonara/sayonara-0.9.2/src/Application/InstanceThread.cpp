
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



#include "InstanceThread.h"
#include "Helper/Helper.h"
#include "Components/Playlist/PlaylistHandler.h"

InstanceThread::InstanceThread(InstanceMessage* instance_message, QObject* parent) :
	QThread(parent),
	_memory("SayonaraMemory")
{
	_may_run = true;
	_instance_message = instance_message;
}

InstanceThread::~InstanceThread()
{

}

void InstanceThread::run()
{
	_may_run = true;

	while(_may_run) {

		if((*_instance_message) != InstanceMessageNone){

			sp_log(Log::Debug) << "Kill signal received";

			emit sig_player_raise();

			if( (*_instance_message) == InstanceMessageWithFiles){
				parse_memory();
			}

			(*_instance_message) = InstanceMessageNone;
		}

		if(_may_run){
			Helper::sleep_ms(100);
		}
	}
}

void InstanceThread::stop() {
	_may_run = false;
}

void InstanceThread::parse_memory()
{
	if(_memory.isAttached()){
		sp_log(Log::Debug) << "memory already attached";
	}

	else if(!_memory.attach()){
		sp_log(Log::Debug) << "Cannot attach shared memory " << _memory.errorString();
		return;
	}

	_memory.lock();

	const char* data = static_cast<const char*>(_memory.constData());

	if(data){

		QByteArray array(data, _memory.size());
		QList<QByteArray> strings = array.split('\n');
		QStringList file_list;

		for(const QByteArray& arr : strings){

			QString filename = QString::fromUtf8(arr);
			sp_log(Log::Debug) << "Add file " << filename;

			if(!filename.isEmpty()){
				file_list << filename;
			}
		}

		QString new_name =  PlaylistHandler::getInstance()->request_new_playlist_name();
		emit sig_create_playlist(file_list, new_name, true);
	}

	_memory.unlock();
	_memory.detach();

}
