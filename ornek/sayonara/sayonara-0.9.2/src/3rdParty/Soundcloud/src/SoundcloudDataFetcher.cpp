/* SoundcloudDataFetcher.cpp */

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



#include "SoundcloudDataFetcher.h"
#include "SoundcloudWebAccess.h"
#include "SoundcloudJsonParser.h"

#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"
#include "Helper/WebAccess/AsyncWebAccess.h"


SoundcloudDataFetcher::SoundcloudDataFetcher(QObject* parent) :
	QObject(parent)
{
	clear();
}

SoundcloudDataFetcher::~SoundcloudDataFetcher()
{

}


void SoundcloudDataFetcher::search_artists(const QString& artist_name){

	clear();

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished,
			this, &SoundcloudDataFetcher::artists_fetched);
	awa->run( SoundcloudWebAccess::create_dl_get_artist(artist_name));
}

void SoundcloudDataFetcher::get_artist(int artist_id){

	clear();

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished,
			this, &SoundcloudDataFetcher::artists_fetched);

	awa->run( SoundcloudWebAccess::create_dl_get_artist(artist_id) );
}


void SoundcloudDataFetcher::get_playlists_by_artist(int artist_id){
	return get_tracks_by_artist(artist_id);
}


void SoundcloudDataFetcher::get_tracks_by_artist(int artist_id){

	clear();

	_artist_id = artist_id;

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished,
			this, &SoundcloudDataFetcher::playlist_tracks_fetched);

	awa->run( SoundcloudWebAccess::create_dl_get_playlists(artist_id) );
}


void SoundcloudDataFetcher::artists_fetched(bool success){

	ArtistList artists;
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	if(!success){
		awa->deleteLater();
		return;
	}

	QByteArray data = awa->get_data();
	SoundcloudJsonParser parser(data);
	parser.parse_artists(artists);

	emit sig_artists_fetched(artists);
	awa->deleteLater();
}


void SoundcloudDataFetcher::playlist_tracks_fetched(bool success){


	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());

	if(!success){
		awa->deleteLater();
		return;
	}

	QByteArray data = awa->get_data();
	SoundcloudJsonParser parser(data);
	parser.parse_playlists(_artists, _playlists, _playlist_tracks);

	AsyncWebAccess* awa_new = new AsyncWebAccess(this);
	connect(awa_new, &AsyncWebAccess::sig_finished,
			this, &SoundcloudDataFetcher::tracks_fetched);

	awa_new->run( SoundcloudWebAccess::create_dl_get_tracks(_artist_id) );

	awa->deleteLater();
}

void SoundcloudDataFetcher::tracks_fetched(bool success){

	MetaDataList v_md;
	ArtistList artists;
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());

	if(!success){
		awa->deleteLater();
		return;
	}

	QByteArray data = awa->get_data();
	SoundcloudJsonParser parser(data);
	parser.parse_tracks(artists, v_md);

	for(const MetaData& md : v_md){
		if(!_playlist_tracks.contains(md.id)){
			_playlist_tracks << md;
		}
	}

	for(const Artist& artist : artists){
		if(!_artists.contains(artist.id)){
			_artists << artist;
		}
	}

	emit sig_playlists_fetched(_playlists);
	emit sig_tracks_fetched(_playlist_tracks);
	emit sig_ext_artists_fetched(_artists);

	awa->deleteLater();
}

void SoundcloudDataFetcher::clear(){
	_playlist_tracks.clear();
	_playlists.clear();
	_artists.clear();
	_artist_id = -1;
}

