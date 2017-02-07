
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




#ifndef MTP_STORAGE_H
#define MTP_STORAGE_H

#include "MTP_Typedefs.h"
#include <QHash>

/**
 * @brief The MTP_Storage class
 * @ingroup MTP
 */
class MTP_Storage
{


public:

	MTP_Storage(MTPIntern_Device* device=nullptr, MTPIntern_Storage* storage=nullptr);
	virtual ~MTP_Storage();

	quint32 id() const;
	quint64 free_space() const;
	quint64 max_space() const;
	QString name() const;
	QString identifier() const;

	QList<MTP_FolderPtr> folders();
	QList<MTP_FilePtr> files_of_folder(quint32 folder_id) const;

	void remove_id(quint32 id);


private:
	quint32					_id;
	quint64					_free_space;
	quint64					_max_space;
	QString					_name;
	QString					_identifier;

	MTPIntern_Storage*		_storage=nullptr;
	MTPIntern_Device*		_device=nullptr;
	QList<MTP_FolderPtr>	_folders;

	QHash<quint32, QList<MTP_FilePtr>> _files;

};





#endif // MTP_STORAGE_H
