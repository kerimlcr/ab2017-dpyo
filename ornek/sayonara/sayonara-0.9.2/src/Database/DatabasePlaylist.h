/* DatabasePlaylist.h */

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



#ifndef DATABASEPLAYLIST_H
#define DATABASEPLAYLIST_H

#include "Components/Library/Sorting.h"
#include "Database/DatabaseModule.h"
#include "Helper/Playlist/CustomPlaylist.h"

class DatabasePlaylist : private DatabaseModule
{

public:

	enum class PlaylistChooserType : quint8 {
		OnlyTemporary=1,
		OnlyPermanent=2,
		TemporaryAndPermanent=3
	};


	DatabasePlaylist(QSqlDatabase db, quint8 db_id);

	bool getAllPlaylistSkeletons(CustomPlaylistSkeletons& skeletons, DatabasePlaylist::PlaylistChooserType type, SortOrderPlaylists sortorder=SortOrderPlaylists::NameAsc);
	bool getPlaylistSkeletonById(CustomPlaylistSkeleton& skeleton);

	int createPlaylist(QString playlist_name, bool temporary);
	bool deleteTrackFromPlaylists(int track_id);
	bool renamePlaylist(int id, const QString& new_name);

	int getPlaylistIdByName(const QString& name);
	bool getPlaylistById(CustomPlaylist& pl);

	// checks if playlist is already there and inserts it, if necessary
	bool storePlaylist(const MetaDataList& vec_md, QString playlist_name, bool temporary);
	bool storePlaylist(const MetaDataList& vec_md, int playlist_id, bool temporary);

	bool updatePlaylistTempState(int playlist_id, bool temporary);
	bool deletePlaylist(int playlist_id);
	bool emptyPlaylist(int playlist_id);

	bool insertTrackIntoPlaylist(const MetaData& md, int playlist_id, int pos);


};

#endif // DATABASEPLAYLIST_H
