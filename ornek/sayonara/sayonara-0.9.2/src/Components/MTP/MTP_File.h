
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




#ifndef MTP_FILE_H
#define MTP_FILE_H

#include "MTP_Typedefs.h"

/**
 * @brief The MTP_File class
 * @ingroup MTP
 */
class MTP_File {

private:
	quint32				_id;
	QString				_filename;

	MTPIntern_File*		_file=nullptr;
	MTPIntern_Device*	_device=nullptr;


public:

	MTP_File(MTPIntern_Device* device, MTPIntern_File* file);
	virtual ~MTP_File();

	quint32 id() const;
	QString filename() const;
	bool remove() const;

	bool operator==(const MTP_File& other) const;

};


#endif // MTP_FILE_H
