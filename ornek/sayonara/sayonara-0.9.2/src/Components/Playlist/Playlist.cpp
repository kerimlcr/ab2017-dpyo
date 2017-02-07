/* Playlist.cpp */

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


#include "Playlist.h"
#include "Helper/FileHelper.h"
#include "Helper/Set.h"
#include "Components/TagEdit/MetaDataChangeNotifier.h"
#include "Components/Engine/EngineHandler.h"

#include <utility>
#include <algorithm>


Playlist::Playlist(int idx, QString name) :
	PlaylistDBInterface(name),
	SayonaraClass()
{
	MetaDataChangeNotifier* md_change_notifier = MetaDataChangeNotifier::getInstance();
	EngineHandler* engine = EngineHandler::getInstance();

	_playlist_changed = false;
	_playlist_idx = idx;
	_playlist_type = Playlist::Type::Std;
	_playlist_mode = _settings->get(Set::PL_Mode);

	_is_storable = false;

	connect(md_change_notifier, &MetaDataChangeNotifier::sig_metadata_changed, this, &Playlist::metadata_changed);

	connect(engine, &EngineHandler::sig_md_changed, this, &Playlist::metadata_changed_single);
	connect(engine, &EngineHandler::sig_dur_changed, this, &Playlist::metadata_changed_single);

	REGISTER_LISTENER(Set::PL_Mode, _sl_playlist_mode_changed);
}

Playlist::~Playlist(){

}

void Playlist::clear() {
	if(_v_md.isEmpty()){
		return;
	}

	_v_md.clear();

	set_changed(true);
}

void Playlist::move_track(const int idx, int tgt) {

	SP::Set<int> indexes(idx);
	move_tracks(indexes, tgt);
}

void Playlist::move_tracks(const SP::Set<int>& indexes, int tgt) {

	_v_md.move_tracks(indexes, tgt);

	set_changed(true);
}

void Playlist::copy_track(int idx, int tgt) {

	SP::Set<int> indexes(idx);
	copy_tracks(indexes, tgt);
}

void Playlist::copy_tracks(const SP::Set<int>& indexes, int tgt) {

	_v_md.copy_tracks(indexes, tgt);
}



void Playlist::delete_track(const int idx) {
	_v_md.remove_track(idx);
	set_changed(true);
}

void Playlist::delete_tracks(const SP::Set<int>& indexes) {
	_v_md.remove_tracks(indexes);
	set_changed(true);
}


void Playlist::insert_track(const MetaData& md, int tgt) {
	MetaDataList v_md;
	v_md << md;
	insert_tracks(v_md, tgt);
}


void Playlist::insert_tracks(const MetaDataList& lst, int tgt) {

	_v_md.insert_tracks(lst, tgt);

	set_changed(true);
}


void Playlist::append_track(const MetaData& md) {

	MetaDataList v_md;
	v_md << md;
	append_tracks(v_md);
}

void Playlist::append_tracks(const MetaDataList& lst) {

	for(const MetaData& md : lst){
		_v_md << std::move(md);
		_v_md.last().is_disabled = !(Helper::File::check_file(md.filepath()));
	}

	set_changed(true);
}


void Playlist::replace_track(int idx, const MetaData& md) {

	if( !between(idx, _v_md) ) {
		return;
	}

	bool is_playing = _v_md[idx].pl_playing;

	_v_md[idx] = md;
	_v_md[idx].is_disabled = !(Helper::File::check_file(md.filepath()));
	_v_md[idx].pl_playing = is_playing;

	emit sig_data_changed(_playlist_idx);
}


Playlist::Type Playlist::get_type() const {
	return _playlist_type;
}

int Playlist::get_idx() const {
	return _playlist_idx;
}

void Playlist::set_idx(int idx){
	_playlist_idx = idx;
}

void Playlist::set_playlist_mode(const PlaylistMode& mode) {

	if(_playlist_mode.shuffle() != mode.shuffle()){
		for(MetaData& md : _v_md){
			md.played = false;
		}
	}

	_playlist_mode = mode;
}

quint64 Playlist::get_running_time() const
{
	quint64 dur_ms = 0;
	dur_ms = std::accumulate(_v_md.begin(), _v_md.end(), dur_ms, [](quint64 time, const MetaData& md){
		return time + md.length_ms;
	});

	return dur_ms;
}

PlaylistMode Playlist::get_playlist_mode() const {
	return _playlist_mode;
}

int Playlist::get_cur_track_idx() const {
	return _v_md.get_cur_play_track();
}

bool Playlist::get_cur_track(MetaData &md) const {
	int cur_play_idx = _v_md.get_cur_play_track();

	if(cur_play_idx < 0){
		return false;
	}

	md = _v_md[cur_play_idx];
	return true;
}

QStringList Playlist::toStringList() const {
	return _v_md.toStringList();
}

IdxList Playlist::find_tracks(int idx) const {
	return _v_md.findTracks(idx);
}

IdxList Playlist::find_tracks(const QString& filepath) const {
	return _v_md.findTracks(filepath);
}

int Playlist::get_count() const {
	return _v_md.size();
}

bool Playlist::is_empty() const {
	return _v_md.isEmpty();
}

const MetaDataList& Playlist::get_playlist() const
{
	return _v_md;
}


void Playlist::set_changed(bool b){

	_playlist_changed = b;

	emit sig_data_changed(_playlist_idx);
}

bool Playlist::was_changed() const
{
	return _playlist_changed;
}

bool Playlist::is_storable() const
{
	return _is_storable;
}


void Playlist::_sl_playlist_mode_changed(){
	PlaylistMode mode = _settings->get(Set::PL_Mode);
	set_playlist_mode(mode);
}


