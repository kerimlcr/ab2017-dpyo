/* PlaylistDBInterface.h */

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



#ifndef PLAYLISTDBINTERFACE_H
#define PLAYLISTDBINTERFACE_H

#include "Helper/MetaData/MetaData.h"
#include "Helper/Playlist/CustomPlaylist.h"

class PlaylistDBWrapper;
/**
 * @brief The PlaylistDBInterface class
 * @ingroup Playlists
 */
class PlaylistDBInterface : public QObject {

	Q_OBJECT

private:

	PlaylistDBWrapper*	_playlist_db_connector=nullptr;
	QString					_name;
	bool					_is_temporary;
	int						_id;
	bool					_save_enabled;

public:

	enum class SaveAsAnswer : quint8 {
		Success=0,
		AlreadyThere,
		ExternTracksError,
		Error
	};

	PlaylistDBInterface(const QString& name);

	int get_id() const;
	void set_id(int db_id);

	QString get_name() const;
	void set_name(const QString& name);

	bool is_temporary() const;
	void set_temporary(bool b);

	void set_skeleton(const CustomPlaylistSkeleton& skeleton);

	bool insert_temporary_into_db();
	SaveAsAnswer save();
	SaveAsAnswer save_as(const QString& str, bool force_override);
	SaveAsAnswer rename(const QString& str);
	bool delete_playlist();
	bool remove_from_db();


	virtual const MetaDataList& get_playlist() const = 0;
	virtual int get_count() const = 0;
	virtual bool is_empty() const = 0;
	virtual void set_changed(bool b) = 0;
	virtual bool was_changed() const = 0;
	virtual bool is_storable() const =0;
	void enable_saving(bool b);
	bool is_save_enabled() const;

	static QString request_new_db_name();
};

#endif // PLAYLISTDBINTERFACE_H
