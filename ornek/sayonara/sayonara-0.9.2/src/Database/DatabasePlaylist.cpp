/* DatabasePlaylist.cpp */

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
#include "Database/DatabasePlaylist.h"

DatabasePlaylist::DatabasePlaylist(QSqlDatabase db, quint8 db_id) :
	DatabaseModule(db, db_id)
{
}


bool DatabasePlaylist::getAllPlaylistSkeletons(CustomPlaylistSkeletons& skeletons, DatabasePlaylist::PlaylistChooserType type, SortOrderPlaylists sortorder) {

	skeletons.clear();

	QString sortorder_str;
	switch(sortorder){
		case SortOrderPlaylists::IDAsc:
			sortorder_str = " ORDER BY playlists.playlistID ASC ";
			break;
		case SortOrderPlaylists::IDDesc:
			sortorder_str = " ORDER BY playlists.playlistID DESC ";
			break;
		case SortOrderPlaylists::NameAsc:
			sortorder_str = " ORDER BY playlists.playlist ASC ";
			break;
		case SortOrderPlaylists::NameDesc:
			sortorder_str = " ORDER BY playlists.playlist DESC ";
			break;
		default:
			break;
	}

	QString type_clause;
	switch(type){
		case DatabasePlaylist::PlaylistChooserType::OnlyTemporary:
			type_clause = " WHERE playlists.temporary = 1 ";
			break;
		case DatabasePlaylist::PlaylistChooserType::OnlyPermanent:
			type_clause = " WHERE playlists.temporary = 0 ";
			break;
		default:
			break;

	}

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q(_db);

	QString querytext = QString() +

			"SELECT "
			"playlists.playlistID, "
			"playlists.playlist, "
			"playlists.temporary, "
			"COUNT(playlisttotracks.trackID) "
			"FROM playlists LEFT OUTER JOIN playlisttotracks "
			"ON playlists.playlistID = playlisttotracks.playlistID "
			+ type_clause +
			"GROUP BY playlists.playlistID " +
			sortorder_str +
			";";

	q.prepare(querytext);

	if(!q.exec()) {
		q.show_error("Cannot fetch all playlists");
		return false;
	}

	while(q.next()) {
		CustomPlaylistSkeleton skeleton;
		if(q.value(0).isNull()){
			continue;
		}

		skeleton.id = q.value(0).toInt();
		skeleton.name =	q.value(1).toString();
		skeleton.is_temporary =	(q.value(2).toInt() == 1) ? true : false;
		skeleton.n_tracks = q.value(3).toInt();
		skeletons << skeleton;
	}

	return true;
}

bool DatabasePlaylist::getPlaylistSkeletonById(CustomPlaylistSkeleton& skeleton){
	DB_RETURN_NOT_OPEN_BOOL(_db);

	if(skeleton.id < 0){
		sp_log(Log::Warning) << "Cannot fetch playlist -1";
		return false;
	}

	SayonaraQuery q(_db);

	QString querytext = QString() +

			"SELECT "
			"playlists.playlistID, "
			"playlists.playlist, "
			"playlists.temporary, "
			"COUNT(playlisttotracks.trackID) "
			"FROM playlists LEFT OUTER JOIN playlisttotracks "
			"ON playlists.playlistID = playlisttotracks.playlistID "
			"WHERE playlists.playlistid = :playlist_id "
			"GROUP BY playlists.playlistID;";

	q.prepare(querytext);
	q.bindValue(":playlist_id", skeleton.id);

	if(!q.exec()) {
		q.show_error("Cannot fetch all playlists");
		return false;
	}

	if(q.next()) {
		skeleton.id = q.value(0).toInt();
		skeleton.name =	q.value(1).toString();
		skeleton.is_temporary =	(q.value(2).toInt() == 1) ? true : false;
		skeleton.n_tracks = q.value(3).toInt();
		return true;
	}

	return false;
}

bool DatabasePlaylist::getPlaylistById(CustomPlaylist& pl) {

	if(!getPlaylistSkeletonById(pl)){
		sp_log(Log::Warning) << "Get playlist by id: cannot fetch skeleton id " << pl.id;
		return false;
	}

	DB_RETURN_NOT_OPEN_BOOL(_db);

	pl.tracks.clear();

	SayonaraQuery q(_db);

	QString querytext = QString("SELECT ") +

			"tracks.trackID AS trackID, "				// 0
			"tracks.title AS title, "					// 1
			"tracks.length AS length, "					// 2
			"tracks.year AS year, "						// 3
			"tracks.bitrate AS bitrate, "				// 4
			"tracks.filename AS filename, "				// 5
			"tracks.track AS trackNum, "				// 6
			"albums.albumID AS albumID, "				// 7
			"artists.artistID AS artistID, "			// 8
			"albums.name AS albumName, "				// 9
			"artists.name AS artistName, "				// 10
			"tracks.genre AS genrename, "				// 11
			"tracks.filesize AS filesize, "				// 12
			"tracks.discnumber AS discnumber, "			// 13
			"tracks.rating AS rating, "					// 14
			"playlistToTracks.filepath AS filepath, "	// 15
			"playlistToTracks.db_id AS db_id "			// 16

			"FROM tracks, albums, artists, playlists, playlisttotracks "
			"WHERE playlists.playlistID = :playlist_id "
			"AND playlists.playlistID = playlistToTracks.playlistID "
			"AND playlistToTracks.trackID = tracks.trackID "
			"AND tracks.albumID = albums.albumID "
			"AND tracks.artistID = artists.artistID "
			"ORDER BY playlistToTracks.position ASC; ";

	q.prepare(querytext);
	q.bindValue(":playlist_id", pl.id);

	if (!q.exec()) {
		q.show_error(QString("Cannot get tracks for playlist ") + QString::number(pl.id));
	}

	else{
		while (q.next()) {
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
			data.rating = q.value(14).toInt();

			data.is_extern = false;
			data.db_id = _module_db_id;

			if(q.value(16).toInt() == 0 || q.value(16).isNull()){
				pl.tracks.push_back(data);
			}
		}
	}

	// non database tracks
	QString querytext2 = QString()+
			"SELECT "
			"playlisttotracks.filepath AS filepath, "
			"playlisttotracks.position AS position "
			"FROM playlists, playlisttotracks "
			"WHERE playlists.playlistID = :playlist_id "
			"AND playlists.playlistID =  playlistToTracks.playlistID "
			"AND playlistToTracks.trackID <= 0 "
			"ORDER BY playlistToTracks.position ASC; ";

	SayonaraQuery q2(_db);
	q2.prepare(querytext2);
	q2.bindValue(":playlist_id", pl.id);

	if(!q2.exec()) {
		q2.show_error(QString("Playlist by id: Cannot fetch playlist ") + QString::number(pl.id));
		return false;
	}

	while (q2.next()) {

		int position = q2.value(1).toInt();

		MetaData data;
		QString filepath = q2.value(0).toString();
		data.set_filepath( filepath );
		data.id = -1;
		data.is_extern = true;
		data.db_id = _module_db_id;
		data.title = filepath;
		data.artist = filepath;

		for(int row=0; row<=pl.tracks.size(); row++) {
			if( row >= position) {
				pl.tracks.insert(row, data);
				break;
			}
		}
	}

	return true;
}

// negative, if error
// nonnegative else
int DatabasePlaylist::getPlaylistIdByName(const QString& name) {

	DB_RETURN_NOT_OPEN_INT(_db);

	SayonaraQuery q(_db);

	q.prepare("SELECT playlistid FROM playlists WHERE playlist = :playlist_name;");
	q.bindValue(":playlist_name", name);

	if(!q.exec()) {
		q.show_error(QString("Playlist by name: Cannot fetch playlist ") + name);
		return -1;
	}

	else {
		  if(q.next()) {
			  return q.value(0).toInt();
		  }

		  return -1;
	}
}


bool DatabasePlaylist::insertTrackIntoPlaylist(const MetaData& md, int playlist_id, int pos) {

	DB_RETURN_NOT_OPEN_BOOL(_db);
	if(md.is_disabled)
	{
		return false;
	}

	SayonaraQuery q (_db);

	QString query_string = QString("INSERT INTO playlisttotracks ") +
							"(trackid, playlistid, position, filepath, db_id) " +
							"VALUES " +
							"(:track_id, :playlist_id, :position, :filepath, :db_id);";


	q.prepare(query_string);
	q.bindValue(":track_id", md.id);
	q.bindValue(":playlist_id", playlist_id);
	q.bindValue(":position", pos);
	q.bindValue(":filepath", md.filepath());
	q.bindValue(":db_id", md.db_id);

	if (!q.exec()) {
		q.show_error("Cannot insert track into playlist");
		return false;
	}

	return true;
}


// returns id if everything ok
// negative otherwise
int DatabasePlaylist::createPlaylist(QString playlist_name, bool temporary) {

	DB_RETURN_NOT_OPEN_INT(_db);


	int temporary_int = (temporary) ? 1 : 0;

	QString query_string = "INSERT INTO playlists (playlist, temporary) VALUES (:playlist_name, :temporary);";

	SayonaraQuery q(_db);

	q.prepare(query_string);
	q.bindValue(":playlist_name", QVariant(playlist_name));
	q.bindValue(":temporary", QVariant(temporary_int));

	if(!q.exec()) {
		q.show_error("Cannot create playlist");
		return -1;
	}

	return q.lastInsertId().toInt();

}

bool DatabasePlaylist::renamePlaylist(int id, const QString& new_name){
	DB_RETURN_NOT_OPEN_BOOL(_db);

	QString query_string = "UPDATE playlists SET playlist=:playlist_name WHERE playlistId=:id;";

	SayonaraQuery q(_db);

	q.prepare(query_string);
	q.bindValue(":playlist_name", new_name);
	q.bindValue(":id", id);

	if(!q.exec()) {
		q.show_error("Cannot rename playlist");
		return false;
	}

	return true;
}

bool DatabasePlaylist::updatePlaylistTempState(int playlist_id, bool temporary){

	DB_RETURN_NOT_OPEN_BOOL(_db);

	int temporary_int = (temporary) ? 1 : 0;

	SayonaraQuery q(_db);
	QString query_string = QString("UPDATE playlists SET temporary=:temporary WHERE playlistID = :playlist_id;");
	q.prepare(query_string);
	q.bindValue(":playlist_id", playlist_id);
	q.bindValue(":temporary", temporary_int);

	if(!q.exec()){
		q.show_error("Cannot update playlist");
		return false;
	}

	return true;
}

bool DatabasePlaylist::storePlaylist(const MetaDataList& vec_md, QString playlist_name, bool temporary) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	int playlist_id;

	if(playlist_name.isEmpty()){
		return false;
	}

	if(playlist_name.isEmpty()){
		sp_log(Log::Warning) << "Try to save empty playlist";
		return false;
	}

	playlist_id = getPlaylistIdByName(playlist_name);
	if(playlist_id >= 0) {
		emptyPlaylist(playlist_id);
	}

	else {
		playlist_id = createPlaylist(playlist_name, temporary);
		if( playlist_id < 0) {
			return false;
		}
	}

	// fill playlist
	for(int i=0; i<vec_md.size(); i++) {

		bool success = insertTrackIntoPlaylist(vec_md[i], playlist_id, i);

		if( !success ) {
			return false;
		}
	}

	return true;
}



bool DatabasePlaylist::storePlaylist(const MetaDataList& vec_md, int playlist_id, bool temporary) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	CustomPlaylist pl;
	pl.id = playlist_id;

	bool success = getPlaylistById(pl);
	if(!success){
		sp_log(Log::Warning) << "Store: Cannot fetch playlist: " << pl.id;
		return false;
	}

	if(pl.name.isEmpty()){
		return false;
	}

	if( playlist_id < 0) {
		playlist_id = createPlaylist(pl.name, temporary);
	}

	else{
		emptyPlaylist(playlist_id);
	}

	// fill playlist
	for(int i=0; i<vec_md.size(); i++) {
		bool success = insertTrackIntoPlaylist(vec_md[i], playlist_id, i);

		if( !success ) {
			return false;
		}
	}

	return true;
}

bool DatabasePlaylist::emptyPlaylist(int playlist_id) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q(_db);
	QString querytext = QString("DELETE FROM playlistToTracks WHERE playlistID = :playlist_id;");
	q.prepare(querytext);
	q.bindValue(":playlist_id", playlist_id);

	if(!q.exec()) {
		q.show_error("DB: Playlist cannot be cleared");
		return false;
	}

	return true;
}


bool DatabasePlaylist::deletePlaylist(int playlist_id) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	emptyPlaylist(playlist_id);

	SayonaraQuery q(_db);
	QString querytext = QString("DELETE FROM playlists WHERE playlistID = :playlist_id;");

	q.prepare(querytext);
	q.bindValue(":playlist_id", playlist_id);

	if(!q.exec()){
		q.show_error(QString("Cannot delete playlist ") + QString::number(playlist_id));
		return false;
	}

	return true;
}


bool DatabasePlaylist::deleteTrackFromPlaylists(int track_id) {

	DB_TRY_OPEN(_db);
	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q(_db);
	QString querytext = QString("DELETE FROM playlistToTracks WHERE trackID = :track_id;");

	q.prepare(querytext);
	q.bindValue(":track_id", track_id);

	if(!q.exec()){
		q.show_error(QString("Cannot delete track ") + QString::number(track_id) + "from playlist");
		return false;
	}

	return true;
}
