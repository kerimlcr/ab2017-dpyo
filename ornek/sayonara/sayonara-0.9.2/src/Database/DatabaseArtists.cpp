/* DatabaseArtists.cpp */

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
#include "Database/DatabaseArtists.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/MetaData/Artist.h"
#include "Helper/Filter.h"

DatabaseArtists::DatabaseArtists(QSqlDatabase db, quint8 db_id) :
	DatabaseModule(db, db_id)
{
	_fetch_query = 	QString("SELECT ") +
					"artists.artistid AS artistID, "
					"artists.name AS artistName, "
					"COUNT(DISTINCT tracks.trackid) AS artistNTracks, "
					"GROUP_CONCAT(DISTINCT albums.albumid) AS artistAlbums "
					"FROM artists, albums, tracks ";
}

void DatabaseArtists::set_artist_fetch_query(const QString &query){
	_fetch_query = query;
}

bool DatabaseArtists::db_fetch_artists(SayonaraQuery& q, ArtistList& result) {

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

		artist.id = q.value(0).toInt();
		artist.name = q.value(1).toString().trimmed();
		artist.num_songs = q.value(2).toInt();

		QStringList list = q.value(3).toString().split(',');		
		artist.num_albums = list.size();
		artist.db_id = _module_db_id;

		result << artist;
	}

	return true;
}

QString DatabaseArtists::_create_order_string(SortOrder sort) {

	switch(sort) {
		case SortOrder::ArtistNameAsc:
			return QString(" ORDER BY artistName ASC ");
		case SortOrder::ArtistNameDesc:
			return QString(" ORDER BY artistName DESC ");
		case SortOrder::ArtistTrackcountAsc:
			return QString(" ORDER BY artistNTracks ASC, artistName ASC ");
		case SortOrder::ArtistTrackcountDesc:
			return QString(" ORDER BY artistNTracks DESC, artistName DESC ");
		default:
			return  "";
	}
}

int DatabaseArtists::getMaxArtistID() {

#ifdef DEBUG_DB
	sp_log(Log::Debug) << Q_FUNC_INFO;
#endif

	DB_RETURN_NOT_OPEN_INT(_db);

	int max_id = -1;

	SayonaraQuery q (_db);
	q.prepare("SELECT MAX(artistID) FROM artists;");

	if (!q.exec()) {
		q.show_error("Cannot get max artist id");
		return -1;
	}

	while (q.next()) {
		max_id = q.value(0).toInt();
	}

	return max_id;
}

bool DatabaseArtists::getArtistByID(int id, Artist& artist) {

	if(id == -1) return false;

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);

	ArtistList artists;

	QString query = _fetch_query +
				"WHERE artists.artistID = ? " +
				"AND tracks.artistID = artists.artistID " +
				"AND tracks.albumID = albums.albumID " +
				"GROUP BY artistName;";

	q.prepare(query);
	q.addBindValue(QVariant (id));

	bool success = db_fetch_artists(q, artists);

	if(artists.size() > 0) {
		success = true;
		artist = artists[0];
	}

	else success = false;

	return success;
}

int DatabaseArtists::getArtistID (const QString & artist)  {

	DB_RETURN_NOT_OPEN_INT(_db);

	SayonaraQuery q (_db);
	int artistID = -1;
	q.prepare("select artistID from artists where name == ?;");
	q.addBindValue(artist);

	if (!q.exec()) {
		return -1;
	}

	if (q.next()) {
		artistID = q.value(0).toInt();
	}

	return artistID;
}

bool DatabaseArtists::getAllArtists(ArtistList& result, SortOrder sortorder, bool also_empty) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	QString query = _fetch_query;

	if(!also_empty){
			query += "WHERE Tracks.albumID = albums.albumID AND artists.artistid = tracks.artistid ";
	}

	query += "GROUP BY artists.artistID, artists.name ";

	query += _create_order_string(sortorder) + ";";

	q.prepare(query);

	return db_fetch_artists(q, result);

}

bool DatabaseArtists::getAllArtistsByAlbum(int album, ArtistList& result, SortOrder sortorder) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	q.prepare(	_fetch_query +
				"WHERE Tracks.albumID = albums.albumID " +
				"AND artists.artistid = tracks.artistid " +
				"AND albums.albumid=" + QString::number(album) + " " +
				"GROUP BY artists.artistID, artists.name " +
				_create_order_string(sortorder) + "; ");

	return db_fetch_artists(q, result);
}

bool DatabaseArtists::getAllArtistsBySearchString(Filter filter, ArtistList& result, SortOrder sortorder) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	QString query;

	switch(filter.mode) {

		case Filter::Mode::Genre:
			query = _fetch_query +
							"	WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.genre LIKE :search_in_genre " +
							"	GROUP BY artists.artistid, artists.name ";
			break;

		case Filter::Mode::Filename:
			query = _fetch_query +
							"	WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.filename LIKE :search_in_filename " +
							"	GROUP BY artists.artistid, artists.name ";
			break;

		case Filter::Mode::Fulltext:
		default:
			query = QString("SELECT * FROM ( ") +
					_fetch_query +
			"			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND artists.cissearch LIKE :search_in_artist " +
			"			GROUP BY artists.artistid, artists.name " +
			"		UNION " +
					_fetch_query +
			"			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND albums.cissearch LIKE :search_in_album " +
			"			GROUP BY artists.artistid, artists.name " +
			"		UNION " +
					_fetch_query +
			"			WHERE albums.albumid = tracks.albumid AND artists.artistID = tracks.artistid AND tracks.cissearch LIKE :search_in_title " +
			"			GROUP BY artists.artistid, artists.name " +
			"	)  " +
			"	GROUP BY artistID, artistName ";
			break;
	}


	query += _create_order_string(sortorder) + ";";

	q.prepare(query);
	switch(filter.mode) {

		case Filter::Mode::Genre:
			q.bindValue(":search_in_genre", QVariant(filter.filtertext));
			break;
		case Filter::Mode::Filename:
			q.bindValue(":search_in_filename",QVariant(filter.filtertext));
			break;

		case Filter::Mode::Fulltext:
		default:
			q.bindValue(":search_in_title",QVariant(filter.filtertext));
			q.bindValue(":search_in_album",QVariant(filter.filtertext));
			q.bindValue(":search_in_artist",QVariant(filter.filtertext));
			break;
	}

	return db_fetch_artists(q, result);
}


int DatabaseArtists::insertArtistIntoDatabase (const QString& artist) {

	DB_RETURN_NOT_OPEN_INT(_db);

	int id = getArtistID(artist);
	if(id >= 0){
		return id;
	}

	SayonaraQuery q (_db);
	q.prepare("INSERT INTO artists (name, cissearch) values (:artist, :cissearch);");
	q.bindValue(":artist", artist);
	q.bindValue(":cissearch", artist.toLower());

	if (!q.exec()) {
		q.show_error(QString("Cannot insert artist ") + artist);
		return -1;
	}

	return getArtistID(artist);
}

int DatabaseArtists::insertArtistIntoDatabase (const Artist & artist) {

	DB_RETURN_NOT_OPEN_INT(_db);

	if(artist.id >= 0){
		updateArtist(artist);
		return artist.id;
	}

	return insertArtistIntoDatabase(artist.name);

}


int DatabaseArtists::updateArtist(const Artist &artist){
	DB_RETURN_NOT_OPEN_INT(_db);

	SayonaraQuery q (_db);

	if(artist.id < 0) return -1;

	q.prepare("UPDATE artists SET name = :name, cissearch = :cissearch WHERE artistid = :artist_id;");
	q.bindValue(":name", artist.name);
	q.bindValue(":cissearch", artist.name.toLower());
	q.bindValue(":artist_id", artist.id);

	if (!q.exec()) {
		q.show_error(QString("Cannot insert (2) artist ") + artist.name);
		return -1;
	}

	return artist.id;
}

void DatabaseArtists::updateArtistCissearch(LibraryHelper::SearchModeMask mode)
{
	ArtistList artists;
	getAllArtists(artists);

	_db.transaction();
	for(const Artist& artist : artists) {
		QString str = "UPDATE artists SET cissearch=:cissearch WHERE artistID=:id;";
		SayonaraQuery q(_db);
		q.prepare(str);
		q.bindValue(":cissearch", LibraryHelper::convert_search_string(artist.name, mode));
		q.bindValue(":id", artist.id);

		if(!q.exec()){
			q.show_error("Cannot update artist cissearch");
		}
	}
	_db.commit();
}

