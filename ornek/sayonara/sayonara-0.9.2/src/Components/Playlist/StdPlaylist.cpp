/* StdPlaylist.cpp */

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

#include "StdPlaylist.h"
#include "Helper/FileHelper.h"
#include "Helper/Parser/PlaylistParser.h"
#include "Helper/Random/RandomGenerator.h"

#include <algorithm>

StdPlaylist::StdPlaylist(int idx, QString name) :
	Playlist(idx, name)
{
	_playlist_type = Playlist::Type::Std;
	_is_storable = true;
}


bool StdPlaylist::change_track(int idx) {

	_v_md.set_cur_play_track(idx);

	// ERROR: invalid idx
	if( !between(idx, _v_md) ) {
		stop();
		return false;
	}

	_v_md[idx].played = true;

	// ERROR: track not available in file system anymore
	if( !Helper::File::check_file(_v_md[idx].filepath()) ) {

		_v_md[idx].is_disabled = true;

		return change_track(idx + 1);
	}

	return true;
}


int StdPlaylist::create_playlist(const MetaDataList& v_md) {

	bool was_changed = (!_v_md.isEmpty() || this->was_changed());

	if(PlaylistMode::isActiveAndEnabled(_playlist_mode.append())){
		_v_md << v_md;
	}

	else{
		_v_md = v_md;
	}

	set_changed(was_changed);

	return _v_md.size();
}


void StdPlaylist::play() {

	if( _v_md.isEmpty() ) {
		stop();
		return;
	}

	if(_v_md.get_cur_play_track() == -1) {
		_v_md.set_cur_play_track(0);
	}
}


void StdPlaylist::pause() {

}


void StdPlaylist::stop() {
	_v_md.set_cur_play_track(-1);

	for(MetaData& md : _v_md){
		md.played = false;
	}
}


void StdPlaylist::fwd() {

	PlaylistMode::PlaylistModeState rep1 = _playlist_mode.rep1();
	_playlist_mode.setRep1(false);

	next();

	_playlist_mode.setRep1(rep1);

}


void StdPlaylist::bwd() {

	int cur_idx = _v_md.get_cur_play_track();
	change_track( cur_idx - 1 );
}


void StdPlaylist::next() {

	int cur_track = _v_md.get_cur_play_track();
	int track_num = -1;

	// no track
	if(_v_md.isEmpty() ) {
		stop();
		return;
	}

	// stopped
	if(track_num == -1){
		track_num = 0;
	}

	if(PlaylistMode::isActiveAndEnabled(_playlist_mode.rep1())){
		track_num = cur_track;
	}

	// shuffle mode
	else if(PlaylistMode::isActiveAndEnabled(_playlist_mode.shuffle())) {
		track_num = calc_shuffle_track();
		if(track_num == -1){
			stop();
			return;
		}
	}

	// normal track
	else {

		// last track
		if(cur_track == _v_md.size() - 1){

			if(PlaylistMode::isActiveAndEnabled(_playlist_mode.repAll())){
				track_num = 0;
			}

			else {
				stop();
				return;
			}
		}

		else{
			track_num = cur_track + 1;
		}
	}

	change_track(track_num);
}


int StdPlaylist::calc_shuffle_track(){

	if(_v_md.size() <= 1){
		return -1;
	}

	QList<int> left_tracks;
	int i, left_tracks_idx;
	RandomGenerator rnd;

	// check all tracks played
	i=0;
	for(MetaData& md : _v_md){
		if(!md.played){
			left_tracks << i;
		}

		i++;
	}

	// no random track to play
	if(left_tracks.isEmpty()){

		if(PlaylistMode::isActiveAndEnabled(_playlist_mode.repAll())){
			return rnd.get_number(0, _v_md.size() -1);
		}

		else{
			return -1;
		}
	}

	left_tracks_idx = rnd.get_number(0, left_tracks.size() - 1);

	return left_tracks[left_tracks_idx];
}


void StdPlaylist::metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new) {

	Q_UNUSED(v_md_old)

	for(auto it=_v_md.begin(); it !=_v_md.end(); it++){

		auto lambda = [it](const MetaData& md) -> bool{
			return it->is_equal(md);
		};

		// find the value in old values
		auto tmp_it = std::find_if(v_md_new.begin(), v_md_new.end(), lambda);
		if(tmp_it != v_md_new.end()){
			// found, overwrite
			*it = *tmp_it;
		}
	}

	emit sig_data_changed(_playlist_idx);
}

void StdPlaylist::metadata_changed_single(const MetaData& md){
	IdxList idx_list = find_tracks(md.filepath());

	for(int i : idx_list){
		replace_track(i, md);
	}
}

