/* Playlist.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * Playlist.h
 *
 *  Created on: Apr 6, 2011
 *      Author: Lucio Carreras
 */

#ifndef PLAYLISTHANDLER_H_
#define PLAYLISTHANDLER_H_

#include "Playlist.h"

#include "Helper/globals.h"
#include "Helper/Settings/SayonaraClass.h"

#include "Components/PlayManager/PlayManager.h"

#include <QTimer>
#include <memory>

class DatabaseConnector;
/**
 * @brief Global handler for playlists
 * @ingroup Playlists
 */
class PlaylistHandler : public QObject, protected SayonaraClass {

	Q_OBJECT

	SINGLETON_QOBJECT(PlaylistHandler)

public:

	/**
	 * @brief The PlaylistIndex enum
	 */
	enum class PlaylistIndex : quint8
	{
		Current=0,
		Active
	};


signals:
	/**
	 * @brief emitted when new playlist has been created
	 * @param pl Playlist, usually current one
	 */
	void sig_playlist_created(PlaylistPtr pl);

	/**
	 * @brief emitted when current track index has changed
	 * @param track_idx index in playlist
	 * @param playlist_idx index of playlist
	 */
	void sig_cur_track_idx_changed(int track_idx, int playlist_idx);

	/**
	 * @brief emitted when new playlist has been added
	 * @param pl reference to new playlist
	 */
	void sig_new_playlist_added(PlaylistPtr pl);

	/**
	 * @brief emitted when playlist name has changed
	 * @param idx index of playlist
	 */
	void sig_playlist_name_changed(int idx);

	/**
	 * @brief emitted when saved playlists have changed
	 */
	void sig_saved_playlists_changed();


	void sig_playlist_idx_changed(int idx);


public:

	/**
	 * @brief clears the current visible playlist
	 * @param pl_idx playlist index
	 */
	void clear_playlist(int pl_idx);

	/**
	 * @brief insert tracks to active playlist after current playback position
	 * @param v_md list of tracks
	 */
	void play_next(const MetaDataList& v_md);

	/**
	 * @brief insert tracks into a playlist at a given index
	 * @param v_md track list
	 * @param idx track index within playlist
	 * @param pl_idx playlist index
	 */
	void insert_tracks(const MetaDataList& v_md, int idx, int pl_idx);


	/**
	 * @brief append tracks at a given playlist index
	 * @param v_md track list
	 * @param pl_idx playlist index
	 */
	void append_tracks(const MetaDataList& v_md, int pl_idx);

	/**
	 * @brief move rows within playlist
	 * @param idx_list list of row indices to be moved
	 * @param tgt_idx target index where rows should be moved
	 * @param pl_idx playlist index
	 */
	void move_rows(const SP::Set<int>& indexes, int tgt_idx, int pl_idx);


	/**
	 * @brief remove rows from playlist
	 * @param indexes list of row indices to be removed
	 * @param pl_idx playlist index
	 */
	void remove_rows(const SP::Set<int>& indexes, int pl_idx);


	/**
	 * @brief change the track in a given playlist
	 * @param idx track index
	 * @param pl_idx playlist index
	 */
	void change_track(int track_idx, int pl_idx);

	/**
	 * @brief get playlist where current track is playing
	 * @return
	 */
	int get_active_idx_of_cur_track() const;


	/**
	 * @brief get active playlist index
	 * @return
	 */
	int	get_active_idx() const;

	int get_current_idx() const;
	void set_current_idx(int pl_idx);

	/**
	 * @brief get number of playlists
	 * @return
	 */
	int get_playlist_count() const;


	/**
	 * @brief get names of playlists
	 * @return
	 */
	QStringList get_playlist_names() const;


	/**
	 * @brief get specific playlist at given index
	 * @param pl_idx playlist index
	 * @return read only pointer object to a playlist, may be nullptr
	 */
	PlaylistConstPtr get_playlist_at(int pl_idx) const;


	/**
	 * @brief delete the given playlist from database
	 * @param pl_idx playlist index
	 */
	void delete_playlist(int pl_idx);

	/**
	 * @brief close playlist
	 * @param pl_idx playlist index
	 */
	void close_playlist(int pl_idx);


	/**
	 * @brief reload playlist from db
	 * @param pl_idx playlist index
	 */
	void reset_playlist(int pl_idx);

	/**
	 * @brief Request a new name for the playlist (usually new %1 is returned)
	 * @return playlist name
	 */
	QString request_new_playlist_name() const;

	/**
	 * @brief save all playlists to database. This could be done in destructor
	 */
	void save_all_playlists();

	/**
	 * @brief save playlist to database, overwrite old one
	 * @param pl_idx playlist index
	 * @return SaveAnswer
	 */
	PlaylistDBInterface::SaveAsAnswer save_playlist(int pl_idx);


	/**
	 * @brief Save playlist under new name
	 * @param pl_idx playlist index
	 * @param name new playlist name
	 * @param force_override override if name exists
	 * @return AlreadyThere if name exists and force_override is false
	 */
	PlaylistDBInterface::SaveAsAnswer save_playlist_as(int pl_idx, const QString& name, bool force_override);


	/**
	 * @brief rename playlist
	 * @param pl_idx playlist index
	 * @param name new playlist name
	 * @return
	 */
	PlaylistDBInterface::SaveAsAnswer rename_playlist(int pl_idx, const QString& name);



	/**
	 * @brief save a playlist to file
	 * @param filename, if filename does not end with m3u, extension is appended automatically
	 * @param relative relative paths in m3u file
	 */
	void save_playlist_to_file(const QString& filename, bool relative);



public slots:

	/**
	 * @brief load playlists of last session from database
	 * @return number of playlists fetched
	 */
	int	load_old_playlists();



	/**
	 * @brief create a new playlist
	 * @param v_md track list
	 * @param name new playlist name. If no name given, current playlist will be overwritten
	 * @param temporary is the playlist temporary or persistent?
	 * @param type deprecated
	 * @return new playlist index
	 */
	int create_playlist(const MetaDataList& v_md, const QString& name=QString(), bool temporary=true, Playlist::Type type=Playlist::Type::Std);

	/**
	 * @brief create a new playlist (overloaded)
	 * @param pathlist paths, may contain files or directories
	* @param name new playlist name. If no name given, current playlist will be overwritten
	 * @param temporary is the playlist temporary or persistent?
	 * @param type deprecated
	 * @return new playlist index
	 */

	int create_playlist(const QStringList& path_list, const QString& name=QString(), bool temporary=true, Playlist::Type type=Playlist::Type::Std);

	/**
	 * @brief create a new playlist (overloaded)
	 * @param dir directory path
	 * @param name new playlist name. If no name given, current playlist will be overwritten
	 * @param temporary is the playlist temporary or persistent?
	 * @param type deprecated
	 * @return new playlist index
	 */

	int create_playlist(const QString& dir, const QString& name=QString(), bool temporary=true, Playlist::Type type=Playlist::Type::Std);

	/**
	 * @brief create a new playlist (overloaded)
	 * @param pl a CustomPlaylist object fetched from database
	 * @return new playlist index
	 */
	int create_playlist(const CustomPlaylist& pl);


	/**
	 * @brief create a new empty playlist
	 * @param name new playlist name. If no name given, current playlist will be overwritten
	 * @return new playlist index
	 */
	int create_empty_playlist(const QString& name);



private slots:

	/**
	 * @brief play active playlist
	 */
	void played();

	/**
	 * @brief pause active playlist
	 */
	void paused();

	/**
	 * @brief stop active playlist
	 */
	void stopped();

	/**
	 * @brief change track to previous track
	 */
	void previous();

	/**
	 * @brief change track to next track
	 */
	void next();


	/**
	 * @brief PlayManager's playstate has changed
	 */
	void playstate_changed(PlayManager::PlayState);

	void www_track_finished(const MetaData& md);

private:

	DatabaseConnector*		_db=nullptr;
	PlayManager*			_play_manager=nullptr;
	QList<PlaylistPtr>		_playlists;

	int						_active_playlist_idx;
	int						_current_playlist_idx;


	// adds a new playlist, creates it, if name is not in the list of playlists. If name already exists,
	// this function returns the index
	int	add_new_playlist(const QString& name, bool editable, Playlist::Type type=Playlist::Type::Std);

	// raw creation of playlists
	PlaylistPtr new_playlist(Playlist::Type type, int idx, QString name="");


	/**
	 * @brief Checks if playlist exists
	 * @param name playlist name, if empty, current playlist index is returned
	 * @return playlist index, -1 if playlist does not exist, current playlist if name is empty
	 */
	int exists(const QString& name) const;


	/**
	 * @brief get active playlist. If no playlists are available, create one.
	 * If there's no active index, the current index is used
	 * @return
	 */
	PlaylistPtr get_active();

	/**
	 * @brief get_playlist at a given index, return fallback if index is invalid
	 * @param pl_idx playlist index
	 * @param fallback playlist returned when index is invalid
	 * @return
	 */
	PlaylistPtr get_playlist(int pl_idx, PlaylistPtr fallback) const;


	/**
	 * @brief tells PlayManager that the current track has been changed,
	 * sets the current playlist index in settings, may insert the playlist into database nevermind if temporary
	 * @param pl paylist of interest. if nullptr, active playlist is taken
	 */
	void emit_cur_track_changed();

	/**
	 * @brief Set active playlist index, if pl_idx is invalid,
	 * @param pl_idx playlist index
	 */
	void set_active_idx(int pl_idx);


};

#endif /* PLAYLISTHANDLER_H_ */
