/* SoundcloudData.cpp */

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


#include "SoundcloudData.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Database/SayonaraQuery.h"

SoundcloudData::SoundcloudData() :
	LibraryDatabase(1, "", "soundcloud.db")
{

	QString artist_fetch_query = QString("SELECT ") +
			"artists.artistid AS artistID, "
			"artists.name AS artistName, "
			"artists.permalink_url AS permalink_url, "
			"artists.description AS description, "
			"artists.followers_following AS followers_following, "
			"artists.cover_url AS cover_url, "
			"COUNT(DISTINCT tracks.trackid) AS artistNTracks, "
			"GROUP_CONCAT(DISTINCT albums.albumid) AS artistAlbums "
			"FROM artists, albums, tracks ";


	QString album_fetch_query = QString("SELECT ") +
			"albums.albumID AS albumID, "
			"albums.name AS albumName, "
			"SUM(tracks.length) / 1000 AS albumLength, "
			"albums.rating AS albumRating, "
			"albums.permalink_url AS permalink_url, "
			"albums.purchase_url AS purchase_url, "
			"albums.cover_url AS cover_url, "
			"COUNT(DISTINCT tracks.trackid) AS albumNTracks, "
			"MAX(tracks.year) AS albumYear, "
			"GROUP_CONCAT(DISTINCT artists.name) AS albumArtists, "
			"GROUP_CONCAT(DISTINCT tracks.discnumber) AS discnumbers "
			"FROM albums, artists, tracks ";

	QString track_fetch_query = QString("SELECT ") +
			"tracks.trackID AS trackID, "
			"tracks.title AS trackTitle, "
			"tracks.length AS trackLength, "
			"tracks.year AS trackYear, "
			"tracks.bitrate AS trackBitrate, "
			"tracks.filename AS trackFilename, "
			"tracks.track AS trackNum, "
			"albums.albumID AS albumID, "
			"artists.artistID AS artistID, "
			"albums.name AS albumName, "
			"artists.name AS artistName, "
			"tracks.genre AS genrename, "
			"tracks.filesize AS filesize, "
			"tracks.discnumber AS discnumber, "
			"tracks.purchase_url AS purchase_url, "
			"tracks.cover_url AS cover_url, "
			"tracks.rating AS rating "
			"FROM tracks "
			"INNER JOIN albums ON tracks.albumID = albums.albumID "
			"INNER JOIN artists ON tracks.artistID = artists.artistID "
			"WHERE filetype is null ";

	set_artist_fetch_query(artist_fetch_query);
	set_album_fetch_query(album_fetch_query);
	set_track_fetch_query(track_fetch_query);
}



SoundcloudData::~SoundcloudData()
{
}

bool SoundcloudData::db_fetch_tracks(SayonaraQuery& q, MetaDataList& result){

	result.clear();

	if (!q.exec()) {
		q.show_error("Cannot fetch tracks from database");
		return false;
	}

	if(!q.last()){
		return true;
	}

	for(bool is_element = q.first(); is_element; is_element = q.next()){

		MetaData data;

		data.id = 		 q.value(0).toInt();
		data.title = 	 q.value(1).toString();
		data.length_ms = q.value(2).toInt();
		data.year = 	 q.value(3).toInt();
		data.bitrate = 	 q.value(4).toInt();
		data.set_filepath(q.value(5).toString());
		data.track_num = q.value(6).toInt();
		data.album_id =  q.value(7).toInt();
		data.artist_id = q.value(8).toInt();
		data.album = 	 q.value(9).toString().trimmed();
		data.artist = 	 q.value(10).toString().trimmed();
		data.genres =	 q.value(11).toString().split(",");
		data.filesize =  q.value(12).toInt();
		data.discnumber = q.value(13).toInt();
		data.add_custom_field("purchase_url", tr("Purchase Url"), q.value(14).toString());
		data.cover_download_url = q.value(15).toString();
		data.rating = q.value(16).toInt();
		data.db_id = this->get_id();

		result << data;
	}

	return true;
}

bool SoundcloudData::db_fetch_albums(SayonaraQuery& q, AlbumList& result){

	result.clear();

	if (!q.exec()) {
		q.show_error("Could not get all albums from database");
		return false;
	}

	while(q.next()){

		Album album;

		album.id =					q.value(0).toInt();
		album.name =				q.value(1).toString().trimmed();
		album.length_sec =			q.value(2).toInt();
		album.rating =				q.value(3).toInt();
		album.add_custom_field("permalink_url", "Permalink Url", q.value(4).toString());
		album.add_custom_field("purchase_url", "Purchase Url", q.value(5).toString());
		album.cover_download_url =	q.value(6).toString();
		album.num_songs =			q.value(7).toInt();
		album.year =				q.value(8).toInt();

		QStringList lst_artists =	q.value(9).toString().split(',');
		album.artists = lst_artists;

		QStringList lst_discnumbers = q.value(10).toString().split(',');
		album.discnumbers.clear();

		for(const QString& disc : lst_discnumbers) {
			int d = disc.toInt();
			if(album.discnumbers.contains(d)) continue;

			album.discnumbers << d;
		}

		if(album.discnumbers.isEmpty()) {
			album.discnumbers << 1;
		}

		album.n_discs = album.discnumbers.size();
		album.is_sampler = (lst_artists.size() > 1);
		album.db_id = this->get_id();

		result << album;
	};

	return true;
}

bool SoundcloudData::db_fetch_artists(SayonaraQuery& q, ArtistList& result){
	result.clear();

	if (!q.exec()) {
		q.show_error("Could not get all artists from database");
		return false;
	}

	if(!q.last()){
		return true;
	}

	for(bool is_element=q.first(); is_element; is_element = q.next()){

		Artist artist;

		artist.id =						q.value(0).toInt();
		artist.name =					q.value(1).toString().trimmed();

		artist.add_custom_field("permalink_url", "Permalink Url", q.value(2).toString());
		artist.add_custom_field("description", "Description", q.value(3).toString());
		artist.add_custom_field("followers_following", "Followers/Following", q.value(4).toString());

		artist.cover_download_url =		q.value(5).toString();
		artist.num_songs =				q.value(6).toInt();
		QStringList list =				q.value(7).toString().split(',');
		artist.num_albums =				list.size();
		artist.db_id =					this->get_id();

		result << artist;
	}

	return true;
}

int SoundcloudData::updateArtist(const Artist& artist){
	SayonaraQuery q (_database);

	QString query_text = QString("UPDATE artists SET ") +
			"name = :name, "
			"cissearch = :cissearch, "
			"permalink_url = :permalink_url, "
			"description = :description, "
			"followers_following = :followers_following, "
			"cover_url = :cover_url "
			"WHERE artistid = :sc_id;";

	q.prepare(query_text);
	q.bindValue(":sc_id", artist.id);
	q.bindValue(":name", artist.name);
	q.bindValue(":cissearch", artist.name.toLower());
	q.bindValue(":permalink_url", artist.get_custom_field("permalink_url"));
	q.bindValue(":description", artist.get_custom_field("description"));
	q.bindValue(":followers_following", artist.get_custom_field("followers_following"));
	q.bindValue(":cover_url", artist.cover_download_url);

	if (!q.exec()) {
		q.show_error(QString("Soundcloud: Cannot update artist ") + artist.name);
		return -1;
	}

	return getArtistID(artist.name);
}

int SoundcloudData::insertArtistIntoDatabase (const QString& artist){
	Q_UNUSED(artist)
	return -1;
}

int SoundcloudData::insertArtistIntoDatabase (const Artist& artist){
	SayonaraQuery q (_database);

	Artist tmp_artist;
	if(getArtistByID(artist.id, tmp_artist)){
		if(tmp_artist.id > 0){
			return updateArtist(artist);
		}
	}

	QString query_text = QString("INSERT INTO artists") +
			"(artistid, name, cissearch, permalink_url, description, followers_following, cover_url) "
			"VALUES "
			"(:sc_id, :name, :cissearch, :permalink_url, :description, :followers_following, :cover_url); ";

	q.prepare(query_text);
	q.bindValue(":sc_id", artist.id);
	q.bindValue(":name", artist.name);
	q.bindValue(":cissearch", artist.name.toLower());
	q.bindValue(":permalink_url", artist.get_custom_field("permalink_url"));
	q.bindValue(":description", artist.get_custom_field("description"));
	q.bindValue(":followers_following", artist.get_custom_field("followers_following"));
	q.bindValue(":cover_url", artist.cover_download_url);

	if (!q.exec()) {
		q.show_error(QString("Soundcloud: Cannot insert artist ") + artist.name);
		return -1;
	}

	return getArtistID(artist.name);
}


int SoundcloudData::updateAlbum(const Album& album){
	SayonaraQuery q (_database);

	QString query_text = QString("UPDATE albums SET ") +
			"name = :name, "
			"cissearch = :cissearch, "
			"permalink_url = :permalink_url, "
			"purchase_url = :purchase_url, "
			"cover_url = :cover_url "
			"WHERE albumID = :sc_id;";

	q.prepare(query_text);

	q.bindValue(":sc_id", album.id);
	q.bindValue(":name", album.name);
	q.bindValue(":cissearch", album.name.toLower());
	q.bindValue(":permalink_url", album.get_custom_field("permalink_url"));
	q.bindValue(":purchase_url", album.get_custom_field("purchase_url"));
	q.bindValue(":cover_url", album.cover_download_url);

	if (!q.exec()) {
		q.show_error(QString("Soundcloud: Cannot insert album ") + album.name);
		return -1;
	}

	return getAlbumID(album.name);
}


int SoundcloudData::insertAlbumIntoDatabase (const QString& album){
	Q_UNUSED(album)
	return -1;
}

int SoundcloudData::insertAlbumIntoDatabase (const Album& album){
	SayonaraQuery q (_database);

	Album tmp_album;
	if(getAlbumByID(album.id, tmp_album) && tmp_album.id > 0){
		return updateAlbum(album);
	}

	QString query_text = QString("INSERT INTO albums ") +
			"(albumid, name, cissearch, permalink_url, purchase_url, cover_url) "
			"VALUES "
			"(:sc_id, :name, :cissearch, :permalink_url, :purchase_url, :cover_url); ";

	q.prepare(query_text);

	q.bindValue(":sc_id", album.id);
	q.bindValue(":name", album.name);
	q.bindValue(":cissearch", album.name.toLower());
	q.bindValue(":permalink_url", album.get_custom_field("permalink_url"));
	q.bindValue(":purchase_url", album.get_custom_field("purchase_url"));
	q.bindValue(":cover_url", album.cover_download_url);

	if (!q.exec()) {
		q.show_error(QString("Soundcloud: Cannot insert album ") + album.name);
		return -1;
	}

	return getAlbumID(album.name);
}

bool SoundcloudData::updateTrack(const MetaData& md){
	SayonaraQuery q (_database);

	sp_log(Log::Info) << "insert new track: " << md.filepath();

	QString querytext = QString("UPDATE tracks SET ") +
				"filename = :filename, "
				"albumID = :albumID, "
				"artistID = :artistID, "
				"length = :length, "
				"year = :year, "
				"title = :title, "
				"track = :track, "
				"bitrate = :bitrate, "
				"genre = :genre, "
				"filesize = :filesize, "
				"discnumber = :discnumber, "
				"cissearch = :cissearch, "
				"purchase_url = :purchase_url, "
				"cover_url = :cover_url "
				"WHERE trackID = :sc_id;";

	q.prepare(querytext);

	q.bindValue(":sc_id", md.id);
	q.bindValue(":filename", md.filepath());
	q.bindValue(":albumID", md.album_id);
	q.bindValue(":artistID",md.artist_id);
	q.bindValue(":length", md.length_ms);
	q.bindValue(":year", md.year);
	q.bindValue(":title", md.title);
	q.bindValue(":track", md.track_num);
	q.bindValue(":bitrate", md.bitrate);
	q.bindValue(":genre", md.genres.join(","));
	q.bindValue(":filesize", md.filesize);
	q.bindValue(":discnumber", md.discnumber);
	q.bindValue(":cissearch", md.title.toLower());
	q.bindValue(":purchase_url", md.get_custom_field("purchase_url"));
	q.bindValue(":cover_url", md.cover_download_url);

	if (!q.exec()) {

		q.show_error(QString("Cannot insert track into database ") + md.filepath());
		return false;
	}

	return true;
}
bool SoundcloudData::insertTrackIntoDatabase(const MetaData &md, int artistID, int albumID){

	SayonaraQuery q (_database);

	int new_id;

	new_id = getTrackById(md.id).id;
	if(new_id > 0){
		return updateTrack(md);
	}

	sp_log(Log::Info) << "insert new track: " << md.filepath();

	QString querytext = QString("INSERT INTO tracks ") +
				"(trackid,filename,albumID,artistID,title,year,length,track,bitrate,genre,filesize,discnumber,cissearch,purchase_url,cover_url) " +
				"VALUES "+
				"(:sc_id,:filename,:albumID,:artistID,:title,:year,:length,:track,:bitrate,:genre,:filesize,:discnumber,:cissearch,:purchase_url,:cover_url); ";

	q.prepare(querytext);

	q.bindValue(":sc_id", md.id);
	q.bindValue(":filename", md.filepath());
	q.bindValue(":albumID", albumID);
	q.bindValue(":artistID",artistID);
	q.bindValue(":length", md.length_ms);
	q.bindValue(":year", md.year);
	q.bindValue(":title", md.title);
	q.bindValue(":track", md.track_num);
	q.bindValue(":bitrate", md.bitrate);
	q.bindValue(":genre", md.genres.join(","));
	q.bindValue(":filesize", md.filesize);
	q.bindValue(":discnumber", md.discnumber);
	q.bindValue(":cissearch", md.title.toLower());
	q.bindValue(":purchase_url", md.get_custom_field("purchase_url"));

	q.bindValue(":cover_url", md.cover_download_url);

	if (!q.exec()) {

		q.show_error(QString("Cannot insert track into database ") + md.filepath());
		return false;
	}

	return true;
}



bool SoundcloudData::storeMetadata(const MetaDataList& v_md)  {

	if(v_md.isEmpty()) return true;

	_database.transaction();

	for(const MetaData& md : v_md) {

		sp_log(Log::Debug) << "Looking for " << md.artist << " and " << md.album;
		if(md.album_id == -1 || md.artist_id == -1){
			sp_log(Log::Warning) << "AlbumID = " << md.album_id << " - ArtistID = " << md.artist_id;
			continue;
		}

		insertTrackIntoDatabase (md, md.artist_id, md.album_id);
	}

	return _database.commit();
}


bool SoundcloudData::apply_fixes(){
	return true;
}
