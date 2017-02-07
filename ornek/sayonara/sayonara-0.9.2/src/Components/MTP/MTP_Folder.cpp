
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


#include "MTP_Folder.h"
#include "MTP_Track.h"
#include "Helper/Settings/Settings.h"

#include <QDir>



MTP_Folder::MTP_Folder(MTPIntern_Device* device, MTPIntern_Folder *folder){

	_id = 0;
	_folder = folder;
	_device = device;
	_parent_id = 0;
	_has_next = true;

	if(folder){
		_id = folder->folder_id;
		_name = folder->name;
		_parent_id = folder->parent_id;
		_storage_id = folder->storage_id;
	}
}

MTP_Folder::~MTP_Folder(){
}



QList<MTP_FolderPtr> MTP_Folder::children() const
{
	MTPIntern_Folder* child;
	QList<MTP_FolderPtr> children;

	if(!_folder){
		return children;
	}

	child = _folder->child;

	while(child){
		children << MTP_FolderPtr( new MTP_Folder(_device, child) );
		child = child->sibling;
	}

	return children;
}


QList<MTP_FolderPtr> MTP_Folder::siblings() const
{

	MTPIntern_Folder* sibling;
	QList<MTP_FolderPtr> siblings;

	if(!_folder){
		return siblings;
	}

	sibling = _folder->sibling;

	while(sibling){
		siblings << MTP_FolderPtr( new MTP_Folder(_device, sibling) );
		sibling = sibling->sibling;
	}

	return siblings;
}


bool MTP_Folder::send_track(MTP_TrackPtr track) const
{

	QString lib_path = Settings::getInstance()->get(Set::Lib_Path);
	QString track_path = track->src_filename();
	sp_log(Log::Debug) << "Track path = " << track_path;

	int ret;

	if(!_device || _id == 0 || _storage_id == 0){
		return false;
	}

	track->set_folder_id(_id);
	track->set_storage_id(_storage_id);

	track_path.remove(lib_path);

	MTP_FolderPtr ptr( new MTP_Folder(_device, _folder) );

	while(true){
		int idx = track_path.indexOf(QDir::separator());
		if(idx == -1){
			break;
		}

		QString subpath = track_path.left(idx);
		if(subpath.isEmpty()){
			track_path.remove(0, idx + 1);
			continue;
		}

		if(track_path.contains(this->name())){
			track_path.remove(0, idx + 1);
			continue;
		}

		bool exists = false;

		// check if folder exists
		QList<MTP_FolderPtr> children = ptr->children();
		for(MTP_FolderPtr child : children){
			if(child->name().compare(subpath) == 0){
				track->set_folder_id(child->id());
				ptr = child;
				sp_log(Log::Debug) << "Change into " << ptr->name();
				track_path.remove(0, idx + 1);
				exists = true;
				break;
			}
		}
		if(exists){
			continue;
		}

		int id = ptr->create_folder(subpath);
		if(id != 0){

			sp_log(Log::Debug) << "Will copy to " << subpath;

			MTPIntern_Folder* f = LIBMTP_new_folder_t();
			f->sibling = ptr->_folder->child;
			ptr->_folder->child = f;
			f->parent_id = ptr->id();
			f->child = NULL;
			f->storage_id = ptr->storage_id();
			f->name = strndup(subpath.toUtf8().data(), subpath.size());
			f->folder_id = id;

			sp_log(Log::Debug) << "Subpath = " << subpath << ": f->name = " << f->name;

			children = ptr->children();
			for(MTP_FolderPtr child : children){
				sp_log(Log::Debug) << "Created " << subpath << " child = " << child->name();
				if(child->name().compare(subpath) == 0){
					track->set_folder_id(id);
					ptr = child;
					sp_log(Log::Debug) << "Change into " << ptr->name();
					break;
				}
			}
		}

		track_path.remove(0, idx + 1);
	}



	ret = LIBMTP_Send_Track_From_File(_device,
							   track->src_filename().toUtf8().data(),
							   track->metadata(),
							   NULL,
							   NULL);

	return (ret == 0);
}

bool MTP_Folder::remove() const
{
	int ret = LIBMTP_Delete_Object(_device, this->id());
	return (ret == 0);
}

quint32 MTP_Folder::create_folder(const QString& name) const
{

	quint32 ret;
	ret = LIBMTP_Create_Folder(_device,
							   strndup(name.toUtf8().data(), name.size()),
							   _id,
							   _storage_id
	);

	return ret;
}

quint32 MTP_Folder::id() const
{
	return _id;
}

QString MTP_Folder::name() const
{
	return _name;
}

quint32 MTP_Folder::parent_id() const
{
	return _parent_id;
}

quint32 MTP_Folder::storage_id() const
{
	return _storage_id;
}




