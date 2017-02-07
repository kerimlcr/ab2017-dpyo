/* PlaylistLoader.h */

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



#ifndef PLAYLISTLOADER_H
#define PLAYLISTLOADER_H

#include "Helper/MetaData/MetaData.h"
#include "Helper/Settings/SayonaraClass.h"

#include "Playlist.h"

class CustomPlaylist;
class PlaylistDBWrapper;
class PlaylistHandler;

/**
 * @brief The PlaylistLoader class
 * @ingroup Playlists
 */
class PlaylistLoader : public QObject, protected SayonaraClass
{
	Q_OBJECT

private:

	CustomPlaylists	_playlists;

	PlaylistDBWrapper*	_playlist_db_connector=nullptr;

	int						_last_playlist_idx;
	int						_last_track_idx;


public:
	explicit PlaylistLoader(QObject* parent=nullptr);


	CustomPlaylists	get_playlists() const;
	int						get_playlist_count() const;

	int						get_last_playlist_idx() const;
	bool					was_last_playlist_found() const;

	int						get_last_track_idx() const;
	bool					was_last_track_found() const;


	int						create_playlists();

};

#endif // PLAYLISTLOADER_H
