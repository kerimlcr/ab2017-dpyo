
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




#ifndef MTP_TYPEDEFS_H
#define MTP_TYPEDEFS_H

#include <memory>

#include <libmtp.h>
#include <QString>
#include <QList>
#include "Helper/Logger/Logger.h"


class MTP_Device;
class MTP_File;
struct MTP_Folder;
class MTP_RawDevice;
class MTP_Storage;
class MTP_Track;

/**
 * @brief MTPIntern_DeviceEntry
 * @ingroup MTP
 */
typedef LIBMTP_device_entry_t			MTPIntern_DeviceEntry;

/**
 * @brief MTPIntern_ErrorNumber
 * @ingroup MTP
 */
typedef LIBMTP_error_number_t			MTPIntern_ErrorNumber;

/**
 * @brief MTPIntern_Error
 * @ingroup MTP
 */
typedef LIBMTP_error_t					MTPIntern_Error;

/**
 * @brief MTPIntern_Device
 * @ingroup MTP
 */
typedef LIBMTP_mtpdevice_t				MTPIntern_Device;

/**
 * @brief MTP_DevicePtr
 * @ingroup MTP
 */
typedef std::shared_ptr<MTP_Device>		MTP_DevicePtr;

/**
 * @brief MTPIntern_File
 * @ingroup MTP
 */
typedef LIBMTP_file_t					MTPIntern_File;

/**
 * @brief MTP_FilePtr
 * @ingroup MTP
 */
typedef std::shared_ptr<MTP_File>		MTP_FilePtr;

/**
 * @brief MTPIntern_Folder
 * @ingroup MTP
 */
typedef LIBMTP_folder_t					MTPIntern_Folder;

/**
 * @brief MTP_FolderPtr
 * @ingroup MTP
 */
typedef std::shared_ptr<MTP_Folder>		MTP_FolderPtr;

/**
 * @brief MTPIntern_RawDevice
 * @ingroup MTP
 */
typedef LIBMTP_raw_device_t				MTPIntern_RawDevice;

/**
 * @brief MTP_RawDevicePtr
 * @ingroup MTP
 */
typedef std::shared_ptr<MTP_RawDevice>	MTP_RawDevicePtr;

/**
 * @brief MTPIntern_Storage
 * @ingroup MTP
 */
typedef LIBMTP_devicestorage_t			MTPIntern_Storage;

/**
 * @brief MTP_StoragePtr
 * @ingroup MTP
 */
typedef std::shared_ptr<MTP_Storage>	MTP_StoragePtr;

/**
 * @brief MTPIntern_Track
 * @ingroup MTP
 */
typedef LIBMTP_track_t					MTPIntern_Track;

/**
 * @brief MTP_TrackPtr
 * @ingroup MTP
 */
typedef std::shared_ptr<MTP_Track>		MTP_TrackPtr;


#endif // MTP_TYPEDEFS_H
