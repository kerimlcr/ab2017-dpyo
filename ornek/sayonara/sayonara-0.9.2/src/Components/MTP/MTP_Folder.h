
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




#ifndef MTP_FOLDER_H
#define MTP_FOLDER_H

#include "MTP_Typedefs.h"

/**
 * @brief The MTP_Folder struct
 * @ingroup MTP
 */
struct MTP_Folder {

private:
	MTPIntern_Folder*	_folder=nullptr;
	MTPIntern_Device*	_device=nullptr;

	quint32				_id;
	QString				_name;
	quint32				_parent_id;
	quint32				_storage_id;
	bool				_has_next;


public:
	MTP_Folder(MTPIntern_Device* device=nullptr, MTPIntern_Folder* folder=nullptr);
	virtual ~MTP_Folder();

	quint32 id() const;
	QString name() const;
	quint32 parent_id() const;
	quint32 storage_id() const;

	QList<MTP_FolderPtr> children() const;
	QList<MTP_FolderPtr> siblings() const;


	bool send_track(MTP_TrackPtr track) const;
	bool remove() const;
	quint32 create_folder(const QString& name) const;
};






#endif // MTP_FOLDER_H
