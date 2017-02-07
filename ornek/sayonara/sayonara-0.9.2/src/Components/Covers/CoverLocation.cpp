/* CoverLocation.cpp */

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


#include "CoverLocation.h"
#include "CoverHelper.h"
#include "LocalCoverSearcher.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"

#include "Database/DatabaseHandler.h"

#include <QDir>

CoverLocation::CoverLocation() {
	qRegisterMetaType<CoverLocation>("CoverLocation");
	_valid = false;
}

CoverLocation::CoverLocation(const CoverLocation& cl) : 
	CoverLocation()
{
	_search_url = cl._search_url;
	_search_term = cl._search_term;
	_cover_path = cl._cover_path;
	_local_paths = cl._local_paths;
	_valid = cl._valid;
}

QString CoverLocation::get_cover_directory(){
	return Helper::get_sayonara_path() + QDir::separator() + "covers";
}

CoverLocation CoverLocation::getInvalidLocation() {

	CoverLocation cl;
	cl._cover_path = Helper::get_share_path() + "logo.png";
	cl._search_url = "";
	cl._search_term = "";
	cl._valid = false;
	return cl;
}

bool CoverLocation::isInvalidLocation(const QString& cover_path)
{
	QString path1 = Helper::File::clean_filename(cover_path);
	QString path2 = getInvalidLocation().cover_path();

	return (path1 == path2);
}

void CoverLocation::print() const{

	sp_log(Log::Info) << "CoverLocation: " << _cover_path;
	sp_log(Log::Info) << "CoverLocation: " << _search_url;
}

QString CoverLocation::toString() const{
	return QString("Location ") + _cover_path + " Url: " + _search_url;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QString& artist_name) {

	QString cover_dir = get_cover_directory();

	QString cover_token = CoverHelper::calc_cover_token(artist_name, album_name);

	QString cover_path =  cover_dir + QDir::separator() + cover_token + ".jpg";

	if(!QFile::exists(cover_dir)) {
		QDir().mkdir(cover_dir);
	}

	CoverLocation ret;
	ret._cover_path = cover_path;
	ret._search_term = artist_name + " " + album_name;
	ret._search_url = CoverHelper::calc_google_album_address(artist_name, album_name);
	ret._valid = true;

	return ret;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QStringList& artists) {

	QString major_artist = ArtistList::get_major_artist(artists);
	return get_cover_location(album_name, major_artist);
}

CoverLocation CoverLocation::get_cover_location(int album_id, quint8 db_id) {

	if(album_id < 0) {
		return CoverLocation::getInvalidLocation();
	}

	Album album;
	MetaDataList v_md;
	LibraryDatabase* db = DB::getInstance(db_id);

	bool success = db->getAlbumByID(album_id, album);

	if(!success) {
		return getInvalidLocation();
	}

	CoverLocation cl;
	cl = get_cover_location(album);

	db->getAllTracksByAlbum(album_id, v_md);
	for(const MetaData& md : v_md){
		cl._local_paths = LocalCoverSearcher::get_local_cover_paths_from_filename(md.filepath());
		if(!cl._local_paths.isEmpty()){
			break;
		}
	}

	return cl;
}


CoverLocation CoverLocation::get_cover_location(const Album& album) {
	int n_artists;

	n_artists = album.artists.size();

	CoverLocation cl;

	if( n_artists > 1 ) {
		cl = CoverLocation::get_cover_location(album.name, album.artists);
	}

	else if( n_artists == 1 ) {
		cl = CoverLocation::get_cover_location(album.name, album.artists[0]);
	}

	else {
		cl = CoverLocation::get_cover_location(album.name, "");
	}

	if(!album.cover_download_url.isEmpty()){
		cl._search_url = album.cover_download_url;
	}

	cl._search_term = album.name + " " + ArtistList::get_major_artist(album.artists);

	return cl;
}

CoverLocation CoverLocation::get_cover_location(const Artist& artist) {

	CoverLocation cl = CoverLocation::get_cover_location(artist.name);

	if(!artist.cover_download_url.isEmpty()){
		cl._search_url = artist.cover_download_url;
	}

	cl._search_term = artist.name;

	return cl;
}


CoverLocation CoverLocation::get_cover_location(const QString& artist) {

	if(artist.isEmpty()) return getInvalidLocation();

	QString cover_dir = get_cover_directory();
	QString token = QString("artist_") + CoverHelper::calc_cover_token(artist, "");
	QString target_file = cover_dir + QDir::separator() + token + ".jpg";

	if(!QFile::exists(cover_dir)) {
		QDir().mkdir(cover_dir);
	}

	CoverLocation ret;
	ret._cover_path = target_file;
	ret._search_url = CoverHelper::calc_google_artist_address(artist);
	ret._search_term = artist;
	ret._valid = true;

	return ret;
}


CoverLocation CoverLocation::get_cover_location(const MetaData& md) {

    CoverLocation cl;
	if(md.album_id >= 0){
		cl = get_cover_location(md.album_id, md.db_id);
	}

	if(!cl._valid){
		cl = get_cover_location(md.album, md.artist);
	}

	if(!md.cover_download_url.isEmpty())
	{
		QString extension = Helper::File::get_file_extension(md.cover_download_url);
		QString cover_dir = get_cover_directory();
		QString cover_token = CoverHelper::calc_cover_token(md.artist, md.album);
		QString cover_path =  cover_dir + QDir::separator() + cover_token + "." + extension;

		cl = get_cover_location(QUrl(md.cover_download_url), cover_path);
	}

	if(cl._search_url.isEmpty()){
		cl._search_url = md.cover_download_url;
	}

	return cl;
}


CoverLocation CoverLocation::get_cover_location(const QUrl& url, const QString& target_path)
{
	CoverLocation cl;
	cl._cover_path = target_path;
	cl._search_url = url.toString();
	cl._valid = true;

	return cl;
}


CoverLocation CoverLocation::get_cover_location_by_searchstring(const QString& search_string, const QString& target_path)
{
	CoverLocation cl;
	cl._cover_path = target_path;
	cl._search_term = search_string;
	cl._search_url = CoverHelper::calc_google_image_search_address(search_string);
	cl._valid = true;

	return cl;
}


bool CoverLocation::valid() const {
	return _valid;
}

QStringList CoverLocation::local_paths() const {
	return _local_paths;
}

QString CoverLocation::local_path(int idx) const
{
	if(!between(idx, _local_paths)){
		return QString();
	}

	return _local_paths[idx];
}

QString CoverLocation::cover_path() const {
	return _cover_path;
}

QString CoverLocation::search_url() const {
	return _search_url;
}

QString CoverLocation::search_term() const {
	return _search_term;
}

void CoverLocation::set_search_term(const QString& search_term)
{
	_search_term = search_term;
	_search_url = CoverHelper::calc_google_image_search_address(search_term);

}
