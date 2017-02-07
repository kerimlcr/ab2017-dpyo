
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




#include "MTP_Device.h"
#include "MTP_Storage.h"


MTP_Device::MTP_Device(MTPIntern_Device* device){

	_device = device;
	_valid = true;

	if(!device){
		_id = 0;
		_valid = false;
	}

	else{

		LIBMTP_Get_Storage(device, LIBMTP_STORAGE_SORTBY_NOTSORTED);
		char* friendly = LIBMTP_Get_Friendlyname(device);
		if(friendly){
			_friendly_name = QString(friendly);
		}

		if(!_device->storage){
			sp_log(Log::Warning) << "Device has no storages!";
		}

		_default_album_dir = QString(device->default_album_folder);
		_default_playlist_dir = QString(device->default_playlist_folder);
		_default_music_dir = QString(device->default_music_folder);
	}
}



MTP_Device::~MTP_Device(){
	if(_device){
		sp_log(Log::Debug) << "Closed device " << _friendly_name;
		LIBMTP_Release_Device(_device);
	}
}

QList<MTP_DevicePtr> MTP_Device::get_all_connected_devices(){

	QList<MTP_DevicePtr> ret;

	MTPIntern_Device* devices;
	MTPIntern_Device* device;
	MTPIntern_ErrorNumber error_number;


	error_number = LIBMTP_Get_Connected_Devices(&devices);
	sp_log(Log::Debug) << "connected devices: " << error_number;

	device = devices;

	while(device){
		ret << MTP_DevicePtr(new MTP_Device(device));
		device = device->next;
	}

	return ret;
}


QList<MTP_StoragePtr> MTP_Device::storages()
{
	_storages.clear();

	MTPIntern_Storage* intern_storage;

	if(!_device){
		sp_log(Log::Warning) << "No device given -> zero storages";
		return _storages;
	}

	intern_storage = _device->storage;

	while(intern_storage){

		MTP_StoragePtr storage = MTP_StoragePtr( new MTP_Storage(_device, intern_storage) );
		_storages << storage;

		intern_storage = intern_storage->next;
	}

	return _storages;
}

quint32 MTP_Device::id() const
{
	return _id;
}

QString MTP_Device::friendly_name() const 
{
	return _friendly_name;
}

QString MTP_Device::default_album_dir() const
{
	return _default_album_dir;
}

QString MTP_Device::default_plaliyst_dir() const
{
	return _default_playlist_dir;
}

QString MTP_Device::default_music_dir() const
{
	return _default_music_dir;
}


