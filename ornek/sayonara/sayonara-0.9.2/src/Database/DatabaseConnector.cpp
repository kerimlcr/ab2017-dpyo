/* DatabaseConnector.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General License for more details.

 * You should have received a copy of the GNU General License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Database/SayonaraQuery.h"
#include "Database/DatabaseConnector.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"

DatabaseConnector::DatabaseConnector() :

	LibraryDatabase(0, "", QString("player.db")),
	DatabaseBookmarks(_database, _db_id),
	DatabasePlaylist(_database, _db_id),
	DatabasePodcasts(_database, _db_id),
	DatabaseSettings(_database, _db_id),
	DatabaseStreams(_database, _db_id),
	DatabaseVisStyles(_database, _db_id)
{
	apply_fixes();
}


DatabaseConnector::~DatabaseConnector() {

}

bool DatabaseConnector::updateAlbumCissearchFix() {

#ifdef DEBUG_DB
	sp_log(Log::Debug) << Q_FUNC_INFO;
#endif

	AlbumList albums;
	getAllAlbums(albums);
	for(const Album& album : albums) {
		QString str = "UPDATE albums SET cissearch=:cissearch WHERE albumID=:id;";
		SayonaraQuery q(_database);
		q.prepare(str);
		q.bindValue(":cissearch", album.name.toLower());
		q.bindValue(":id", album.id);

		if(!q.exec()){
			q.show_error("Cannot update album cissearch");
		}
	}

	return true;
}


bool DatabaseConnector::updateArtistCissearchFix() {

	ArtistList artists;
	getAllArtists(artists);
	for(const Artist& artist : artists) {
		QString str = "UPDATE artists SET cissearch=:cissearch WHERE artistID=:id;";
		SayonaraQuery q(_database);
		q.prepare(str);
		q.bindValue(":cissearch", artist.name.toLower());
		q.bindValue(":id", artist.id);

		if(!q.exec()){
			q.show_error("Cannot update artist cissearch");
		}
	}

	return true;
}

bool DatabaseConnector::updateTrackCissearchFix() {

	MetaDataList v_md;
	getTracksFromDatabase(v_md);
	for(const MetaData& md : v_md) {
		updateTrack(md);
	}

	return true;
}

bool DatabaseConnector::apply_fixes() {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	QString str_version;
	int version;
	bool success;
	const int LatestVersion = 11;

	success = load_setting("version", str_version);
	version = str_version.toInt(&success);
	sp_log(Log::Info) << "Database Version " << version;

	if(version == LatestVersion) {
		sp_log(Log::Warning) << "No need to update db";
		return true;
	}

	else if(!success){
		 sp_log(Log::Warning) << "Cannot get database version";
	}

	sp_log(Log::Info) << "Apply fixes";

	if(version < 1) {

		check_and_insert_column("playlisttotracks", "position", "INTEGER");
		check_and_insert_column("playlisttotracks", "filepath", "VARCHAR(512)");
		check_and_insert_column("tracks", "genre", "VARCHAR(1024)");

		QString create_savedstreams = QString("CREATE TABLE savedstreams ") +
				"( " +
				"	name VARCHAR(255) PRIMARY KEY, " +
				"	url VARCHAR(255) " +
				");";

		check_and_create_table("savedstreams", create_savedstreams);


		QString create_savedpodcasts = QString("CREATE TABLE savedpodcasts ") +
				"( " +
				"	name VARCHAR(255) PRIMARY KEY, " +
				"	url VARCHAR(255) " +
				");";

		check_and_create_table("savedpodcasts", create_savedpodcasts);
	}

	if(version < 3) {
		_database.transaction();

		bool success = true;
		success &= check_and_insert_column("tracks", "cissearch", "VARCHAR(512)");
		success &= check_and_insert_column("albums", "cissearch", "VARCHAR(512)");
		success &= check_and_insert_column("artists", "cissearch", "VARCHAR(512)");

		Q_UNUSED(success)

		updateAlbumCissearchFix();
		updateArtistCissearchFix();
		updateTrackCissearchFix();

		_database.commit();
	}


	if(version == 3) {
		check_and_drop_table("VisualStyles");
	}

	if(version < 4) {

		QString create_vis_styles = QString("CREATE TABLE VisualStyles ") +
				"( " +
				"  name VARCHAR(255) PRIMARY KEY, " +
				"  col1 VARCHAR(20), "
				"  col2 VARCHAR(20), "
				"  col3 VARCHAR(20), "
				"  col4 VARCHAR(20), "
				"  nBinsSpectrum INTEGER, "
				"  rectHeightSpectrum INTEGER, "
				"  fadingStepsSpectrum INTEGER, "
				"  horSpacingSpectrum INTEGER, "
				"  vertSpacingSpectrum INTEGER, "
				"  rectWidthLevel INTEGER, "
				"  rectHeightLevel INTEGER, "
				"  horSpacingLevel INTEGER, "
				"  verSpacingLevel INTEGER, "
				"  fadingStepsLevel INTEGER "
				");";

		bool success = check_and_create_table("VisualStyles", create_vis_styles);
		if(success) store_setting("version", 4);
	}

	if(version < 5) {
		bool success = check_and_insert_column("tracks", "rating", "integer");
		if(success) store_setting("version", 5);
	}

	if(version < 6) {
		QString create_savedbookmarks = QString("CREATE TABLE savedbookmarks ") +
				"( " +
				"	trackid INTEGER, " +
				"	name VARCHAR(255), " +
				"	timeidx INTEGER, " +
				"   PRIMARY KEY (trackid, timeidx), " +
				"   FOREIGN KEY (trackid) REFERENCES tracks(trackid) " +
				");";

		bool success = check_and_create_table("savedbookmarks", create_savedbookmarks);
		if(success) store_setting("version", 6);
	}

	if(version < 7) {
		bool success = check_and_insert_column("albums", "rating", "integer");
		if(success) store_setting("version", 7);
	}

	if(version < 9) {
		bool success = check_and_insert_column("playlists", "temporary", "integer");

		if(success) {

			SayonaraQuery q (_database);
			QString querytext = "UPDATE playlists SET temporary=0;";
			q.prepare(querytext);
			if(q.exec()){
				store_setting("version", 9);
			};
		}
	}

	if(version < 10){
		bool success = check_and_insert_column("playlisttotracks", "db_id", "integer");
		if(success) {

			SayonaraQuery q (_database);
			SayonaraQuery q_index(_database);
			QString querytext = "UPDATE playlisttotracks SET db_id = (CASE WHEN trackid > 0 THEN 0 ELSE -1 END)";
			QString index_query = "CREATE INDEX album_search ON albums(cissearch, albumID);"
					"CREATE INDEX artist_search ON artists(cissearch, artistID);"
					"CREATE INDEX track_search ON tracks(cissearch, trackID);";

			q.prepare(querytext);
			q_index.prepare(index_query);

			if(q.exec()){
				store_setting("version", 10);
			};

			q_index.exec();
		}
	}

	if(version < 11)
	{
		// look in UpdateDatesThread
	}


	return true;
}


void DatabaseConnector::clean_up()
{
	DB_RETURN_NOT_OPEN_VOID(_database);

	SayonaraQuery q (_database);
	QString querytext = "VACUUM;";
	q.prepare(querytext);
	q.exec();
}
