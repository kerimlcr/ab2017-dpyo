/* PlaylistDBWrapper.cpp */

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
#include "Helper/Tagging/Tagging.h"

#include "Helper/FileHelper.h"
#include "Helper/Parser/PlaylistParser.h"
#include "Database/DatabaseConnector.h"

#include <utility>

PlaylistDBWrapper::PlaylistDBWrapper()
{
	_db = DatabaseConnector::getInstance();
}


PlaylistDBWrapper::~PlaylistDBWrapper()
{}


void PlaylistDBWrapper::apply_tags(MetaDataList& v_md)
{
	for(MetaData& md : v_md){
		if(md.is_extern){

			if(Helper::File::is_file(md.filepath())){
				Tagging::getMetaDataOfFile(md);
			}
		}
	}
}


bool PlaylistDBWrapper::get_skeletons(CustomPlaylistSkeletons& skeletons,
                                      DatabasePlaylist::PlaylistChooserType type,
                                      SortOrderPlaylists so)
{
	return _db->getAllPlaylistSkeletons(skeletons, type, so);
}

bool PlaylistDBWrapper::get_all_skeletons(CustomPlaylistSkeletons& skeletons,
					   SortOrderPlaylists so)
{
	return get_skeletons(skeletons,
	                     DatabaseConnector::PlaylistChooserType::TemporaryAndPermanent,
                             so);
}

bool PlaylistDBWrapper::get_temporary_skeletons(CustomPlaylistSkeletons& skeletons,
					   SortOrderPlaylists so)
{
	return get_skeletons(skeletons,
	                     DatabaseConnector::PlaylistChooserType::OnlyTemporary,
	                     so);
}

bool PlaylistDBWrapper:: get_non_temporary_skeletons(CustomPlaylistSkeletons& skeletons,
					   SortOrderPlaylists so)
{
	return get_skeletons(skeletons,
	                     DatabaseConnector::PlaylistChooserType::OnlyPermanent,
	                     so);
}


bool PlaylistDBWrapper::extract_stream(CustomPlaylist& pl, QString name, QString url){

	pl.is_temporary = false;

	if(Helper::File::is_playlistfile(url)) 
	{
		if(PlaylistParser::parse_playlist(url, pl.tracks) > 0) 
		{
			for(MetaData& md : pl.tracks) {

				md.album = name;
				if(md.title.isEmpty()){
					md.title = name;
				}

				if(md.artist.isEmpty()){
					md.artist = url;
				}

				md.is_extern = true;
				md.id = -1;
			}
		}
	}

	// real stream
	else
	{
		MetaData md;

		md.title = name;
		md.album = name;
		md.artist = url;
		md.set_filepath(url);

		md.is_extern = true;
		md.id = -1;

		pl.tracks << std::move(md);
	}

	pl.is_valid = (pl.tracks.size() > 0);

	return pl.is_valid;
}


bool PlaylistDBWrapper::get_playlists(CustomPlaylists& playlists,
                                      DatabasePlaylist::PlaylistChooserType type,
                                      SortOrderPlaylists so)
{
	Q_UNUSED(type)

	bool success;
	CustomPlaylistSkeletons skeletons;

	success = get_all_skeletons(skeletons, so);
	if(!success){
		return false;
	}

	bool load_temporary = (type == DatabasePlaylist::PlaylistChooserType::OnlyTemporary ||
	                       type == DatabasePlaylist::PlaylistChooserType::TemporaryAndPermanent);

	bool load_permanent = (type == DatabasePlaylist::PlaylistChooserType::OnlyPermanent ||
	                       type == DatabasePlaylist::PlaylistChooserType::TemporaryAndPermanent);

	for(const CustomPlaylistSkeleton& skeleton : skeletons){

		CustomPlaylist pl(skeleton);
		if(pl.id < 0){
			continue;
		}

		success = _db->getPlaylistById(pl);

		if(!success){
			continue;
		}

		apply_tags(pl.tracks);

		if( (pl.is_temporary && load_temporary) ||
		    (!pl.is_temporary && load_permanent) )
		{
			playlists.push_back(pl);
		}
	}

	return true;
}


bool PlaylistDBWrapper::get_all_playlists(CustomPlaylists& playlists, SortOrderPlaylists so){
	return get_playlists(playlists,
	                     DatabaseConnector::PlaylistChooserType::TemporaryAndPermanent,
	                     so);
}


bool PlaylistDBWrapper::get_temporary_playlists(CustomPlaylists& playlists, SortOrderPlaylists so){
	return get_playlists(playlists,
						 DatabaseConnector::PlaylistChooserType::OnlyTemporary,
						 so);
}


bool PlaylistDBWrapper::get_non_temporary_playlists(CustomPlaylists& playlists, SortOrderPlaylists so){
	return get_playlists(playlists,
	                     DatabaseConnector::PlaylistChooserType::OnlyPermanent,
	                     so);
}


CustomPlaylist PlaylistDBWrapper::get_playlist_by_id(int id){

	bool success;
	CustomPlaylist pl;
	pl.id = id;

	success = _db->getPlaylistById(pl);
	pl.is_valid = success;

	if(!success){
		return pl;
	}

	return pl;
}


CustomPlaylist PlaylistDBWrapper::get_playlist_by_name(const QString& name){


	int id = _db->getPlaylistIdByName(name);

	if(id < 0){
		CustomPlaylist pl;
		pl.id = -1;
		pl.is_valid = false;
		return pl;
	}

	return get_playlist_by_id(id);
}

bool PlaylistDBWrapper::rename_playlist(int id, const QString& new_name){
	return _db->renamePlaylist(id, new_name);
}


bool PlaylistDBWrapper::save_playlist_as(const MetaDataList& v_md, const QString& name){
	bool success;

	_db->transaction();
	success = _db->storePlaylist(v_md, name, false);
	_db->commit();

	return success;
}

bool PlaylistDBWrapper::save_playlist_temporary(const MetaDataList& v_md, const QString& name){
	bool success;

	_db->transaction();
	success = _db->storePlaylist(v_md, name, true);
	_db->commit();

	return success;
}


bool PlaylistDBWrapper::save_playlist(const CustomPlaylist& pl){
	bool success;

	_db->transaction();
	success = _db->storePlaylist(pl.tracks, pl.id, pl.is_temporary);
	_db->commit();

	return success;
}


bool PlaylistDBWrapper::save_playlist(const MetaDataList& v_md, int id, bool is_temporary){
	bool success;

	_db->transaction();
	success = _db->storePlaylist(v_md, id, is_temporary);
	_db->commit();

	return success;
}


bool PlaylistDBWrapper::delete_playlist(int id){
	return _db->deletePlaylist(id);
}


bool PlaylistDBWrapper::delete_playlist(const QString& name){
	int id = _db->getPlaylistIdByName(name);
	return _db->deletePlaylist(id);
}


bool PlaylistDBWrapper::exists(const QString& name){
	int id = _db->getPlaylistIdByName(name);
	return (id >= 0);
}
