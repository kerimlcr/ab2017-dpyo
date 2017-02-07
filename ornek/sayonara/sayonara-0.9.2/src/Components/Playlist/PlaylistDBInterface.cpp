/* PlaylistDBInterface.cpp */

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


#include "PlaylistDBWrapper.h"
#include "PlaylistDBInterface.h"


PlaylistDBInterface::PlaylistDBInterface(const QString& name)
{
	_playlist_db_connector = PlaylistDBWrapper::getInstance();

	_name = name;
	_id = _playlist_db_connector->get_playlist_by_name(name).id;

	_is_temporary = true;
	_save_enabled = true;
}

int PlaylistDBInterface::get_id() const {
	return _id;
}

void PlaylistDBInterface::set_id(int id){
	_id = id;
}


QString PlaylistDBInterface::get_name() const {
	return _name;
}

void PlaylistDBInterface::set_name(const QString& name){
	_name = name;
}

bool PlaylistDBInterface::is_temporary() const{
	return _is_temporary;
}

void PlaylistDBInterface::set_temporary(bool b){
	_is_temporary = b;
}


void PlaylistDBInterface::set_skeleton(const CustomPlaylistSkeleton& skeleton){
	_name = skeleton.name;
	_id = skeleton.id;
	_is_temporary = skeleton.is_temporary;
}



PlaylistDBInterface::SaveAsAnswer PlaylistDBInterface::save(){

	if(!is_storable()){
		return SaveAsAnswer::ExternTracksError;
	}

	const MetaDataList& v_md = this->get_playlist();

	PlaylistDBInterface::SaveAsAnswer answer;

	if(_id >= 0){

		bool success;

		answer = SaveAsAnswer::Error;
		success = _playlist_db_connector->save_playlist(v_md, _id, _is_temporary);

		if(success){
			answer = SaveAsAnswer::Success;
			this->set_changed(false);
		}
	}

	else {
		answer = save_as(_name, true);
	}

	return answer;
}


bool PlaylistDBInterface::insert_temporary_into_db(){

	if(!_is_temporary) {
		return false;
	}

	if(!is_storable()){
		return false;
	}


	const MetaDataList& v_md = get_playlist();

	bool success = _playlist_db_connector->save_playlist_temporary(v_md, _name);

	if(!success){
		return false;
	}

	_id = _playlist_db_connector->get_playlist_by_name(_name).id;

	return true;
}


PlaylistDBInterface::SaveAsAnswer PlaylistDBInterface::save_as(const QString& name, bool force_override){

	if(!is_storable()){
		return SaveAsAnswer::ExternTracksError;
	}

	const MetaDataList& v_md = this->get_playlist();

	int tgt_id = -1;
	bool success;
	bool was_temporary;
	int old_id;

	CustomPlaylistSkeletons skeletons;

	_playlist_db_connector->get_all_skeletons(skeletons);

	// check if name already exists
	for(const CustomPlaylistSkeleton& skeleton : skeletons){

		QString tmp_name = skeleton.name;

		if( tmp_name.compare(name, Qt::CaseInsensitive) == 0 ){

			tgt_id = skeleton.id;

			if(!force_override){
				return SaveAsAnswer::AlreadyThere;
			}

			else {
				break;
			}
		}
	}

	old_id = this->get_id();
	was_temporary = this->is_temporary();

	// Name already exists, override
	if(tgt_id >= 0){
		success = _playlist_db_connector->save_playlist(v_md, tgt_id, _is_temporary);
	}

	// New playlist
	else{

		success = _playlist_db_connector->save_playlist_as( v_md, name);

		if(success && was_temporary){
			_playlist_db_connector->delete_playlist(old_id);
		}
	}

	if(success){

		int id = _playlist_db_connector->get_playlist_by_name(name).id;
		if(id >= 0){
			this->set_id(id);
		}

		this->set_temporary(false);
		this->set_name(name);
		this->set_changed(false);

		return SaveAsAnswer::Success;
	}

	return SaveAsAnswer::Error;
}


PlaylistDBInterface::SaveAsAnswer PlaylistDBInterface::rename(const QString& name){

	bool success;

	if(!is_storable()){
		return SaveAsAnswer::ExternTracksError;;
	}

	CustomPlaylistSkeletons skeletons;
	_playlist_db_connector->get_all_skeletons(skeletons);

	// check if name already exists
	for(const CustomPlaylistSkeleton& skeleton : skeletons){

		QString tmp_name = skeleton.name;

		if( tmp_name.compare(name, Qt::CaseInsensitive) == 0 ){
			return SaveAsAnswer::AlreadyThere;
		}
	}

	success = _playlist_db_connector->rename_playlist(_id, name);

	if(success){
		this->set_name(name);
		return SaveAsAnswer::Success;
	}

	return SaveAsAnswer::Error;
}

bool PlaylistDBInterface::delete_playlist()
{
	return _playlist_db_connector->delete_playlist(_id);
}


bool PlaylistDBInterface::remove_from_db(){

	if(!is_storable()){
		return false;
	}

	bool success;
	if(_id >= 0){
		success = _playlist_db_connector->delete_playlist(_id);
	}

	else{
		success = _playlist_db_connector->delete_playlist(_name);
	}

	_is_temporary = true;
	return success;
}

void PlaylistDBInterface::enable_saving(bool b)
{
	_save_enabled = b;
}

bool PlaylistDBInterface::is_save_enabled() const
{
	return _save_enabled;
}


QString PlaylistDBInterface::request_new_db_name()
{
	CustomPlaylistSkeletons skeletons;
	PlaylistDBWrapper::getInstance()->get_all_skeletons(skeletons);

	QString target_name;

	for(int idx = 1; idx < 1000; idx++){

		bool found = false;
		target_name = tr("New %1").arg(idx);
		for(const CustomPlaylistSkeleton& skeleton : skeletons){

			QString name = skeleton.name;

			if(name.compare(target_name, Qt::CaseInsensitive) == 0){
				found = true;
				break;
			}
		}

		if(!found){
			break;
		}
	}

	return target_name;
}
