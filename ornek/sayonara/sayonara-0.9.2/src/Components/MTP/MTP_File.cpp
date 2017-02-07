
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


#include "MTP_File.h"


MTP_File::MTP_File(MTPIntern_Device* device, MTPIntern_File* file){

	_id = 0;
	_file = file;
	_device = device;

	if(_file){
		_id = _file->item_id;
		_filename = QString(file->filename);
	}
}

MTP_File::~MTP_File(){
	/*if(_file){
		LIBMTP_destroy_file_t(_file);
	}*/
}

bool MTP_File::remove() const
{
	quint32 ret = LIBMTP_Delete_Object(_device, _id);
	return (ret == 0);
}


quint32 MTP_File::id() const
{
	return _id;
}

QString MTP_File::filename() const
{
	return _filename;
}

bool MTP_File::operator==(const MTP_File& other) const
{
	return (_filename == other.filename());
}


