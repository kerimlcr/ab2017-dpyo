/* PlaylistChooser.cpp */

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


#include "PlaylistChooser.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/Playlist/CustomPlaylist.h"
#include "Components/Playlist/PlaylistHandler.h"
#include "Components/Playlist/PlaylistDBWrapper.h"

PlaylistChooser::PlaylistChooser() {

	_playlist_handler = PlaylistHandler::getInstance();

	_playlist_db_connector = PlaylistDBWrapper::getInstance();

	connect(_playlist_handler, &PlaylistHandler::sig_saved_playlists_changed,
			this, &PlaylistChooser::load_all_playlists);
}

PlaylistChooser::~PlaylistChooser() {

}

void PlaylistChooser::load_all_playlists() {

	bool success;

	_skeletons.clear();
	success = _playlist_db_connector->get_non_temporary_skeletons(_skeletons, SortOrderPlaylists::NameAsc);

	if(success) {
		emit sig_all_playlists_loaded(_skeletons);
	}
}

CustomPlaylist PlaylistChooser::find_custom_playlist(int id){

	CustomPlaylist pl = _playlist_db_connector->get_playlist_by_id(id);
	return pl;

}

void PlaylistChooser::load_single_playlist(int id) {

	int idx;
	if(id < 0) {
		return;
	}

	CustomPlaylist pl = find_custom_playlist(id);
	if(!pl.is_valid) {
		return;
	}

	idx = _playlist_handler->create_playlist(pl);
	_playlist_handler->set_current_idx(idx);
}



void PlaylistChooser::delete_playlist(int id) {

	bool success = _playlist_db_connector->delete_playlist(id);

	if(!success) {
		sp_log(Log::Warning) << "playlist " << id << " could not be deleted";
	}

	load_all_playlists();
}


void PlaylistChooser::save_playlist(int id)
{
	CustomPlaylist pl = find_custom_playlist(id);
	if(pl.is_valid){
		int cur_idx = _playlist_handler->get_current_idx();
		_playlist_handler->save_playlist(cur_idx);
	}
}

void PlaylistChooser::save_playlist(QString name){
	int cur_idx = _playlist_handler->get_current_idx();
	_playlist_handler->save_playlist_as(cur_idx, name, true);
}

void PlaylistChooser::save_playlist_file(QString filename, bool relative_paths){
	_playlist_handler->save_playlist_to_file(filename, relative_paths);
}


void PlaylistChooser::clear_playlist(){
	int cur_idx = _playlist_handler->get_current_idx();
	_playlist_handler->clear_playlist(cur_idx);
}


void PlaylistChooser::playlist_files_selected(const QStringList& lst){
	_playlist_handler->create_playlist(lst, "", false);
}

int PlaylistChooser::find_playlist(const QString& name) const
{
	for(const CustomPlaylistSkeleton& skeleton : _skeletons){
		if(skeleton.name.compare(name) == 0){
			return skeleton.id;
		}
	}

	return -1;
}

