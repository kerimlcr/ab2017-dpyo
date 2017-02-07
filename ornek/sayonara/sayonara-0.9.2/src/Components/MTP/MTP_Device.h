
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




#ifndef MTP_DEVICE_H
#define MTP_DEVICE_H

#include "MTP_Typedefs.h"

/**
 * @brief The MTP_Device class
 * @ingroup MTP
 */
class MTP_Device {

public:
	MTP_Device(MTPIntern_Device* device);
	virtual ~MTP_Device();

	quint32 id() const;
	QString default_album_dir() const;
	QString default_plaliyst_dir() const;
	QString default_music_dir() const;
	QString friendly_name() const;
	QList<MTP_StoragePtr> storages();

	static QList<MTP_DevicePtr> get_all_connected_devices();


private:
	MTPIntern_Device*		_device=nullptr;
	QList<MTP_StoragePtr>	_storages;

	bool					_valid;
	quint32					_id;
	QString					_default_album_dir;
	QString					_default_playlist_dir;
	QString					_default_music_dir;
	QString					_friendly_name;
};


#endif // MTP_DEVICE_H
