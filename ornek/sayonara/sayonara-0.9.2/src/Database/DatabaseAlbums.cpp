/* DatabaseAlbums.cpp */

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


#include "Database/SayonaraQuery.h"
#include "Database/DatabaseAlbums.h"
#include "Helper/MetaData/Album.h"

DatabaseAlbums::DatabaseAlbums(QSqlDatabase db, quint8 db_id) :
	DatabaseModule(db, db_id)
{
	_fetch_query = QString("SELECT ") +
					"albums.albumID AS albumID, " +
					"albums.name AS albumName, " +
					"SUM(tracks.length) / 1000 AS albumLength, " +
					"albums.rating AS albumRating, " +
					"COUNT(DISTINCT tracks.trackid) AS albumNTracks, " +
					"MAX(tracks.year) AS albumYear, " +
					"GROUP_CONCAT(DISTINCT artists.name) AS albumArtists, " +
					"GROUP_CONCAT(DISTINCT tracks.discnumber) AS discnumbers " +
					"FROM albums, artists, tracks ";
}


bool DatabaseAlbums::db_fetch_albums(SayonaraQuery& q, AlbumList& result) {

	result.clear();

	if (!q.exec()) {
		q.show_error("Could not get all albums from database");
		return false;
	}

	if(!q.last()){
		return true;
	}

	for(bool is_element=q.first(); is_element; is_element = q.next()){

		Album album;

		album.id = q.value(0).toInt();
		album.name = q.value(1).toString().trimmed();
		album.length_sec = q.value(2).toInt();
		album.rating = q.value(3).toInt();
		album.num_songs = q.value(4).toInt();
		album.year = q.value(5).toInt();
		QStringList artistList = q.value(6).toString().split(',');
		album.artists = artistList;

		QStringList discnumberList = q.value(7).toString().split(',');
		album.discnumbers.clear();

		for(const QString& disc : discnumberList) {
			int d = disc.toInt();
			if(album.discnumbers.contains(d)) continue;

			album.discnumbers << d;
		}

		if(album.discnumbers.size() == 0) {
			album.discnumbers << 1;
		}

		album.n_discs = album.discnumbers.size();
		album.is_sampler = (artistList.size() > 1);
		album.db_id = _module_db_id;

		result << album;
	};

	return true;
}

void DatabaseAlbums::set_album_fetch_query(const QString &query){
	_fetch_query = query;
}

QString DatabaseAlbums::_create_order_string(SortOrder sortorder) {

	switch(sortorder) {

		case SortOrder::AlbumNameAsc:
			return QString (" ORDER BY albumName ASC ");

		case SortOrder::AlbumNameDesc:
			return QString (" ORDER BY albumName DESC ");

		case SortOrder::AlbumYearAsc:
			return QString (" ORDER BY albumYear ASC, albumName ASC ");

		case SortOrder::AlbumYearDesc:
			return QString (" ORDER BY albumYear DESC, albumName ASC ");

		case SortOrder::AlbumTracksAsc:
			return QString (" ORDER BY albumNTracks ASC, albumName ASC ");

		case SortOrder::AlbumTracksDesc:
			return QString (" ORDER BY albumNTracks DESC, albumName ASC ");

		case SortOrder::AlbumDurationAsc:
			return QString (" ORDER BY albumLength ASC, albumName ASC ");

		case SortOrder::AlbumDurationDesc:
			return QString (" ORDER BY albumLength DESC, albumName ASC ");

		case SortOrder::AlbumRatingAsc:
			return QString (" ORDER BY albumRating ASC, albumName ASC ");

		case SortOrder::AlbumRatingDesc:
			return QString (" ORDER BY albumRating DESC, albumName ASC ");

		default:
			return "";
	}
}

int DatabaseAlbums::getAlbumID (const QString & album)  {

	SayonaraQuery q (_db);
	int albumID = -1;

	q.prepare("SELECT albumID FROM albums WHERE name = ?;");
	q.addBindValue(album);

	if(!q.exec()) {
		return -1;
	}

	if (q.next()) {
		albumID = q.value(0).toInt();
	}

	return albumID;
}

int DatabaseAlbums::getMaxAlbumID() {

	int max_id = -1;

	SayonaraQuery q (_db);
	q.prepare("SELECT MAX(albumID) FROM albums;");

	if (!q.exec()) {
		return -1;
	}

	while (q.next()) {
		max_id = q.value(0).toInt();
	}

	return max_id;

}

bool DatabaseAlbums::getAlbumByID(const int& id, Album& album) {

	if(id == -1) return false;

	AlbumList albums;

	SayonaraQuery q (_db);
	QString querytext =
			_fetch_query +
			"WHERE albums.albumID = :id "
			"AND tracks.albumID = albums.albumID "
			"AND artists.artistID = tracks.artistID;";


	q.prepare(querytext);
	q.bindValue(":id", QVariant(id));

	db_fetch_albums(q, albums);
	if(albums.size() > 0) {
		album = albums[0];
	}

	return (albums.size() > 0);
}

bool DatabaseAlbums::getAllAlbums(AlbumList& result, SortOrder sortorder, bool also_empty) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	QString querytext = _fetch_query ;

	if(!also_empty){
			querytext += " WHERE tracks.albumID = albums.albumID "
					"AND artists.artistid = tracks.artistid ";

	}

	querytext += "GROUP BY albums.albumID, albumName";


	querytext += _create_order_string(sortorder) + ";";

	q.prepare(querytext);

	return db_fetch_albums(q, result);
}


bool DatabaseAlbums::getAllAlbumsByArtist(IDList artists, AlbumList& result, Filter filter,SortOrder sortorder) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);

	// fetch all albums
	QString querytext =
			_fetch_query +
			"WHERE tracks.albumID = albums.albumID AND "
			"artists.artistid = tracks.artistid AND ";

	if(artists.size() == 0) {
		return false;
	}

	else if(artists.size() > 1) {
		querytext += "(artists.artistid = :artist_id ";
		for(int i=1; i<artists.size(); i++) {
			querytext += QString("OR artists.artistid = :artist_id_" + QString::number(i) + " ");
		}

		querytext += ") ";
	}

	else{
		querytext += "artists.artistid = :artist_id ";
	}


	if( !filter.filtertext.isEmpty() ) {

		switch(filter.mode) {

			case Filter::Mode::Genre:
				querytext += QString("AND tracks.genre LIKE :filter1 ");			// track title is like filter
				break;

			case Filter::Mode::Filename:
				querytext += QString("AND tracks.filename LIKE :filter1 ");			// track title is like filter

				break;

			case Filter::Mode::Fulltext:
			default:
				querytext += QString("AND tracks.trackID IN ( "
									 "SELECT t2.trackID "
									 "FROM tracks t2 "
									 "WHERE t2.cissearch LIKE :filter1 ") +			// track title is like filter

								"UNION SELECT t3.trackid "+			// album title is like filter
								"FROM tracks t3, albums "+
								"WHERE albums.albumid = t3.albumid AND albums.cissearch LIKE :filter2 "+

								"UNION SELECT t4.trackid " +		// artist title is like filter
								"FROM tracks t4, albums, artists " +
								"WHERE t4.albumid = albums.albumid AND t4.artistid = artists.artistid AND artists.cissearch LIKE :filter3 "
							") ";
				break;
		}
	}

	querytext += QString("GROUP BY albums.albumID, albumName ");
	querytext += _create_order_string(sortorder) + ";";

	q.prepare(querytext);

	q.bindValue(":artist_id", QVariant(artists[0]));
	for(int i=1; i<artists.size(); i++) {
		q.bindValue(QString(":artist_id_") + QString::number(i), artists[i]);
	}

	if(filter.filtertext.length() > 0) {

		q.bindValue(":filter1", QVariant(filter.filtertext));

		switch(filter.mode) {

			case Filter::Mode::Fulltext:
				q.bindValue(":filter2", QVariant(filter.filtertext));
				q.bindValue(":filter3", QVariant(filter.filtertext));
				break;
			default:
				break;
		}
	}


	return db_fetch_albums(q, result);

}

bool DatabaseAlbums::getAllAlbumsByArtist(int artist, AlbumList& result, Filter filter, SortOrder sortorder) {

	IDList list;
	list << artist;
	return getAllAlbumsByArtist(list, result, filter, sortorder);
}

bool DatabaseAlbums::getAllAlbumsBySearchString(Filter filter, AlbumList& result, SortOrder sortorder) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	QString query;
	switch(filter.mode){

		case Filter::Mode::Genre:
			query = _fetch_query +
						 "WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.genre LIKE :search_in_genre " +
						 "GROUP BY albums.albumID, albumName";
			break;

		case Filter::Mode::Filename:
			query = _fetch_query +
						"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.filename LIKE :search_in_filename " +
						"GROUP BY albums.albumID, albumName";
			break;

		case Filter::Mode::Fulltext:
		default:
			query = QString("SELECT DISTINCT * FROM ( ") +
					_fetch_query +
						"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND albums.cissearch LIKE :search_in_album " +
						"GROUP BY albums.albumid, albums.name " +
					"UNION " +
					_fetch_query +
						"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.cissearch LIKE :search_in_title " +
						"GROUP BY albums.albumid, albums.name " +
					"UNION " +
					_fetch_query +
						"WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND artists.cissearch LIKE :search_in_artist " +
						"GROUP BY albums.albumid, albums.name " +
					"" +
				") " +
				"GROUP BY albumID, albumName";
			break;
	}


	query += _create_order_string(sortorder) + ";";

	q.prepare(query);


	switch(filter.mode){

		case Filter::Mode::Genre:
			q.bindValue(":search_in_genre", QVariant(filter.filtertext));
			break;

		case Filter::Mode::Filename:
			q.bindValue(":search_in_filename", QVariant(filter.filtertext));
			break;

		case Filter::Mode::Fulltext:
		default:
			q.bindValue(":search_in_title",QVariant(filter.filtertext));
			q.bindValue(":search_in_album",QVariant(filter.filtertext));
			q.bindValue(":search_in_artist",QVariant(filter.filtertext));
			break;

	}

	return db_fetch_albums(q, result);
}


int DatabaseAlbums::updateAlbum (const Album & album) {
	SayonaraQuery q (_db);

	q.prepare("UPDATE albums "
			  "SET name=:name, "
			  "    cissearch=:cissearch, "
			  "    rating=:rating "
			  "WHERE albumID = :id;");

	q.bindValue(":id", album.id);
	q.bindValue(":name", album.name);
	q.bindValue(":cissearch", album.name.toLower());
	q.bindValue(":rating", album.rating);


	if (!q.exec()) {
		q.show_error(QString("Cannot update album ") + album.name);
		return -1;
	}

	return getAlbumID (album.name);
}

void DatabaseAlbums::updateAlbumCissearch(LibraryHelper::SearchModeMask mode)
{
	AlbumList albums;
	getAllAlbums(albums);

	_db.transaction();
	for(const Album& album : albums) {
		QString str = "UPDATE albums SET cissearch=:cissearch WHERE albumID=:id;";
		SayonaraQuery q(_db);
		q.prepare(str);
		q.bindValue(":cissearch", LibraryHelper::convert_search_string(album.name, mode));
		q.bindValue(":id", album.id);

		if(!q.exec()){
			q.show_error("Cannot update album cissearch");
		}
	}

	_db.commit();
}

int DatabaseAlbums::insertAlbumIntoDatabase (const QString& album) {

	SayonaraQuery q (_db);

	int album_id = getAlbumID(album);
	if(album_id >= 0){
		Album a;
		getAlbumByID(album_id, a);
		return updateAlbum(a);
	}

	q.prepare("INSERT INTO albums (name, cissearch) values (:album, :cissearch);");
	q.bindValue(":album", album);
	q.bindValue(":cissearch", album.toLower());

	if (!q.exec()) {
		q.show_error(QString("Cannot insert album ") + album + " to db");
		return -1;
	}

	return getAlbumID(album);
}

int DatabaseAlbums::insertAlbumIntoDatabase (const Album& album) {

	if(album.id >= 0){
		return updateAlbum(album);
	}

	SayonaraQuery q (_db);

	q.prepare("INSERT INTO albums (name, cissearch, rating) values (:name, :cissearch, :rating);");

	q.bindValue(":name", album.name);
	q.bindValue(":cissearch", album.name.toLower());
	q.bindValue(":rating", album.rating);

	if (!q.exec()) {
		q.show_error("SQL: Cannot insert album into database");
		return -1;
	}

	return album.id;
}

