
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


#include "MTP_Storage.h"
#include "MTP_File.h"
#include "MTP_Folder.h"

#include <algorithm>

MTP_Storage::MTP_Storage(MTPIntern_Device* device, MTPIntern_Storage* storage){

	_device = device;
	_storage = storage;
	_id = 0;

	if(storage){
		_id = storage->id;
		_free_space = storage->FreeSpaceInBytes;
		_max_space = storage->MaxCapacity;
		_name = QString(storage->StorageDescription);
		_identifier = QString(storage->VolumeIdentifier);

		MTPIntern_File* raw_file = LIBMTP_Get_Filelisting_With_Callback(_device, NULL, NULL);
		while(raw_file){
			if(raw_file->storage_id != _storage->id){
				raw_file = raw_file->next;
				continue;
			}

			MTP_FilePtr file = MTP_FilePtr( new MTP_File(_device, raw_file) );
			QList<MTP_FilePtr> files = _files[raw_file->parent_id];

			files << file;
			_files[raw_file->parent_id] = files;


			raw_file = raw_file->next;
		}
	}
}

MTP_Storage::~MTP_Storage(){

}

void MTP_Storage::remove_id(quint32 id){
	if(_files.contains(id)){
		_files.remove(id);
	}

	for(quint32 key : _files.keys()){
		QList<MTP_FilePtr> file_list = _files[key];

		auto lambda = [&id](MTP_FilePtr ptr){
			return (ptr->id() == id);
		};

		auto it = std::find_if(file_list.begin(), file_list.end(), lambda);
		if(it != file_list.end()){
			file_list.erase(it);
		}

		_files[key] = file_list;
	}
}


QList<MTP_FolderPtr> MTP_Storage::folders()
{
	_folders.clear();

	MTPIntern_Folder* intern_folder;

	if(!_storage || !_device){
		return _folders;
	}

	intern_folder = LIBMTP_Get_Folder_List_For_Storage(_device, _storage->id);

	while(intern_folder){
		_folders << MTP_FolderPtr( new MTP_Folder(_device, intern_folder) );
		intern_folder = intern_folder->sibling;
	}

	return _folders;
}

QList<MTP_FilePtr> MTP_Storage::files_of_folder(quint32 folder_id) const
{
	return _files[folder_id];
}



quint32 MTP_Storage::id() const {
	return _id;
}

quint64 MTP_Storage::free_space() const {
	return _free_space;
}

quint64 MTP_Storage::max_space() const {
	return _max_space;
}

QString MTP_Storage::name() const {
	return _name;
}

QString MTP_Storage::identifier() const {
	return _identifier;
}

