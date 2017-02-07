
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



#include "MTP_RawDevice.h"
#include "MTP_Device.h"


MTP_RawDevice::MTP_RawDevice(MTPIntern_RawDevice* raw_device) :
	_raw_device(raw_device),
	_open_device( MTP_DevicePtr( new MTP_Device(nullptr)) )

{
}

MTP_RawDevice::~MTP_RawDevice(){
	sp_log(Log::Warning) << "Raw device deleted";
}

MTP_DevicePtr MTP_RawDevice::open()
{
	if(!_raw_device){
		return MTP_DevicePtr(nullptr);
	}

	MTPIntern_Device* intern_device;

	QString device_str;

	intern_device = LIBMTP_Open_Raw_Device(_raw_device);
	if(!intern_device){
		sp_log(Log::Warning) << "Cannot open device " << device_str;
		return MTP_DevicePtr(nullptr);
	}

	LIBMTP_Set_Syncpartner(intern_device, "Sayonara");

	sp_log(Log::Info) << "Successfully opened device " << this->get_device_string();
	_open_device = MTP_DevicePtr( new MTP_Device(intern_device) );

	return _open_device;
}


QString MTP_RawDevice::get_device_string() const
{
	if(!_raw_device){
		return "No device detected";
	}

	QString str;
	MTPIntern_DeviceEntry entry = _raw_device->device_entry;
	if(entry.product){
		str += QString(entry.product) + " by ";
	}

	if(entry.vendor){
		str += entry.vendor;
	}

	if(str.isEmpty()){
		str = "Unknown Device";
	}

	return str;
}


