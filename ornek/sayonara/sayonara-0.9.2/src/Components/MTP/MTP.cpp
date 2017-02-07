/* MTP.cpp */

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



#include "MTP.h"

MTP::MTP(QObject* parent) :
	QThread(parent)
{
	_mtp_initialized = false;
	LIBMTP_Init();
}

MTP::~MTP()
{
	_raw_devices.clear();
}


void MTP::run_devices(){
	_devices = MTP_Device::get_all_connected_devices();
}

void MTP::run_raw_devices(){

	MTPIntern_RawDevice* raw_device_ptr;
	int n_devices;

	MTPIntern_ErrorNumber error = LIBMTP_Detect_Raw_Devices(&raw_device_ptr, &n_devices);

	if(error != LIBMTP_ERROR_NONE){
		return;
	}


	sp_log(Log::Debug) << "Scan  for devices: " << n_devices;
	for(int i=0; i<n_devices; i++){

		MTPIntern_RawDevice* raw_device = (raw_device_ptr + i);
		if(!raw_device){
			continue;
		}

		sp_log(Log::Debug) << "Add device " << i;
		_raw_devices << MTP_RawDevicePtr( new MTP_RawDevice(raw_device) );
	}
}

void MTP::run(){

	_raw_devices.clear();
	_devices.clear();

	run_raw_devices();
}

QList<MTP_RawDevicePtr> MTP::get_raw_devices(){
	return _raw_devices;
}

QList<MTP_DevicePtr> MTP::get_devices(){
	return _devices;
}


