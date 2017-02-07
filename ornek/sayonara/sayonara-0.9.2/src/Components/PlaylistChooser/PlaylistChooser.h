/* PlaylistChooser.h */

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



#ifndef PLAYLISTCHOOSER_H_
#define PLAYLISTCHOOSER_H_

#define STATE_WAIT 0
#define STATE_IMPORT_SUCCESS 1
#define STATE_IMPORT_FAIL 2

#include "Helper/MetaData/MetaData.h"
#include "Helper/Playlist/CustomPlaylist.h"

#include <QMap>

class CustomPlaylist;
class CustomPlaylistSkeleton;
class PlaylistDBWrapper;
class PlaylistHandler;

/**
 * @brief The PlaylistChooser class
 * @ingroup Components
 */
class PlaylistChooser : public QObject {
	Q_OBJECT

public:
	PlaylistChooser();
	~PlaylistChooser();

	void delete_playlist(int id);

	void load_single_playlist(int id);

	void save_playlist(int id);
	void save_playlist(QString playlist_name);
	void save_playlist_file(QString filename, bool);
	void clear_playlist();
	void playlist_files_selected(const QStringList& lst);
	int find_playlist(const QString& name) const;

signals:
	void sig_all_playlists_loaded(const CustomPlaylistSkeletons&);


public slots:
	void load_all_playlists();


private:
	CustomPlaylistSkeletons	_skeletons;
	int						_import_state;

	PlaylistHandler*		_playlist_handler=nullptr;
	PlaylistDBWrapper*	_playlist_db_connector=nullptr;
	CustomPlaylist			find_custom_playlist(int id);


};

#endif /* PLAYLISTS_H_ */
