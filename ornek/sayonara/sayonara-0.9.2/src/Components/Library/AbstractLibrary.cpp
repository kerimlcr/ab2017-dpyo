/* AbstractLibrary.cpp */

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

#include "AbstractLibrary.h"

#include "Components/Playlist/PlaylistHandler.h"

#include "Components/TagEdit/MetaDataChangeNotifier.h"
#include "Helper/Tagging/Tagging.h"
#include <QHash>

AbstractLibrary::AbstractLibrary(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{

	_playlist = PlaylistHandler::getInstance();
	_sortorder = _settings->get(Set::Lib_Sorting);

	_filter.mode = Filter::Mode::Fulltext;
	_filter.filtertext = "";

	MetaDataChangeNotifier* md_change_notifier = MetaDataChangeNotifier::getInstance();
	connect(md_change_notifier, &MetaDataChangeNotifier::sig_metadata_changed,
			this,				&AbstractLibrary::psl_metadata_id3_changed);
}

AbstractLibrary::~AbstractLibrary(){

}


void AbstractLibrary::load () {

	REGISTER_LISTENER(Set::Lib_Sorting, _sl_sortorder_changed);

	_filter.cleared = true;
	_filter.filtertext = "";

	get_all_artists(_vec_artists, _sortorder);
	get_all_albums(_vec_albums, _sortorder);
	get_all_tracks(_vec_md, _sortorder);

	emit_stuff();
}



void AbstractLibrary::emit_stuff() {

	emit sig_all_albums_loaded( _vec_albums );
	emit sig_all_artists_loaded( _vec_artists );
	emit sig_all_tracks_loaded( _vec_md );
}



void AbstractLibrary::refetch(){

	_selected_albums.clear();
	_selected_artists.clear();
	_selected_tracks.clear();

	_vec_albums.clear();
	_vec_artists.clear();
	_vec_md.clear();

	_filter.cleared = true;
	_filter.filtertext = "";

	get_all_artists(_vec_artists, _sortorder);
	get_all_albums(_vec_albums, _sortorder);
	get_all_tracks(_vec_md, _sortorder);

	emit_stuff();
}



void AbstractLibrary::refresh() {

	SP::Set<int> sel_artists, sel_albums, sel_tracks;
	SP::Set<int> sel_artists_idx, sel_albums_idx, sel_tracks_idx;

	sel_artists = _selected_artists;
	sel_albums = _selected_albums;
	sel_tracks = _selected_tracks;

	fetch_by_filter(_filter, true);

	for(int i=0; i<_vec_artists.size(); i++){
		if(sel_artists.contains(_vec_artists[i].id)) {
			sel_artists_idx.insert(i);
		}
	}

	change_artist_selection(sel_artists_idx);

	for(int i=0; i<_vec_albums.size(); i++){
		if(sel_albums.contains(_vec_albums[i].id)) {
			sel_albums_idx.insert(i);
		}
	}

	change_album_selection(sel_albums_idx);

	for(int i=0; i<_vec_md.size(); i++){
		if(sel_tracks.contains(_vec_md[i].id)) {
			sel_tracks_idx.insert(i);
		}
	}

	emit_stuff();

	if(sel_tracks_idx.size() > 0){
		MetaDataList v_md = change_track_selection(sel_tracks_idx);

		if(v_md.size() > 0){
			sp_log(Log::Info) << "1: mimedata";
			emit sig_track_mime_data_available(v_md);
		}
	}
}


void AbstractLibrary::psl_prepare_artist_for_playlist(int idx, bool new_playlist) {
	Q_UNUSED(idx);

	if(!new_playlist){
		_playlist->create_playlist(_vec_md);
	}

	else{
		_playlist->create_playlist(_vec_md, _playlist->request_new_playlist_name());
	}

	set_playlist_action_after_double_click();
}


void AbstractLibrary::psl_prepare_album_for_playlist(int idx, bool new_playlist) {
	Q_UNUSED(idx);

	if(!new_playlist){
		_playlist->create_playlist(_vec_md);
	}

	else{
		_playlist->create_playlist(_vec_md, _playlist->request_new_playlist_name());
	}

	set_playlist_action_after_double_click();
}


void AbstractLibrary::psl_prepare_track_for_playlist(int idx, bool new_playlist) {
	SP::Set<int> indexes;
	indexes.insert(idx);
	psl_prepare_tracks_for_playlist(indexes, new_playlist);
}

void AbstractLibrary::psl_prepare_tracks_for_playlist(bool new_playlist){
	if(!new_playlist){
		_playlist->create_playlist(_vec_md);
	}

	else{
		_playlist->create_playlist(_vec_md, _playlist->request_new_playlist_name());
	}

	set_playlist_action_after_double_click();
}

void AbstractLibrary::psl_prepare_tracks_for_playlist(const SP::Set<int>& indexes, bool new_playlist) {
	MetaDataList v_md;

	for(int idx : indexes) {
		v_md.push_back(_vec_md[idx]);
	}


	if(!new_playlist){
		_playlist->create_playlist(v_md);
	}

	else{
		_playlist->create_playlist(v_md, _playlist->request_new_playlist_name());
	}

	set_playlist_action_after_double_click();
}

void AbstractLibrary::psl_prepare_tracks_for_playlist(const QStringList& paths, bool new_playlist) {

	if(!new_playlist){
		_playlist->create_playlist(paths);
	}

	else{
		_playlist->create_playlist(paths, _playlist->request_new_playlist_name());
	}

	set_playlist_action_after_double_click();

}


void AbstractLibrary::set_playlist_action_after_double_click()
{

	PlayManager* play_manager = PlayManager::getInstance();

	if(_settings->get(Set::Lib_DC_DoNothing)){
		return;
	}

	else if(_settings->get(Set::Lib_DC_PlayIfStopped)){
		if(play_manager->get_play_state() != PlayManager::PlayState::Playing){
			_playlist->change_track(0, _playlist->get_current_idx());
		}
	}

	else{
		_playlist->change_track(0, _playlist->get_current_idx());
	}
}



void AbstractLibrary::psl_play_next_all_tracks() {
	_playlist->play_next(_vec_md);

}

void AbstractLibrary::psl_play_next_tracks(const SP::Set<int>& indexes) {
	MetaDataList v_md;

	for(auto it=indexes.begin(); it != indexes.end(); it++){
		int idx = *it;
		v_md.push_back(_vec_md[idx]);
	}

	_playlist->play_next(v_md);
}


void AbstractLibrary::psl_append_all_tracks() {
	_playlist->append_tracks(_vec_md, _playlist->get_current_idx());
}


void AbstractLibrary::psl_append_tracks(const SP::Set<int>& indexes) {
	MetaDataList v_md;

	for(auto it=indexes.begin(); it != indexes.end(); it++){
		int idx = *it;
		v_md.push_back(_vec_md[idx]);
	}

	_playlist->append_tracks(v_md, _playlist->get_current_idx());
}


void AbstractLibrary::restore_artist_selection(){

	SP::Set<ArtistID> new_selected_artists;

	for(const Artist& artist : _vec_artists) {
		if(!_selected_artists.contains( artist.id)) {
			continue;
		}

		new_selected_artists.insert(artist.id);
	}

	_selected_artists = new_selected_artists;
}


void AbstractLibrary::restore_album_selection(){

	SP::Set<AlbumID> new_selected_albums;
	for(const Album& album : _vec_albums) {

		if(!_selected_albums.contains(album.id)) {
			continue;
		}

		new_selected_albums.insert(album.id);
	}

	_selected_albums = new_selected_albums;
}


void AbstractLibrary::restore_track_selection(){

	SP::Set<TrackID> new_selected_tracks;
	for(const MetaData& md : _vec_md) {

		if(!_selected_tracks.contains(md.id)) {
			continue;
		}

		new_selected_tracks.insert(md.id);
	}

	_selected_tracks = new_selected_tracks;
}




void AbstractLibrary::change_artist_selection(const SP::Set<int>& indexes){

	SP::Set<ArtistID> selected_artists;

	if(indexes.isEmpty() && _selected_artists.isEmpty()){

		restore_album_selection();
		restore_track_selection();

		emit sig_all_albums_loaded(_vec_albums);
		emit sig_all_tracks_loaded(_vec_md);
		return;
	}

	for(auto it=indexes.begin(); it!=indexes.end(); it++){
		int idx = *it;
		const Artist& artist = _vec_artists[idx];
		selected_artists.insert(artist.id);
	}

	_vec_albums.clear();
	_vec_md.clear();

	_selected_artists = selected_artists;

	if(_selected_artists.size() > 0) {
		get_all_tracks_by_artist(_selected_artists.toList(), _vec_md, _filter, _sortorder);
		get_all_albums_by_artist(_selected_artists.toList(), _vec_albums, _filter, _sortorder);
	}

	else if(!_filter.cleared) {
		get_all_tracks_by_searchstring(_filter, _vec_md, _sortorder);
		get_all_albums_by_searchstring(_filter, _vec_albums, _sortorder);
		get_all_artists_by_searchstring(_filter, _vec_artists, _sortorder);
	}

	else{
		get_all_tracks(_vec_md, _sortorder);
		get_all_albums(_vec_albums, _sortorder);
	}

	restore_album_selection();
	restore_track_selection();
}




void AbstractLibrary::psl_selected_artists_changed(const SP::Set<int>& indexes) {

	change_artist_selection(indexes);

	emit sig_all_albums_loaded(_vec_albums);
	emit sig_all_tracks_loaded(_vec_md);
}


void AbstractLibrary::change_album_selection(const SP::Set<int>& indexes){

	SP::Set<AlbumID> selected_albums;

	for(auto it=indexes.begin(); it != indexes.end(); it++){
		int idx = *it;
		const Album& album = _vec_albums[idx];
		selected_albums.insert(album.id);
	}

	if(selected_albums == _selected_albums) {
		return;
	}

	_vec_md.clear();
	_selected_albums = selected_albums;

	// only show tracks of selected album / artist
	if(_selected_artists.size() > 0) {

		if(_selected_albums.size() > 0) {

			MetaDataList v_md;

			get_all_tracks_by_album(_selected_albums.toList(), v_md, _filter, _sortorder);

			// filter by artist

			for(const MetaData& md : v_md) {
				if(_selected_artists.contains(md.artist_id)){
					_vec_md << std::move(md);
				}
			}
		}

		else{
			get_all_tracks_by_artist(_selected_artists.toList(), _vec_md, _filter, _sortorder);
		}
	}

	// only album is selected
	else if(_selected_albums.size() > 0) {
		get_all_tracks_by_album(_selected_albums.toList(), _vec_md, _filter, _sortorder);
	}

	// neither album nor artist, but searchstring
	else if(!_filter.cleared) {
		get_all_tracks_by_searchstring(_filter, _vec_md, _sortorder);
	}

	// no album, no artist, no searchstring
	else{
		get_all_tracks(_vec_md, _sortorder);
	}

	restore_track_selection();
}


void AbstractLibrary::psl_selected_albums_changed(const SP::Set<int>& idx_list) {

	change_album_selection(idx_list);
	emit sig_all_tracks_loaded(_vec_md);
}


MetaDataList AbstractLibrary::change_track_selection(const SP::Set<int>& idx_list){

	_selected_tracks.clear();

	MetaDataList v_md;

	for(int idx : idx_list) {
		const MetaData& md = _vec_md[idx];
		v_md << md;

		_selected_tracks.insert(md.id);
	}

	return v_md;
}



void AbstractLibrary::psl_selected_tracks_changed(const SP::Set<int>& idx_list) {

	MetaDataList v_md =	change_track_selection(idx_list);
	if(v_md.size() > 0){
		emit sig_track_mime_data_available(v_md);
	}
}


void AbstractLibrary::fetch_by_filter(const Filter& filter, bool force){

	if( _filter.cleared &&
		filter.cleared &&
		filter.filtertext.size() < 5 &&
		(_selected_artists.size() == 0) &&
		(_selected_albums.size() == 0) &&
		!force)
	{
		return;
	}

	_filter = filter;

	_vec_albums.clear();
	_vec_artists.clear();
	_vec_md.clear();

	_selected_albums.clear();
	_selected_artists.clear();


	if(_filter.cleared) {

		get_all_artists(_vec_artists, _sortorder);
		get_all_albums(_vec_albums, _sortorder);
		get_all_tracks(_vec_md, _sortorder);
	}

	else {
		get_all_artists_by_searchstring(_filter, _vec_artists, _sortorder);
		get_all_albums_by_searchstring(_filter, _vec_albums, _sortorder);
		get_all_tracks_by_searchstring(_filter, _vec_md, _sortorder);
	}
}


void AbstractLibrary::psl_filter_changed(const Filter& filter, bool force) {

	//sp_log(Log::Debug) << "Library: Filter changed";
	fetch_by_filter(filter, force);
	emit_stuff();
}




void AbstractLibrary::_sl_sortorder_changed() {

	LibSortOrder so = _settings->get(Set::Lib_Sorting);

	// artist sort order has changed
	if(so.so_artists != _sortorder.so_artists) {

		_sortorder = so;
		_vec_artists.clear();

		if(!_filter.cleared) {
			get_all_artists_by_searchstring(_filter, _vec_artists, _sortorder);
		}

		else{
			get_all_artists(_vec_artists, _sortorder);
		}

		emit sig_all_artists_loaded(_vec_artists);
	}


	// album sort order has changed
	if(so.so_albums != _sortorder.so_albums) {

		_sortorder = so;
		_vec_albums.clear();

		// selected artists and maybe filter
		if (_selected_artists.size() > 0) {
			get_all_albums_by_artist(_selected_artists.toList(), _vec_albums, _filter, _sortorder);
		}

		// only filter
		else if( !_filter.cleared ) {
			get_all_albums_by_searchstring(_filter, _vec_albums, _sortorder);
		}

		// all albums
		else{
			get_all_albums(_vec_albums, _sortorder);
		}

		emit sig_all_albums_loaded(_vec_albums);
	}


	// track sort order has changed
	if(so.so_tracks != _sortorder.so_tracks) {

		_sortorder = so;
		_vec_md.clear();

		if(_selected_albums.size() > 0) {
			get_all_tracks_by_album(_selected_albums.toList(), _vec_md, _filter, _sortorder);
		}
		else if(_selected_artists.size() > 0) {
			get_all_tracks_by_artist(_selected_artists.toList(), _vec_md, _filter, _sortorder);
		}

		else if(!_filter.cleared) {
			get_all_tracks_by_searchstring(_filter, _vec_md, _sortorder);
		}

		else {
			get_all_tracks(_vec_md, _sortorder);
		}

		emit sig_all_tracks_loaded(_vec_md);
	}
}



void AbstractLibrary::psl_track_rating_changed(int idx, int rating) {

	_vec_md[idx].rating = rating;
	update_track(_vec_md[idx]);
}

void AbstractLibrary::psl_album_rating_changed(int idx, int rating) {

	_vec_albums[idx].rating = rating;
	update_album(_vec_albums[idx]);
}



void AbstractLibrary::psl_metadata_changed(const MetaData& md) {

	if( md.id < 0 ) return;

	update_track(md);
	refresh();
	emit_stuff();
}


void AbstractLibrary::psl_dur_changed(const MetaData& md){

	if( md.id < 0 ) return;

	update_track(md);
	refresh();
	emit_stuff();
}


void AbstractLibrary::psl_metadata_id3_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new){

	// id -> idx
	QHash<int, int> md_map;
	for(int i=0; i<_vec_md.size(); i++){
		md_map[_vec_md[i].id] = i;
	}

	// check for new artists and albums
	for(int i=0; i<v_md_old.size(); i++){
		int id = v_md_old[i].id;
		int new_artist_id = v_md_new[i].artist_id;
		int new_album_id = v_md_new[i].album_id;

		if( v_md_old[i].artist_id != new_artist_id ){
			if( !_selected_artists.contains(new_artist_id) ){
				_selected_artists.insert(new_artist_id);
			}
		}

		if( v_md_old[i].album_id != new_album_id){
			if( !_selected_albums.contains(new_album_id) ){
				_selected_albums.insert(new_album_id);
			}
		}

		if(md_map.contains(id)){
			int val = md_map[id];
			_vec_md[val] = v_md_new[i];
		}
	}

	refresh();
	emit_stuff();
}


void AbstractLibrary::insert_tracks(const MetaDataList &v_md){
	Q_UNUSED(v_md)
	refresh();
}

void AbstractLibrary::import_files(const QStringList &files){
	Q_UNUSED(files)
}


void AbstractLibrary::delete_current_tracks(Library::TrackDeletionMode mode){
	if(mode == Library::TrackDeletionMode::None) return;
	delete_tracks(_vec_md, mode);
}


void AbstractLibrary::delete_all_tracks(){

	MetaDataList v_md;
	get_all_tracks(v_md, _sortorder);
	delete_tracks(v_md, Library::TrackDeletionMode::OnlyLibrary);
}



void AbstractLibrary::delete_tracks(const MetaDataList &v_md, Library::TrackDeletionMode mode){

	if(mode == Library::TrackDeletionMode::None) return;

	QString file_entry = tr("entries");
	QString answer_str;

	int n_fails = 0;
	if(mode == Library::TrackDeletionMode::AlsoFiles){
		file_entry = tr("files");
		for( const MetaData& md : v_md ){
			QFile f(md.filepath());
			if(!f.remove()){
				n_fails++;
			}
		}
	}

	if(n_fails == 0) {
		answer_str = tr("All %1 could be removed").arg(file_entry);
	}

	else {
		answer_str = tr("%1 of %2 %3 could not be removed").arg(n_fails).arg(v_md.size()).arg(file_entry);
	}

	emit sig_delete_answer(answer_str);

	refresh();
}


void AbstractLibrary::delete_tracks_by_idx(const SP::Set<int>& indexes, Library::TrackDeletionMode mode){

	if(mode == Library::TrackDeletionMode::None) return;

	MetaDataList v_md;
	for(auto it = indexes.begin(); it != indexes.end(); it++){
		int idx = *it;
		v_md.push_back(_vec_md[idx]);
	}

	delete_tracks(v_md, mode);
}
