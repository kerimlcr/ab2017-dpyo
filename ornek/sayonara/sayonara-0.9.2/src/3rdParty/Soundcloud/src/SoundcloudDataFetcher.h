/* SoundcloudDataFetcher.h */

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



#ifndef SOUNDCLOUDDATAFETCHER_H
#define SOUNDCLOUDDATAFETCHER_H


#include "Helper/MetaData/MetaDataList.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"




class SoundcloudDataFetcher : public QObject
{
	Q_OBJECT

signals:
	void sig_ext_artists_fetched(const ArtistList& artists);
	void sig_artists_fetched(const ArtistList& artists);
	void sig_playlists_fetched(const AlbumList& albums);
	void sig_tracks_fetched(const MetaDataList& v_md);

public:
	SoundcloudDataFetcher(QObject* parent=nullptr);
	~SoundcloudDataFetcher();

	void search_artists(const QString& artist_name);
	void get_artist(int artist_id);

	void get_playlists_by_artist(int artist_id);
	void get_tracks_by_artist(int artist_id);

	void clear();

private slots:

	void artists_fetched(bool success);
	void playlist_tracks_fetched(bool success);
	void tracks_fetched(bool success);

private:
	MetaDataList	_playlist_tracks;
	AlbumList		_playlists;
	ArtistList		_artists;
	int				_artist_id;

};

#endif // SOUNDCLOUDDataFetcher_H
