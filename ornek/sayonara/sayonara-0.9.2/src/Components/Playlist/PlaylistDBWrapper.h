/* PlaylistDBWrapper.h */

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



#ifndef PLAYLISTDB_WRAPPER_H
#define PLAYLISTDB_WRAPPER_H

#include "Helper/MetaData/MetaData.h"
#include "Database/DatabasePlaylist.h"

class DatabaseConnector;

/**
 * @brief PlaylistDBWrapper is responsible for fetching playlist data from database,
 * especially the handling between skeleton and the playlist itself
 * Most of the functions are wrappers for the DatabasePlaylist class
 * @ingroup Playlists
 */
class PlaylistDBWrapper
{

	SINGLETON(PlaylistDBWrapper)

private:
	DatabaseConnector* _db=nullptr;

	void apply_tags(MetaDataList& v_md);
	bool get_playlists(CustomPlaylists& playlists,
					   DatabasePlaylist::PlaylistChooserType type,
					   SortOrderPlaylists sortorder);

	bool extract_stream(CustomPlaylist& pl, QString name, QString url);


public:



	bool get_skeletons(CustomPlaylistSkeletons& skeletons,
						   DatabasePlaylist::PlaylistChooserType type,
						   SortOrderPlaylists so=SortOrderPlaylists::IDAsc);

	bool get_all_skeletons(CustomPlaylistSkeletons& skeletons,
						   SortOrderPlaylists so=SortOrderPlaylists::IDAsc);

	bool get_temporary_skeletons(CustomPlaylistSkeletons& skeletons,
						   SortOrderPlaylists so=SortOrderPlaylists::IDAsc);

	bool get_non_temporary_skeletons(CustomPlaylistSkeletons& skeletons,
						   SortOrderPlaylists so=SortOrderPlaylists::IDAsc);

	bool get_all_playlists(CustomPlaylists& playlists,
						   SortOrderPlaylists so=SortOrderPlaylists::IDAsc);

	bool get_temporary_playlists(CustomPlaylists& playlists,
								 SortOrderPlaylists so=SortOrderPlaylists::IDAsc);

	bool get_non_temporary_playlists(CustomPlaylists& playlists,
									 SortOrderPlaylists so=SortOrderPlaylists::IDAsc);

	CustomPlaylist get_playlist_by_id(int id);
	CustomPlaylist get_playlist_by_name(const QString& name);

	bool rename_playlist(int id, const QString& new_name);
	bool save_playlist_as(const MetaDataList& v_md, const QString& name);
	bool save_playlist_temporary(const MetaDataList& v_md, const QString& name);
	bool save_playlist(const CustomPlaylist& pl);
	bool save_playlist(const MetaDataList& v_md, int id, bool is_temporary);

	bool delete_playlist(int id);
	bool delete_playlist(const QString& name);
	bool exists(const QString& name);
};

#endif // PLAYLISTDBCONNECTOR_H
