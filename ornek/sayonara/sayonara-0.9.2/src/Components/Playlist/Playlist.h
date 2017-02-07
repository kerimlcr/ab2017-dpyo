/* Playlist.h */

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



#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "Helper/Playlist/PlaylistMode.h"
#include "Helper/Settings/SayonaraClass.h"
#include "PlaylistDBInterface.h"

#include <QString>
#include <QList>

#include <memory>

/**
 * @brief The Playlist class
 * @ingroup Playlists
 */
class Playlist :
		public PlaylistDBInterface,
		protected SayonaraClass
{

	Q_OBJECT

	friend class PlaylistHandler;

signals:
	void sig_data_changed(int idx);

public:

	enum class Type : quint8
	{
		Std=0,
		Stream
	};

private:
	bool            _playlist_changed;


protected:

	bool			_is_storable;
	int				_playlist_idx;

	MetaDataList    _v_md;
	Type			_playlist_type;
	PlaylistMode	_playlist_mode;

	Playlist(int idx, QString name="");
	virtual ~Playlist();

	virtual void play()=0;
	virtual void pause()=0;
	virtual void stop()=0;
	virtual void fwd()=0;
	virtual void bwd()=0;
    virtual void next()=0;

	virtual int create_playlist(const MetaDataList& v_md)=0;

    virtual void replace_track(int idx, const MetaData& md);

public:

	QStringList		toStringList() const;

	IdxList			find_tracks(int id) const;
	IdxList			find_tracks(const QString& filepath) const;

	Type			get_type() const;
	int				get_cur_track_idx() const;
	bool			get_cur_track(MetaData& md) const;
	int				get_idx() const;
	void			set_idx(int idx);	
	PlaylistMode	get_playlist_mode() const;
	void			set_playlist_mode(const PlaylistMode& mode);	
	quint64			get_running_time() const;


	// from PlaylistDBInterface
	virtual bool				is_empty() const override;
	virtual int					get_count() const override;
	virtual const MetaDataList&	get_playlist() const override;

	virtual void			set_changed(bool b) override;
	virtual bool			was_changed() const override;
	virtual bool			is_storable() const override;



	const MetaData& operator[](int idx) const{
		return _v_md[idx];
	}

	const MetaData& at_const_ref(int idx) const {
		return _v_md[idx];
	}

	MetaData& at_ref(int idx) {
		return _v_md[idx];
	}


	virtual void clear();

	virtual void move_track(const int idx, int tgt);
	virtual void move_tracks(const SP::Set<int>& indexes, int tgt);

	virtual void copy_track(const int idx, int tgt);
	virtual void copy_tracks(const SP::Set<int>& indexes, int tgt);

	virtual void delete_track(const int idx);
	virtual void delete_tracks(const SP::Set<int>& indexes);

	virtual void insert_track(const MetaData& md, int tgt);
	virtual void insert_tracks(const MetaDataList& lst, int tgt);

	virtual void append_track(const MetaData& md);
	virtual void append_tracks(const MetaDataList& lst);

	virtual bool change_track(int idx)=0;

	virtual void metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new)=0;
	virtual void metadata_changed_single(const MetaData& md)=0;


private slots:
	void			_sl_playlist_mode_changed();
};


/**
 * @brief PlaylistPtr Playlist Pointer (only used internally)
 */
typedef std::shared_ptr<Playlist> PlaylistPtr;

/**
 * @brief PlaylistConstPtr read only Playlist Pointer
 */
typedef std::shared_ptr<const Playlist> PlaylistConstPtr;




#endif // PLAYLIST_H
