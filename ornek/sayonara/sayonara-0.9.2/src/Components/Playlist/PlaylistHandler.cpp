/* Playlist.cpp */

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
 *
 *  Created on: Apr 6, 2011
 *      Author: Lucio Carreras
 */

#include "PlaylistHandler.h"
#include "StdPlaylist.h"
#include "StreamPlaylist.h"
#include "PlaylistLoader.h"
#include "PlaylistDBWrapper.h"

#include "Database/DatabaseConnector.h"

#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Helper/Parser/PlaylistParser.h"

#include <algorithm>
#include <memory>

PlaylistHandler::PlaylistHandler(QObject * parent) :
	QObject (parent),
	SayonaraClass()
{

	qRegisterMetaType<PlaylistPtr>("PlaylistPtr");
	qRegisterMetaType<PlaylistConstPtr>("PlaylistConstPtr");

	_play_manager = PlayManager::getInstance();
	_db = DatabaseConnector::getInstance();

	_active_playlist_idx = -1;
	_current_playlist_idx = -1;

	connect(_play_manager,	&PlayManager::sig_playstate_changed, this, &PlaylistHandler::playstate_changed);
	connect(_play_manager, &PlayManager::sig_next, this, &PlaylistHandler::next);
	connect(_play_manager, &PlayManager::sig_previous, this, &PlaylistHandler::previous);
	connect(_play_manager, &PlayManager::sig_www_track_finished, this, &PlaylistHandler::www_track_finished);
}

PlaylistHandler::~PlaylistHandler() {

	_playlists.clear();
}


void PlaylistHandler::emit_cur_track_changed(){

	MetaData md;
	bool success;
	int cur_track_idx;

	PlaylistPtr pl = get_active();

	success = pl->get_cur_track(md);
	cur_track_idx = pl->get_cur_track_idx();

	if(!success || cur_track_idx == -1){
		_play_manager->stop();
		return;
	}

	_settings->set(Set::PL_LastPlaylist, pl->get_id());

	_play_manager->change_track(md, cur_track_idx);

	emit sig_cur_track_idx_changed( cur_track_idx,	pl->get_idx() );
}


int PlaylistHandler::load_old_playlists(){

	sp_log(Log::Debug) << "Loading playlists...";

	int last_track_idx=-1;
	int last_playlist_idx;

	PlaylistLoader loader;
	loader.create_playlists();

	last_playlist_idx = std::max(loader.get_last_playlist_idx(), 0);

	set_active_idx(last_playlist_idx);
	set_current_idx(last_playlist_idx);

	if(get_active()->get_count() > 0){
		last_track_idx = std::max(loader.get_last_track_idx(), 0);
	}

	if(last_track_idx >= 0){
		change_track(last_track_idx, last_playlist_idx);
	}

	else{
		_play_manager->stop();
		return _playlists.size();
	}

	if(_settings->get(Set::PL_StartPlaying)){
		_play_manager->play();
	}

	else{
		_play_manager->pause();
	}

	return _playlists.size();
}


PlaylistPtr PlaylistHandler::new_playlist(Playlist::Type type, int playlist_idx, QString name) {

	switch(type) {

	case Playlist::Type::Stream:
		return PlaylistPtr(new StreamPlaylist(playlist_idx, name));

	case Playlist::Type::Std:
	default:
		return PlaylistPtr(new StdPlaylist(playlist_idx, name));
	}

	return nullptr;
}


int PlaylistHandler::add_new_playlist(const QString& name, bool temporary, Playlist::Type type){

	PlaylistPtr pl;
	int idx = exists(name);

	if(idx >= 0) {
		return idx;
	}

	pl = new_playlist(type, _playlists.size(), name);
	pl->set_temporary(temporary);

	_playlists.append(pl);

	emit sig_new_playlist_added(pl);

	return pl->get_idx();
}


// create a playlist, where metadata is already available
int PlaylistHandler::create_playlist(const MetaDataList& v_md, const QString& name, bool temporary, Playlist::Type type) {

	int idx;
	PlaylistPtr pl;

	idx = exists(name);

	if(idx == -1){
		idx = add_new_playlist(name, temporary, type);
		pl =  _playlists[idx];
		pl->insert_temporary_into_db();
	}

	pl = _playlists[idx];

	pl->create_playlist(v_md);
	pl->set_temporary( pl->is_temporary() && temporary );

	set_current_idx(idx);

	return idx;
}


// create a new playlist, where only filepaths are given
// Load Folder, Load File...
int PlaylistHandler::create_playlist(const QStringList& pathlist, const QString& name, bool temporary, Playlist::Type type) {

	DirectoryReader reader;
	MetaDataList v_md = reader.get_md_from_filelist(pathlist);
	return create_playlist(v_md, name, temporary, type);
}


int PlaylistHandler::create_playlist(const QString& dir, const QString& name, bool temporary, Playlist::Type type) {

	QStringList lst;
	lst << dir;

	return create_playlist(lst, name, temporary, type);
}


int PlaylistHandler::create_playlist(const CustomPlaylist& cpl){

	int id = cpl.id;
	int idx;
	PlaylistPtr pl;

	auto it = std::find_if(_playlists.begin(), _playlists.end(), [id](const PlaylistPtr& pl){
		return (pl->get_id() == id);
	});

	if(it == _playlists.end()){
		idx = add_new_playlist(cpl.name, cpl.is_temporary, Playlist::Type::Std);
	}

	else{
		idx = (*it)->get_idx();
	}

	pl = _playlists[idx];
	pl->create_playlist(cpl.tracks);
	pl->set_changed(false);

	return pl->get_idx();
}


int PlaylistHandler::create_empty_playlist(const QString& name){

	MetaDataList v_md;
	return create_playlist(v_md, name, true);
}



void PlaylistHandler::clear_playlist(int pl_idx) {

	if( !between(pl_idx, _playlists)){
		return;
	}

	_playlists[pl_idx]->clear();
}


void PlaylistHandler::playstate_changed(PlayManager::PlayState state){

	switch(state){
		case PlayManager::PlayState::Playing:
			played();
			break;
		case PlayManager::PlayState::Paused:
			paused();
			break;
		case PlayManager::PlayState::Stopped:
			stopped();
			break;

		default:
			return;
	}
}

void PlaylistHandler::played(){

	get_active()->play();
}

void PlaylistHandler::paused(){

	get_active()->pause();
}

void PlaylistHandler::stopped(){

	_active_playlist_idx = -1;

	for(PlaylistPtr pl : _playlists){
		pl->stop();
	}

	return;
}

void PlaylistHandler::next() {
	get_active()->next();
	emit_cur_track_changed();
}


void PlaylistHandler::previous() {
	if( _play_manager->get_cur_position_ms() > 2000)
	{
		_play_manager->seek_abs_ms(0);
		return;
	}

	get_active()->bwd();
	emit_cur_track_changed();
}


void PlaylistHandler::change_track(int track_idx, int playlist_idx) {


	bool track_changed;
	PlaylistPtr pl;

	if( !between(playlist_idx, _playlists) ) {
		playlist_idx = get_active()->get_idx();
	}

	if( playlist_idx != _active_playlist_idx &&
			playlist_idx >= 0 )
	{
		get_active()->stop();
		set_active_idx(playlist_idx);
		pl = get_active();
	}

	pl = _playlists[playlist_idx];
	track_changed = pl->change_track(track_idx);

	if(track_changed){
		emit_cur_track_changed();
	}

	else{
		_play_manager->stop();
	}
}


void PlaylistHandler::set_active_idx(int idx){

	if(between(idx, _playlists)){
		_active_playlist_idx = idx;
	}

	else{
		_active_playlist_idx = get_active()->get_idx();
	}

	_settings->set(Set::PL_LastPlaylist, get_active()->get_id());
}

void PlaylistHandler::set_current_idx(int pl_idx){
	if(pl_idx == _current_playlist_idx){
		return;
	}

	_current_playlist_idx = pl_idx;

	emit sig_playlist_idx_changed(pl_idx);
}



void PlaylistHandler::play_next(const MetaDataList& v_md) {

	PlaylistPtr active = get_active();

	active->insert_tracks(v_md, active->get_cur_track_idx() + 1);
}


void PlaylistHandler::insert_tracks(const MetaDataList& v_md, int row, int pl_idx) {

	if(!between(pl_idx, _playlists)){
		return;
	}

	PlaylistPtr pl = _playlists[pl_idx];

	bool is_empty = pl->is_empty();
	bool stopped = (_play_manager->get_play_state() == PlayManager::PlayState::Stopped);

	pl->insert_tracks(v_md, row);

	if( is_empty &&
		stopped &&
		_settings->get(Set::Lib_DD_PlayIfStoppedAndEmpty))
	{
		this->change_track(0, pl_idx);
	}
}


void PlaylistHandler::append_tracks(const MetaDataList& v_md, int pl_idx) {

	if(!between(pl_idx, _playlists)){
		return;
	}

	_playlists[pl_idx]->append_tracks(v_md);
}

void PlaylistHandler::remove_rows(const SP::Set<int>& indexes, int pl_idx) {

	if(!between(pl_idx, _playlists)){
		return;
	}

	_playlists[pl_idx]->delete_tracks(indexes);
}


void PlaylistHandler::move_rows(const SP::Set<int>& indexes, int tgt_idx, int pl_idx) {

	if(!between(pl_idx, _playlists)){
		return;
	}

	_playlists[pl_idx]->move_tracks(indexes, tgt_idx);
}


QString PlaylistHandler::request_new_playlist_name() const
{
	return PlaylistDBInterface::request_new_db_name();
}

void PlaylistHandler::save_all_playlists()
{
	if(_settings->get(Set::PL_LoadTemporaryPlaylists))
	{
		_db->transaction();
		for(PlaylistPtr pl : _playlists){

			if(pl->is_temporary() && pl->was_changed()){
				pl->save();
			}
		}
		_db->commit();
	}
}


int PlaylistHandler::get_active_idx_of_cur_track() const
{
	for(PlaylistPtr pl : _playlists){
		if(pl->get_cur_track_idx() >= 0){
			return pl->get_idx();
		}
	}

	return _current_playlist_idx;
}

void PlaylistHandler::close_playlist(int idx){

	if(!between(idx, _playlists)){
		return;
	}

	bool was_active = (idx == _active_playlist_idx);

	if(_playlists[idx]->is_temporary()){
		_playlists[idx]->delete_playlist();
	}

	_playlists.removeAt(idx);

	if( was_active ){
		set_active_idx(0);
	}

	else if(_active_playlist_idx > idx){
		_active_playlist_idx --;
	}

	for(PlaylistPtr pl : _playlists){
		if(pl->get_idx() >= idx){
			pl->set_idx(pl->get_idx() - 1);
		}
	}

	if(was_active){
		_settings->set(Set::PL_LastPlaylist, -1);
		_settings->set(Set::PL_LastTrack, -1);
	}
	else{
		_settings->set(Set::PL_LastPlaylist, get_active()->get_id());
	}
}

PlaylistConstPtr PlaylistHandler::get_playlist_at(int idx) const {

	if(! between(idx, _playlists) ){
		return nullptr;
	}

	return std::const_pointer_cast<const Playlist>(_playlists[idx]);
}

PlaylistPtr PlaylistHandler::get_playlist(int idx, PlaylistPtr fallback) const
{
	if(! between(idx, _playlists)){
		return fallback;
	}

	return _playlists[idx];
}

PlaylistPtr PlaylistHandler::get_active()
{
	if(_play_manager->get_play_state() == PlayManager::PlayState::Stopped){
		_active_playlist_idx = -1;
	}

	// assure we have at least one playlist
	if(_playlists.size() == 0){
		_active_playlist_idx = add_new_playlist(request_new_playlist_name(), true, Playlist::Type::Std);
	}

	// assure valid idx
	if( !between(_active_playlist_idx, _playlists) )
	{
		if(between(_current_playlist_idx, _playlists)){
			_active_playlist_idx = _current_playlist_idx;
		}

		else {
			_active_playlist_idx = 0;
		}
	}

	return _playlists[_active_playlist_idx];
}


int	PlaylistHandler::get_active_idx() const
{
	return _active_playlist_idx;
}

int PlaylistHandler::get_current_idx() const
{
	return _current_playlist_idx;
}

int PlaylistHandler::get_playlist_count() const
{
	return _playlists.size();
}

QStringList PlaylistHandler::get_playlist_names() const
{
	QStringList names;
	for(const PlaylistPtr& pl : _playlists){
		names << pl->get_name();
	}

	return names;
}

int PlaylistHandler::exists(const QString& name) const
{
	if( name.isEmpty() &&
			between(_current_playlist_idx, _playlists))
	{
		return _current_playlist_idx;
	}

	for(const PlaylistPtr& pl : _playlists){

		if(pl->get_name().compare(name, Qt::CaseInsensitive) == 0){
			return pl->get_idx();
		}
	}

	return -1;
}



void PlaylistHandler::save_playlist_to_file(const QString& filename, bool relative) {

	if(!between(_current_playlist_idx, _playlists)){
		return;
	}

	PlaylistPtr pl = _playlists[_current_playlist_idx];
	PlaylistParser::save_playlist(filename, pl->get_playlist(), relative);
}


void PlaylistHandler::reset_playlist(int pl_idx){

	if(!between(pl_idx, _playlists)){
		return;
	}

	PlaylistDBWrapper* db_connector = PlaylistDBWrapper::getInstance();
	int id = _playlists[pl_idx]->get_id();

	CustomPlaylist cpl = db_connector->get_playlist_by_id(id);

	clear_playlist(pl_idx);
	create_playlist(cpl);
}


PlaylistDBInterface::SaveAsAnswer PlaylistHandler::save_playlist(int idx){

	PlaylistDBInterface::SaveAsAnswer ret;

	if( !between(idx, _playlists) ){
		return PlaylistDBInterface::SaveAsAnswer::Error;
	}

	PlaylistPtr pl = _playlists[idx];

	_db->transaction();
	ret = pl->save();
	_db->commit();

	if(!pl->is_temporary()){
		emit sig_saved_playlists_changed();
	}

	return ret;
}


PlaylistDBInterface::SaveAsAnswer PlaylistHandler::save_playlist_as(int idx, const QString& name, bool force_override){

	PlaylistPtr pl = _playlists[idx];
	PlaylistDBInterface::SaveAsAnswer ret;

	// no empty playlists
	if(name.isEmpty()){
		return PlaylistDBInterface::SaveAsAnswer::Error;
	}

	ret = pl->save_as(name, force_override);
	if(ret != PlaylistDBInterface::SaveAsAnswer::Success){
		return ret;
	}

	// for PlaylistChooser
	if(!pl->is_temporary()){
		emit sig_saved_playlists_changed();
	}

	// for GUI
	emit sig_playlist_name_changed(idx);

	return PlaylistDBInterface::SaveAsAnswer::Success;
}


PlaylistDBInterface::SaveAsAnswer PlaylistHandler::rename_playlist(int pl_idx, const QString& name){

	// no empty playlists
	if(name.isEmpty()){
		return PlaylistDBInterface::SaveAsAnswer::Error;
	}

	// get playlist we want to save
	PlaylistPtr pl = _playlists[pl_idx];

	PlaylistDBInterface::SaveAsAnswer ret = pl->rename(name);
	if(ret != PlaylistDBInterface::SaveAsAnswer::Success){
		return ret;
	}

	emit sig_playlist_name_changed(pl_idx);

	// for PlaylistChooser
	if(!pl->is_temporary()){
		emit sig_saved_playlists_changed();
	}

	return PlaylistDBInterface::SaveAsAnswer::Success;
}

void PlaylistHandler::delete_playlist(int idx){

	PlaylistPtr pl = _playlists[idx];
	bool was_temporary = pl->is_temporary();
	bool success = pl->remove_from_db();

	// for PlaylistChooser
	if(success && !was_temporary){
		emit sig_saved_playlists_changed();
	}
}

void PlaylistHandler::www_track_finished(const MetaData& md){
	PlaylistPtr active_pl = this->get_active();
	if(!active_pl){
		return;
	}

	active_pl->insert_track(md, active_pl->get_cur_track_idx());
}
