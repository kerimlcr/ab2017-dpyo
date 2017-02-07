/* SoundcloudLibrary.cpp */

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



#include "SoundcloudLibrary.h"
#include "SoundcloudDataFetcher.h"
#include "Components/Covers/CoverLookup.h"
#include "Components/Covers/CoverLocation.h"


SoundcloudLibrary::SoundcloudLibrary(QObject *parent) :
	AbstractLibrary(parent)
{
	_scd = SoundcloudData::getInstance();
}

SoundcloudLibrary::~SoundcloudLibrary(){

}

void SoundcloudLibrary::load(){
	AbstractLibrary::load();

	ArtistList artists;
	get_all_artists(artists);
}


void SoundcloudLibrary::get_all_artists(ArtistList& artists, LibSortOrder so){
	_scd->getAllArtists(artists, so.so_artists);
}

void SoundcloudLibrary::get_all_artists_by_searchstring(Filter filter, ArtistList& artists, LibSortOrder so){
	_scd->getAllArtistsBySearchString(filter, artists, so.so_artists);
}

void SoundcloudLibrary::get_all_albums(AlbumList& albums, LibSortOrder so){
	_scd->getAllAlbums(albums, so.so_albums);
}

void SoundcloudLibrary::get_all_albums_by_artist(IDList artist_ids, AlbumList& albums, Filter filter, LibSortOrder so){
	_scd->getAllAlbumsByArtist(artist_ids, albums, filter, so.so_albums);
}

void SoundcloudLibrary::get_all_albums_by_searchstring(Filter filter, AlbumList& albums, LibSortOrder so){
	_scd->getAllAlbumsBySearchString(filter, albums, so.so_albums);
}

void SoundcloudLibrary::get_all_tracks(const QStringList& paths, MetaDataList& v_md){
	Q_UNUSED(paths)
	Q_UNUSED(v_md)
	return;
}

void SoundcloudLibrary::get_all_tracks(MetaDataList& v_md, LibSortOrder so){
	_scd->getTracksFromDatabase(v_md, so.so_tracks);
}

void SoundcloudLibrary::get_all_tracks_by_artist(IDList artist_ids, MetaDataList& v_md, Filter filter, LibSortOrder so){
	_scd->getAllTracksByArtist(artist_ids, v_md, filter, so.so_tracks);
}

void SoundcloudLibrary::get_all_tracks_by_album(IDList album_ids, MetaDataList& v_md, Filter filter, LibSortOrder so){
	_scd->getAllTracksByAlbum(album_ids, v_md, filter, so.so_tracks);
}

void SoundcloudLibrary::get_all_tracks_by_searchstring(Filter filter, MetaDataList& v_md, LibSortOrder so){
	_scd->getAllTracksBySearchString(filter, v_md, so.so_tracks);

}

void SoundcloudLibrary::get_album_by_id(int album_id, Album& album){
	_scd->getAlbumByID(album_id, album);
}

void SoundcloudLibrary::get_artist_by_id(int artist_id, Artist& artist){
	_scd->getArtistByID(artist_id, artist);
}


void SoundcloudLibrary::update_track(const MetaData& md){
	_scd->updateTrack(md);
}

void SoundcloudLibrary::update_album(const Album& album){
	_scd->updateAlbum(album);
}

void SoundcloudLibrary::delete_tracks(const MetaDataList& v_md, Library::TrackDeletionMode mode){
	Q_UNUSED(mode)
	_scd->deleteTracks(v_md);
	refresh();
}

void SoundcloudLibrary::psl_reload_library(bool b, Library::ReloadQuality quality){
	Q_UNUSED(b)
	Q_UNUSED(quality)
}

void SoundcloudLibrary::refresh_artist(){

	if(_selected_artists.isEmpty()){
		return;
	}

	MetaDataList v_md;
	int artist_id = _selected_artists.first();

	IDList artist_ids;
	artist_ids << artist_id;

	get_all_tracks_by_artist(artist_ids, v_md, Filter(), LibSortOrder());

	delete_tracks(v_md, Library::TrackDeletionMode::OnlyLibrary);
	sp_log(Log::Debug) << "Deleted " << v_md.size() << " soundcloud tracks";

	SoundcloudDataFetcher* fetcher = new SoundcloudDataFetcher(this);

	connect(fetcher, &SoundcloudDataFetcher::sig_artists_fetched,
			this, &SoundcloudLibrary::artists_fetched);


	fetcher->get_artist(artist_id);
}


void SoundcloudLibrary::refresh_albums(){

}

void SoundcloudLibrary::refresh_tracks(){

}



void SoundcloudLibrary::cover_lookup_finished(bool){
	CoverLookup* cover_lookup = static_cast<CoverLookup*>(sender());
	cover_lookup->deleteLater();
}

void SoundcloudLibrary::cover_found(const CoverLocation& cl){
	sp_log(Log::Debug) << "Saved sound cloud cover: " << cl.toString();
}



void SoundcloudLibrary::insert_tracks(const MetaDataList& v_md){
	Q_UNUSED(v_md)
	return;
}

void SoundcloudLibrary::insert_tracks(const MetaDataList& v_md, const ArtistList& artists, const AlbumList& albums){

	Artist artist_tmp;
	Album album_tmp;

	for(const Artist& artist : artists){
		if(!_scd->getArtistByID(artist.id, artist_tmp) || artist.id != artist_tmp.id){
			int id = _scd->insertArtistIntoDatabase(artist);
			sp_log(Log::Debug) << "Insert artist into database: " << id;
		}
	}

	for(const Album& album : albums){
		sp_log(Log::Debug) << "Try to fetch album " << album.id;
		if(!_scd->getAlbumByID(album.id, album_tmp) || album.id != album_tmp.id){
			int id = _scd->insertAlbumIntoDatabase(album);
			sp_log(Log::Debug) << "Insert album into database: " << id;
		}

		else{
			album_tmp.print();
		}
	}

	if(!_scd->getAlbumByID(-1, album_tmp)){
		Album album;
		album.name = "None";
		album.id = 0;

		_scd->insertAlbumIntoDatabase(album);
	}

	_scd->storeMetadata(v_md);

	AbstractLibrary::insert_tracks(v_md);
}


void SoundcloudLibrary::artists_fetched(const ArtistList& artists){

	for(const Artist& artist : artists){

		SoundcloudDataFetcher* fetcher;

		if(artist.id <= 0) {
			continue;
		}

		_scd->updateArtist(artist);

		fetcher = new SoundcloudDataFetcher(this);

		connect(fetcher, &SoundcloudDataFetcher::sig_playlists_fetched,
				this, &SoundcloudLibrary::albums_fetched);

		connect(fetcher, &SoundcloudDataFetcher::sig_tracks_fetched,
				this, &SoundcloudLibrary::tracks_fetched);

		fetcher->get_tracks_by_artist(artist.id);
	}

	refetch();

	sender()->deleteLater();
}

void SoundcloudLibrary::tracks_fetched(const MetaDataList& v_md){

	for(const MetaData& md : v_md){
		if(md.id > 0){
			_scd->insertTrackIntoDatabase(md, md.artist_id, md.album_id);
		}
	}

	refetch();

	sender()->deleteLater();
}

void SoundcloudLibrary::albums_fetched(const AlbumList& albums){
	for(const Album& album : albums){
		if(album.id > 0){
			_scd->insertAlbumIntoDatabase(album);
		}
	}

	refetch();
	//sender()->deleteLater();
}
