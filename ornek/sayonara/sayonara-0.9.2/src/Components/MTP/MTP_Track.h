
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




#ifndef MTP_TRACK_H
#define MTP_TRACK_H

#include "Helper/MetaData/MetaData.h"

#include "MTP_Typedefs.h"

/**
 * @brief The MTP_Track class
 * @ingroup MTP
 */
class MTP_Track {

	friend struct MTP_Folder;

private:
	MTPIntern_Track*	_track=nullptr;
	QString				_src_filename;


	// this is needed to send a track, only available for MTP_Folder
	MTPIntern_Track* metadata() const;

	void set_folder_id(quint32 folder_id);
	void set_storage_id(quint32 storage_id);

	void read_metadata(const MetaData& md);


public:
	MTP_Track(const MetaData& md);
	MTP_Track(const QString& filename);
	virtual ~MTP_Track();

	QString src_filename() const;

	MetaData export_metadata();
};


#endif // MTP_TRACK_H
