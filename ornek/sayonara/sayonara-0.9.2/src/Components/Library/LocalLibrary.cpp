/* LocalLibrary.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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

#include "LocalLibrary.h"
#include "Importer/LibraryImporter.h"
#include "threads/ReloadThread.h"
#include "threads/IndexDirectoriesThread.h"
#include "threads/FileSystemWatcher.h"
#include "threads/UpdateDatesThread.h"
#include "Database/DatabaseConnector.h"

#include "Helper/LibrarySearchMode.h"
#include <utility>

LocalLibrary::LocalLibrary(QObject *parent) :
	AbstractLibrary(parent)
{
    _db = DatabaseConnector::getInstance();

	apply_db_fixes();

	REGISTER_LISTENER_NO_CALL(Set::Lib_SearchMode, _sl_search_mode_changed);
	REGISTER_LISTENER(Set::Lib_AutoUpdate, _sl_auto_update_changed);
}

LocalLibrary::~LocalLibrary(){

}

void LocalLibrary::apply_db_fixes()
{
	QString str_val;
	_db->load_setting("version", str_val);

	int version = str_val.toInt();
	if(version < 11){
		UpdateDatesThread* t = new UpdateDatesThread(this);
		connect(t, &QThread::finished, t, &QObject::deleteLater);
		t->start();
	}
}


void LocalLibrary::psl_reload_library(bool clear_first, Library::ReloadQuality quality) {

	if(_reload_thread && _reload_thread->is_running()){
		return;
	}

	init_reload_thread();

	QString library_path = _settings->get(Set::Lib_Path);

	if(library_path.isEmpty()) {

		emit sig_no_library_path();
		return;
	}

	if(clear_first) {
		delete_all_tracks();
	}

	_reload_thread->set_quality(quality);
	_reload_thread->set_lib_path(library_path);
	_reload_thread->start();
}


void LocalLibrary::reload_thread_finished() {

	load();

	emit sig_reloading_library("", -1);
	emit sig_reloading_library_finished();

	if(_fsw){
		_fsw->refresh();
	}
}

void LocalLibrary::_sl_search_mode_changed()
{
	sp_log(Log::Debug) << "Updating cissearch...";

	LibraryHelper::SearchModeMask mode = _settings->get(Set::Lib_SearchMode);
	_db->updateArtistCissearch(mode);
	_db->updateAlbumCissearch(mode);
	_db->updateTrackCissearch(mode);

	sp_log(Log::Debug) << "Updating cissearch finished";
}

void LocalLibrary::_sl_auto_update_changed()
{
	bool active = _settings->get(Set::Lib_AutoUpdate);
	if(active){
		MetaDataList v_md;
		get_all_tracks(v_md, LibSortOrder());
		IndexDirectoriesThread* thread = new IndexDirectoriesThread(v_md);
		connect(thread, &QThread::finished, this, &LocalLibrary::indexing_finished);
		thread->start();
	}

	else{
		if(_fsw){
			_fsw->stop();

			sp_log(Log::Debug) << "Removed filesystem watcher";
		}
	}
}

void LocalLibrary::indexing_finished()
{
	IndexDirectoriesThread* thread = dynamic_cast<IndexDirectoriesThread*>(sender());

	_fsw = new FileSystemWatcher(_settings->get(Set::Lib_Path), this);

	connect(_fsw, &QThread::finished, _fsw, &QObject::deleteLater);
	connect(_fsw, &QThread::destroyed, this, [=](){
		_fsw = nullptr;
	});

	connect(_fsw, &FileSystemWatcher::sig_changed, this, [=](){
		if(!_reload_thread || (_reload_thread && !_reload_thread->is_running())){
			this->psl_reload_library(false, Library::ReloadQuality::Fast);
		}
	});

	_fsw->start();

	thread->deleteLater();
	sp_log(Log::Debug) << "Added filesystem watcher";

}

void LocalLibrary::library_reloading_state_new_block() {

	_reload_thread->pause();

	_db->getAllAlbums(_vec_albums, _sortorder.so_albums);
	_db->getAllArtists(_vec_artists, _sortorder.so_artists);
	_db->getTracksFromDatabase(_vec_md, _sortorder.so_tracks);

	emit_stuff();

	_reload_thread->goon();
}


void LocalLibrary::psl_disc_pressed(int disc) {

    if( _selected_albums.size() == 0 ||
		_selected_albums.size() > 1 )
	{
		return;
	}

	MetaDataList v_metadata;


	if(disc < 0){
		_db->getAllTracksByAlbum(_selected_albums.first(), _vec_md, _filter, _sortorder.so_tracks);
		emit sig_all_tracks_loaded(_vec_md);
		return;
	}

	_db->getAllTracksByAlbum(_selected_albums.first(), v_metadata, _filter, _sortorder.so_tracks);

	_vec_md.clear();

	for(const MetaData& md : v_metadata) {

		if(md.discnumber != disc) {
			continue;
		}

		_vec_md << std::move(md);
    }

	emit sig_all_tracks_loaded(_vec_md);
}

void LocalLibrary::psl_track_rating_changed(int idx, int rating){
	sp_log(Log::Debug) << "Change rating for track at idx " << idx << "(" << _vec_md[idx].title << ") : " << rating;
	_vec_md[idx].rating = rating;
	update_track(_vec_md[idx]);

	Tagging::setMetaDataOfFile(_vec_md[idx]);
}


void LocalLibrary::get_all_artists(ArtistList& artists, LibSortOrder so){
	_db->getAllArtists(artists, so.so_artists);
}

void LocalLibrary::get_all_artists_by_searchstring(Filter filter, ArtistList& artists, LibSortOrder so){
	_db->getAllArtistsBySearchString(filter, artists, so.so_artists);
}


void LocalLibrary::get_all_albums(AlbumList& albums, LibSortOrder so){
	_db->getAllAlbums(albums, so.so_albums);
}

void LocalLibrary::get_all_albums_by_artist(IDList artist_ids, AlbumList& albums, Filter filter, LibSortOrder so){
	_db->getAllAlbumsByArtist(artist_ids, albums, filter, so.so_albums)	;
}

void LocalLibrary::get_all_albums_by_searchstring(Filter filter, AlbumList& albums, LibSortOrder so){
	_db->getAllAlbumsBySearchString(filter, albums, so.so_albums);
}

void LocalLibrary::get_all_tracks(MetaDataList& v_md, LibSortOrder so){
	_db->getTracksFromDatabase(v_md, so.so_tracks);
}

void LocalLibrary::get_all_tracks(const QStringList& paths, MetaDataList& v_md){
	_db->getMultipleTracksByPath(paths, v_md);
}

void LocalLibrary::get_all_tracks_by_artist(IDList artist_ids, MetaDataList& v_md, Filter filter, LibSortOrder so){
	_db->getAllTracksByArtist(artist_ids, v_md, filter, so.so_tracks);
}

void LocalLibrary::get_all_tracks_by_album(IDList album_ids, MetaDataList& v_md, Filter filter, LibSortOrder so){
	_db->getAllTracksByAlbum(album_ids, v_md, filter, so.so_tracks);
}

void LocalLibrary::get_all_tracks_by_searchstring(Filter filter, MetaDataList& v_md, LibSortOrder so){
	_db->getAllTracksBySearchString(filter, v_md, so.so_tracks);
}

void LocalLibrary::get_album_by_id(int album_id, Album& album){
	_db->getAlbumByID(album_id, album);
}

void LocalLibrary::get_artist_by_id(int artist_id, Artist& artist){
	_db->getArtistByID(artist_id, artist);
}

void LocalLibrary::update_track(const MetaData& md){
	_db->updateTrack(md);
}

void LocalLibrary::update_album(const Album& album){
	_db->updateAlbum(album);
}


void LocalLibrary::insert_tracks(const MetaDataList &v_md){
	_db->storeMetadata(v_md);
	AbstractLibrary::insert_tracks(v_md);
}

void LocalLibrary::init_reload_thread()
{
	if(_reload_thread){
		return;
	}

	_reload_thread = ReloadThread::getInstance();

	connect(_reload_thread, &ReloadThread::sig_reloading_library,
			this, &LocalLibrary::sig_reloading_library);

	connect(_reload_thread, &ReloadThread::sig_new_block_saved,
			this, &LocalLibrary::library_reloading_state_new_block);

	connect(_reload_thread, &ReloadThread::finished,
			this, &LocalLibrary::reload_thread_finished);
}

void LocalLibrary::delete_tracks(const MetaDataList &v_md, Library::TrackDeletionMode mode){

	_db->deleteTracks(v_md);

	AbstractLibrary::delete_tracks(v_md, mode);
}

// TODO: check for orphaned tracks
void LocalLibrary::refresh_artist(){

}

void LocalLibrary::refresh_albums(){

}

void LocalLibrary::refresh_tracks(){

}

void LocalLibrary::import_files(const QStringList& files){
	LibraryImporter::getInstance()->import_files(files);
	emit sig_import_dialog_requested();
}

void LocalLibrary::merge_artists(int target_artist)
{
	if(_selected_artists.isEmpty())	{
		return;
	}

	bool success;

	Artist artist;
	success =_db->getArtistByID(target_artist, artist);
	if(!success){
		return;
	}

	MetaDataList v_md;
	get_all_tracks_by_artist(_selected_artists.toList(), v_md, _filter, _sortorder);
	for(MetaData& md : v_md){
		md.artist_id = artist.id;
		md.artist = artist.name;
	}

	_db->updateTracks(v_md);
	refresh();
}

void LocalLibrary::merge_albums(int target_album)
{

	if(_selected_albums.isEmpty())	{
		return;
	}

	bool success;

	Album album;
	success =_db->getAlbumByID(target_album, album);
	if(!success){
		return;
	}

	MetaDataList v_md;
	get_all_tracks_by_album(_selected_albums.toList(), v_md, _filter, _sortorder);
	for(MetaData& md : v_md){
		md.album_id = album.id;
		md.album = album.name;
	}

	_db->updateTracks(v_md);
	refresh();

}
