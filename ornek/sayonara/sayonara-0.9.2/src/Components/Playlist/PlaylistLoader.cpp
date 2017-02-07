/* PlaylistLoader.cpp */

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



#include "PlaylistLoader.h"
#include "PlaylistHandler.h"
#include "Helper/MetaData/MetaData.h"
#include "PlaylistDBWrapper.h"
#include "Helper/Playlist/CustomPlaylist.h"

#include <algorithm>

PlaylistLoader::PlaylistLoader(QObject* parent) :
	QObject(parent),
	SayonaraClass()
{

	_playlist_db_connector = PlaylistDBWrapper::getInstance();

	bool success=false;

	bool load_playlists = (_settings->get(Set::PL_LoadSavedPlaylists) || _settings->get(Set::PL_LoadTemporaryPlaylists));
	bool load_temporary_playlists = _settings->get(Set::PL_LoadTemporaryPlaylists);
	bool load_saved_playlists = _settings->get(Set::PL_LoadSavedPlaylists);
	bool load_last_track = _settings->get(Set::PL_LoadLastTrack);
	int saved_playlist_id = _settings->get(Set::PL_LastPlaylist);
	int saved_track_idx = _settings->get(Set::PL_LastTrack);

	_last_playlist_idx = -1;
	_last_track_idx = -1;


	// we don't load any playlists
	if(!load_playlists){

		CustomPlaylists playlists;
		success = _playlist_db_connector->get_temporary_playlists(playlists);

		if(success){
			for(const CustomPlaylist& pl : playlists){
				_playlist_db_connector->delete_playlist(pl.id);
			}
		}

		return;
	}

	bool has_playlist_id=false;
	if(load_temporary_playlists && !load_saved_playlists){
		success = _playlist_db_connector->get_temporary_playlists(_playlists);

	}

	else if(load_saved_playlists && !load_temporary_playlists){
		success = _playlist_db_connector->get_non_temporary_playlists(_playlists);
	}

	else if(load_saved_playlists && load_temporary_playlists){
		success = _playlist_db_connector->get_all_playlists(_playlists);
	}

	if(!success){
		return;
	}


	has_playlist_id = std::any_of(_playlists.begin(), _playlists.end(), [&saved_playlist_id](const CustomPlaylist& pl){
		return (saved_playlist_id == pl.id);
	});

	if(!has_playlist_id){
		_playlists.prepend(_playlist_db_connector->get_playlist_by_id(saved_playlist_id));
	}


	for(int i=0; i<_playlists.size(); i++){

		// we load all temporary playlist
		bool add_playlist = false;
		CustomPlaylist pl = _playlists[i];

		if(pl.name.trimmed().isEmpty() ||
		   pl.n_tracks == 0)
		{
			_playlist_db_connector->delete_playlist(pl.id);
			_playlists.removeAt(i);
			i--;
			continue;
		}


		// playlist maybe permanent or temporary
		// but this was the last one

		if(pl.id == saved_playlist_id){

			if( between(saved_track_idx, pl.tracks) )
			{
				if(load_last_track){
					_last_track_idx = saved_track_idx;
					_last_playlist_idx = i;

					add_playlist = true;
				}
			}
		}


		if(pl.is_temporary){
			if(load_temporary_playlists){
				add_playlist = true;
			}

			else if(!add_playlist){
				_playlist_db_connector->delete_playlist(pl.id);
			}
		}

		else{
			if(load_saved_playlists){
				add_playlist = true;
			}
		}

		if(!add_playlist){
			_playlists.removeAt(i);
			i--;
		}
	}
}


CustomPlaylists PlaylistLoader::get_playlists() const
{
	return _playlists;
}

int	PlaylistLoader::get_playlist_count() const
{
	return _playlists.size();
}

int	PlaylistLoader::get_last_playlist_idx() const
{
	if( !between(_last_playlist_idx, _playlists))
	{
		return -1;
	}

	return _last_playlist_idx;
}

int	PlaylistLoader::get_last_track_idx() const
{

	int n_tracks;
	if(	_last_playlist_idx < 0 ||
		_last_playlist_idx >= _playlists.size())
	{
		return -1;
	}

	n_tracks = _playlists[_last_playlist_idx].tracks.size();
	if( _last_track_idx < 0 ||
		_last_track_idx >= n_tracks)
	{
		 return -1;
	}

	return _last_track_idx;
}

bool PlaylistLoader::was_last_playlist_found() const
{
	return (get_last_playlist_idx() != -1);
}

bool PlaylistLoader::was_last_track_found() const
{
	return (get_last_track_idx() != -1);
}


int PlaylistLoader::create_playlists(){

	PlaylistHandler* plh = PlaylistHandler::getInstance();
	int idx;

	// no playlists found
	if( _playlists.isEmpty() ){
		QString name = plh->request_new_playlist_name();
		idx = plh->create_empty_playlist(name);
		plh->set_current_idx(idx);
	}

	else {

		// add playlists
		for(const CustomPlaylist& pl : _playlists){
			plh->create_playlist(pl);
		}
	}

	return _playlists.size();

}
