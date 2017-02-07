/* DatabaseTracks.cpp */

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
#include "Database/DatabaseTracks.h"
#include "Database/DatabaseLibrary.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"

#include <QDateTime>
#include <QMap>

#include <utility>
#include <tuple>

DatabaseTracks::DatabaseTracks(QSqlDatabase db, quint8 db_id) :
	DatabaseModule(db, db_id)
{
	_fetch_query = QString("SELECT ") +
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
			"tracks.rating AS rating "
			"FROM tracks "
			"INNER JOIN albums ON tracks.albumID = albums.albumID "
			"INNER JOIN artists ON tracks.artistID = artists.artistID "
			"WHERE filetype is null ";
}




bool DatabaseTracks::db_fetch_tracks(SayonaraQuery& q, MetaDataList& result) {

	result.clear();

	//sp_log(Log::Debug) << q.get_query_string();
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
		data.rating = q.value(14).toInt();
		data.db_id = _module_db_id;

		result << data;
	}

	return true;
}

void DatabaseTracks::set_track_fetch_query(const QString &query){
	_fetch_query = query;
}


QString DatabaseTracks::append_track_sort_string(QString querytext, SortOrder sort) {


	if(sort == SortOrder::TrackArtistAsc) querytext += QString(" ORDER BY artistName ASC, discnumber ASC, albumName ASC, trackNum;");
	else if(sort == SortOrder::TrackArtistDesc) querytext += QString(" ORDER BY artistName DESC, discnumber ASC, albumName ASC, trackNum;");
	else if(sort == SortOrder::TrackAlbumAsc) querytext += QString(" ORDER BY discnumber ASC, albumName ASC, trackNum;");
	else if(sort == SortOrder::TrackAlbumDesc) querytext += QString(" ORDER BY discnumber ASC, albumName DESC, trackNum;");
	else if(sort == SortOrder::TrackTitleAsc) querytext += QString(" ORDER BY trackTitle ASC;");
	else if(sort == SortOrder::TrackTitleDesc) querytext += QString(" ORDER BY trackTitle DESC;");
	else if(sort == SortOrder::TrackNumAsc) querytext += QString(" ORDER BY trackNum ASC;");
	else if(sort == SortOrder::TrackNumDesc) querytext += QString(" ORDER BY trackNum DESC;");
	else if(sort == SortOrder::TrackYearAsc) querytext += QString(" ORDER BY trackYear ASC;");
	else if(sort == SortOrder::TrackYearDesc) querytext += QString(" ORDER BY trackYear DESC;");
	else if(sort == SortOrder::TrackLenghtAsc) querytext += QString(" ORDER BY trackLength ASC;");
	else if(sort == SortOrder::TrackLengthDesc) querytext += QString(" ORDER BY trackLength DESC;");
	else if(sort == SortOrder::TrackBitrateAsc) querytext += QString(" ORDER BY trackBitrate ASC;");
	else if(sort == SortOrder::TrackBitrateDesc) querytext += QString(" ORDER BY trackBitrate DESC;");
	else if(sort == SortOrder::TrackSizeAsc) querytext += QString(" ORDER BY filesize ASC;");
	else if(sort == SortOrder::TrackSizeDesc) querytext += QString(" ORDER BY filesize DESC;");
	else if(sort == SortOrder::TrackRatingAsc) querytext += QString(" ORDER BY rating ASC;");
	else if(sort == SortOrder::TrackRatingDesc) querytext += QString(" ORDER BY rating DESC;");


	else querytext += ";";

	return querytext;
}


bool DatabaseTracks::getMultipleTracksByPath(const QStringList& paths, MetaDataList& v_md) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	_db.transaction();

	for(const QString& path : paths) {
		v_md << getTrackByPath(path);
	}

	_db.commit();

	return (v_md.size() == paths.size());
}


MetaData DatabaseTracks::getTrackByPath(const QString& path) {

	DB_TRY_OPEN(_db);

	MetaDataList vec_data;

	SayonaraQuery q (_db);

	QString querytext = _fetch_query + " AND tracks.filename = :filename;";
	q.prepare(querytext);
	q.bindValue(":filename", path);

	MetaData md;
	md.id = -1;
	md.set_filepath(path);
	md.db_id = _module_db_id;

	if(!db_fetch_tracks(q, vec_data)) return md;

	if(vec_data.size() == 0) {
		md.is_extern = true;

		return md;
	}

	return vec_data[0];
}

struct Blupp{
	int idx;
	Blupp(){
		idx = -1;
	}
	Blupp(int i){
		idx = i;
	}
};

MetaData DatabaseTracks::getTrackById(int id) {

	DB_TRY_OPEN(_db);

	MetaDataList vec_data;

	SayonaraQuery q (_db);
	QString querytext = _fetch_query + " AND tracks.trackID = :track_id;";

	q.prepare(querytext);
	q.bindValue(":track_id", QVariant(id));

	MetaData md;
	md.id = -1;

	if(!db_fetch_tracks(q, vec_data)) return md;

	if(vec_data.size() == 0) {
		md.is_extern = true;
		return md;
	}

	return vec_data[0];
}


bool DatabaseTracks::getTracksFromDatabase (MetaDataList & returndata, SortOrder sort) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);

	QString querytext = append_track_sort_string(_fetch_query, sort);
	q.prepare(querytext);

	return db_fetch_tracks(q, returndata);
}

bool DatabaseTracks::getAllTracksByAlbum(int album, MetaDataList& returndata, Filter filter, SortOrder sort, int discnumber) {

	bool success;
	IDList list;
	MetaDataList v_md;

	list << album;
	returndata.clear();

	success = getAllTracksByAlbum(list, v_md, filter, sort);

	if(discnumber < 0) {
		returndata = v_md;
	}

	for(const MetaData& md : v_md) {
		if(discnumber != md.discnumber) {
			continue;
		}

		returndata << std::move(md);
	}

	return success;
}

bool DatabaseTracks::getAllTracksByAlbum(IDList albums, MetaDataList& returndata, Filter filter, SortOrder sort) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	QString querytext = _fetch_query;

	if(albums.size() == 0) return false;

	else if(albums.size() == 1) {
		querytext += "AND tracks.albumid=:albumid ";
	}

	else {
		querytext += "AND (tracks.albumid=:albumid ";
		for(int i=1; i<albums.size(); i++) {
			querytext += "OR tracks.albumid=:albumid_" + QString::number(i) + " ";
		}

		querytext += ") ";
	}

	if( !filter.filtertext.isEmpty() ) {


		switch(filter.mode) {
			case Filter::Mode::Genre:
				querytext += "AND tracks.genre LIKE :filter1 ";
				break;

			case Filter::Mode::Filename:
				querytext += "AND tracks.filename LIKE :filter1 ";
				break;

			case Filter::Mode::Fulltext:
			default:
				// consider the case, that the search string may fit to the title
				// union the case that the search string may fit to the album
				querytext += QString("AND tracks.trackid IN ( ") +
						"	SELECT t2.trackid " +
						"	FROM tracks t2 "+
						"	WHERE t2.cissearch LIKE :filter1 "+

						"	UNION SELECT t3.trackid "+
						"	FROM tracks t3, albums a2 "+
						"	WHERE a2.albumid = t3.albumid AND a2.cissearch LIKE :filter2 "+

						"	UNION SELECT t4.trackid " +
						"	FROM tracks t4, albums a3, artists ar2" +
						"	WHERE t4.albumid = a3.albumid " +
						"	AND t4.artistid = ar2.artistid " +
						"	AND ar2.cissearch LIKE :filter3 "

						") ";
				break;


		}

	}

	querytext = append_track_sort_string(querytext, sort);

	q.prepare(querytext);
	q.bindValue(":albumid", QVariant(albums[0]));
	for(int i=1; i<albums.size(); i++) {
		q.bindValue(QString(":albumid_") + QString::number(i), albums[i]);
	}

	if(filter.filtertext.length() > 0) {
		q.bindValue(":filter1", QVariant(filter.filtertext));

		switch(filter.mode) {
			case Filter::Mode::Genre:
			case Filter::Mode::Filename:
				break;

			case Filter::Mode::Fulltext:
			default:
				q.bindValue(":filter2", QVariant(filter.filtertext));
				q.bindValue(":filter3", QVariant(filter.filtertext));
				break;
		}

	}

	return db_fetch_tracks(q, returndata);

}

bool DatabaseTracks::getAllTracksByArtist(int artist, MetaDataList& returndata, Filter filter, SortOrder sort) {

	IDList list;
	list << artist;
	return getAllTracksByArtist(list, returndata, filter, sort);
}

bool DatabaseTracks::getAllTracksByArtist(IDList artists, MetaDataList& returndata, Filter filter, SortOrder sort) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	QString querytext = _fetch_query;

	if(artists.size() == 0){
		return false;
	}

	if(artists.size() == 1) {
		querytext += "AND tracks.artistid=:artist_id ";
	}

	else {
		querytext += "AND (tracks.artistid=:artist_id ";
		for(int i=1; i<artists.size(); i++) {
			querytext += "OR tracks.artistid=:artist_id_" + QString::number(i) + " ";
		}

		querytext += ") ";
	}

	if(filter.filtertext.length() > 0 ) {
		switch(filter.mode) {

			case Filter::Mode::Genre:
				querytext += "AND tracks.genre LIKE :filter1";
				break;

			case Filter::Mode::Filename:
				querytext += "AND tracks.filename LIKE :filter1 ";
				break;

			case Filter::Mode::Fulltext:
			default:
				querytext += QString("AND tracks.trackid IN ( ") +
						"	SELECT t2.trackid "
						"	FROM tracks t2 "
						"	WHERE t2.cissearch LIKE :filter1 "
						"	UNION SELECT t3.trackID "
						"	FROM tracks t3, albums a2"
						"	WHERE a2.albumID = t3.albumID AND a2.cissearch LIKE :filter2 "
						"	UNION SELECT t4.trackID "
						"	FROM tracks t4, albums a3, artists ar2"
						"	WHERE t4.albumid = a3.albumid AND t4.artistID = ar2.artistID AND ar2.cissearch LIKE :filter3 "
						") ";
				break;
		}
		// consider the case, that the search string may fit to the title
		// union the case that the search string may fit to the album

	}

	querytext = append_track_sort_string(querytext, sort);

	q.prepare(querytext);
	q.bindValue(":artist_id", QVariant(artists[0]));
	for(int i=1; i<artists.size(); i++) {
		q.bindValue(QString(":artist_id_") + QString::number(i), artists[i]);
	}

	if( !filter.filtertext.isEmpty() ) {
		q.bindValue(":filter1", QVariant(filter.filtertext));

		switch(filter.mode) {
			case Filter::Mode::Genre:
			case Filter::Mode::Filename:
				break;

			case Filter::Mode::Fulltext:
			default:
				q.bindValue(":filter2", QVariant(filter.filtertext));
				q.bindValue(":filter3", QVariant(filter.filtertext));
				break;
		}
	}

	return db_fetch_tracks(q, returndata);
}


bool DatabaseTracks::getAllTracksBySearchString(Filter filter, MetaDataList& result, SortOrder sort) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	QString querytext;

	switch(filter.mode) {

		case Filter::Mode::Genre:
			querytext = _fetch_query +
					"AND genrename LIKE :search_in_genre ";
			break;

		case Filter::Mode::Filename:
			querytext = _fetch_query +

					"AND tracks.filename LIKE :search_in_filename ";
			break;

		case Filter::Mode::Fulltext:
		default:

			querytext = _fetch_query + " AND tracks.trackID IN ("+
					"SELECT tracks.trackID FROM tracks WHERE tracks.cissearch LIKE :search_in_title " +
					"UNION "+
					"SELECT tracks.trackID FROM tracks INNER JOIN albums ON tracks.albumID = albums.albumID AND albums.cissearch LIKE :search_in_album "
					"UNION "+
					"SELECT tracks.trackID FROM tracks INNER JOIN artists ON tracks.artistID = artists.artistID AND artists.cissearch LIKE :search_in_artist "
					") ";

			break;
	}




	querytext = append_track_sort_string(querytext, sort);
	q.prepare(querytext);


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


	return db_fetch_tracks(q, result);
}

bool DatabaseTracks::deleteTrack(int id){

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	QString querytext = QString("DELETE FROM tracks WHERE trackID = :track_id;");

	q.prepare(querytext);
	q.bindValue(":track_id", id);

	if (!q.exec()) {
		q.show_error(QString("Cannot delete track") + QString::number(id));
		return false;
	}

	//deleteTrackFromPlaylists(id);

	return true;
}


bool DatabaseTracks::deleteTracks(const IDList& ids){

	int n_files = 0;
	bool success;

	_db.transaction();

	for(const int& id : ids){
		if( deleteTrack(id) ){
			n_files++;
		};
	}

	success = _db.commit();

	return success && (n_files == ids.size());
}


bool DatabaseTracks::deleteTracks(const MetaDataList& v_md) {

	int success = 0;

	_db.transaction();

	for(const MetaData& md : v_md){
		if( deleteTrack(md.id) ){
			success++;
		};
	}

	_db.commit();

	sp_log(Log::Info) << "Deleted " << success << " of " << v_md.size() << " tracks";

	return (success == v_md.size());
}

bool DatabaseTracks::deleteInvalidTracks(){

	bool success;

	MetaDataList v_md;
	QMap<QString, int> map;
	IDList to_delete;
	MetaDataList v_md_update;

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q(_db);
	DatabaseLibrary db_library(_db, _module_db_id);


	if(!getTracksFromDatabase(v_md)){
		sp_log(Log::Error) << "Cannot get tracks from db";
		return false;
	}

	int idx = 0;
	for(const MetaData& md : v_md){

		if(map.contains(md.filepath())){
			sp_log(Log::Warning) << "found double path: " << md.filepath();
			int old_idx = map[md.filepath()];
			to_delete << md.id;
			v_md_update << v_md[old_idx];
		}

		else{
			map.insert(md.filepath(), idx);
		}

		idx++;
	}

	sp_log(Log::Debug) << "Will delete " << to_delete.size() << " tracks";
	success = deleteTracks(to_delete);
	sp_log(Log::Debug) << "delete tracks " << success;

	success = deleteTracks(v_md_update);
	sp_log(Log::Debug) << "delete other tracks " << success;

	success = db_library.storeMetadata(v_md_update);
	sp_log(Log::Debug) << "update tracks " << success;

	return false;
}

QStringList DatabaseTracks::getAllGenres(){

	QString querystring;
	bool success;

	SayonaraQuery q(_db);

	querystring = "SELECT genre FROM tracks GROUP BY genre;";
	q.prepare(querystring);

	success = q.exec();
	if(!success){
		return QStringList();
	}

	QHash<QString, bool> hash;
	while(q.next()){

		QString genre = q.value("genre").toString();
		QStringList subgenres = genre.split(",");

		for(const QString& g: subgenres){
			hash[g.toLower()] = true;
		}
	}

	return QStringList(hash.keys());
}


void DatabaseTracks::updateTrackCissearch(LibraryHelper::SearchModeMask mode)
{
	MetaDataList v_md;
	getTracksFromDatabase(v_md);

	_db.transaction();
	for(const MetaData& md : v_md) {
		QString str = "UPDATE tracks SET cissearch=:cissearch WHERE trackID=:id;";
		SayonaraQuery q(_db);
		q.prepare(str);
		q.bindValue(":cissearch", LibraryHelper::convert_search_string(md.title, mode));
		q.bindValue(":id", md.id);

		if(!q.exec()){
			q.show_error("Cannot update album cissearch");
		}
	}
	_db.commit();
}


bool DatabaseTracks::updateTrack(const MetaData& md) {

	if(md.id == -1) return false;

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);

	QStringList genres;
	for(const QString& genre : md.genres){
		if(!genre.trimmed().isEmpty()){
			genres << genre;
		}
	}

	q.prepare("UPDATE Tracks "
			  "SET albumID=:albumID, "
			  "artistID=:artistID, "
			  "title=:title, "
			  "year=:year, "
			  "length=:length, "
			  "bitrate=:bitrate, "
			  "track=:track, "
			  "genre=:genre, "
			  "filesize=:filesize, "
			  "discnumber=:discnumber, "
			  "cissearch=:cissearch, "
			  "rating=:rating, "
			  "modifydate=:modifydate "
			  "WHERE TrackID = :trackID;");

	q.bindValue(":albumID",QVariant(md.album_id));
	q.bindValue(":artistID",QVariant(md.artist_id));
	q.bindValue(":title",QVariant(md.title));
	q.bindValue(":track",QVariant(md.track_num));
	q.bindValue(":length", QVariant(md.length_ms));
	q.bindValue(":bitrate", QVariant(md.bitrate));
	q.bindValue(":year",QVariant(md.year));
	q.bindValue(":trackID", QVariant(md.id));
	q.bindValue(":genre", QVariant(genres.join(",")));
	q.bindValue(":filesize", QVariant(md.filesize));
	q.bindValue(":discnumber", QVariant(md.discnumber));
	q.bindValue(":cissearch", QVariant(md.title.toLower()));
	q.bindValue(":rating", QVariant(md.rating));
	q.bindValue(":modifydate", Helper::current_date_to_int());

	if (!q.exec()) {
		q.show_error(QString("Cannot update track ") + md.filepath());
		return false;
	}

	return true;
}

bool DatabaseTracks::updateTracks(const MetaDataList& lst) {

	bool success;
	int n_files = 0;

	_db.transaction();
	for(const MetaData& md : lst){

		if(updateTrack(md)){
			n_files++;
		}
	}

	success = _db.commit();

	return success && (n_files == lst.size());
}

bool DatabaseTracks::insertTrackIntoDatabase (const MetaData& md, int artistID, int albumID) {

	DB_RETURN_NOT_OPEN_INT(_db);

	SayonaraQuery q (_db);

	MetaData md_tmp = getTrackByPath( md.filepath() );

	if( md_tmp.id >= 0 ) {
		MetaData track_copy = md;
		track_copy.id = md_tmp.id;
		track_copy.artist_id = artistID;
		track_copy.album_id = albumID;

		return updateTrack(track_copy);
	}

	QStringList genres;
	for(const QString& genre : md.genres){
		if(!genre.trimmed().isEmpty()){
			genres << genre;
		}
	}

	sp_log(Log::Info) << "insert new track: " << md.filepath();

	QString querytext = QString("INSERT INTO tracks ") +
			"(filename,albumID,artistID,title,year,length,track,bitrate,genre,filesize,discnumber,rating,cissearch,createdate,modifydate) " +
			"VALUES "+
			"(:filename,:albumID,:artistID,:title,:year,:length,:track,:bitrate,:genre,:filesize,:discnumber,:rating,:cissearch,:createdate,:modifydate); ";

	quint64 current_time = Helper::current_date_to_int();
	q.prepare(querytext);

	q.bindValue(":filename", md.filepath());
	q.bindValue(":albumID", albumID);
	q.bindValue(":artistID",artistID);
	q.bindValue(":length", md.length_ms);
	q.bindValue(":year", md.year);
	q.bindValue(":title", md.title);
	q.bindValue(":track", md.track_num);
	q.bindValue(":bitrate", md.bitrate);
	q.bindValue(":genre", genres.join(","));
	q.bindValue(":filesize", md.filesize);
	q.bindValue(":discnumber", md.discnumber);
	q.bindValue(":rating", md.rating);
	q.bindValue(":cissearch", md.title.toLower());
	q.bindValue(":createdate", current_time);
	q.bindValue(":modifydate", current_time);

	if (!q.exec()) {
		q.show_error(QString("Cannot insert track into database ") + md.filepath());
		return false;
	}

	return true;
}


bool DatabaseTracks::updateTrackDates()
{
	sp_log(Log::Debug, "Database Tracks") << "Insert dates...";

	QString querytext = QString("SELECT trackID, filename FROM tracks;");
	SayonaraQuery q(_db);
	q.prepare(querytext);
	QMap<int, QString> v_md;

	QList< std::tuple<int, quint64, quint64> > lst;
	if(q.exec())
	{
		while(q.next())
		{
			int id = q.value(0).toInt();
			QString filepath = q.value(1).toString();

			QString dir = Helper::File::get_parent_directory(filepath);
			QFileInfo fi(filepath);
			QFileInfo fi_dir(dir);
			QDateTime created = fi_dir.created();
			QDateTime modified = fi.lastModified();

			lst << std::make_tuple(id, Helper::date_to_int(created), Helper::date_to_int(modified));
		}
	}

	else{
		sp_log(Log::Error, "Database Tracks") << "Insert dates finished with error";
		return false;
	}

	_db.transaction();

	for(auto t : lst){
		SayonaraQuery q(_db);
		q.prepare("UPDATE tracks SET createdate=:createdate, modifydate=:modifydate WHERE trackID = :id;");
		q.bindValue(":id", std::get<0>(t));
		q.bindValue(":createdate", std::get<1>(t));
		q.bindValue(":modifydate", std::get<2>(t));
		q.exec();
	}

	_db.commit();

	sp_log(Log::Debug, "Database Tracks") << "Insert dates finished!";
	return true;
}
