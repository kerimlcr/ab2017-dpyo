/* MetaDataList.cpp */

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

#include "MetaDataList.h"
#include "Helper/FileHelper.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Random/RandomGenerator.h"
#include "Helper/Set.h"

#include <algorithm>

MetaDataList::MetaDataList() :
	QList<MetaData>()
{
	_cur_played_track = -1;
}

MetaDataList::MetaDataList(const MetaDataList& lst) :
	QList(lst)
{
	_cur_played_track = lst._cur_played_track;
}

MetaDataList::~MetaDataList() {

}

void MetaDataList::set_cur_play_track_idx_only(int idx) {
	_cur_played_track = idx;
}

void MetaDataList::set_cur_play_track(int idx) {

	_cur_played_track = -1;
	if( !between(idx, this)) {
		return;
	}

	int tmp_idx=0;
	for(auto it=this->begin(); it != this->end(); it++ ){
		it->pl_playing = (idx == tmp_idx);
		tmp_idx++;
	}

	_cur_played_track = idx;
}

MetaDataList& MetaDataList::insert_tracks(const MetaDataList& v_md, int tgt_idx){

	if(v_md.isEmpty()) {
		return *this;
	}

	int cur_track = this->get_cur_play_track();

	tgt_idx = std::max(0, tgt_idx);
	tgt_idx = std::min(this->size(), tgt_idx);

	int old_tgt_idx = tgt_idx;

	for(const MetaData& md : v_md){
		this->insert(tgt_idx, md);
		tgt_idx++;
	}

	if(cur_track >= old_tgt_idx){
		set_cur_play_track(cur_track + v_md.size());
	}

	return *this;
}

MetaDataList& MetaDataList::copy_tracks(const SP::Set<int>& indexes, int tgt_idx){

	MetaDataList v_md;
	for(int idx : indexes){
		v_md << this->operator[](idx);
	}

	return insert_tracks(v_md, tgt_idx);
}


MetaDataList& MetaDataList::move_tracks(const SP::Set<int>& indexes, int tgt_idx){

MetaDataList v_md_to_move;
	MetaDataList v_md_before_tgt;
	MetaDataList v_md_after_tgt;

	int n_tracks_before_cur_idx =
		std::count_if(indexes.begin(), indexes.end(), [=](int idx){
			return idx < _cur_played_track;
		});

	int n_tracks_after_cur_idx =
		std::count_if(indexes.begin(), indexes.end(), [=](int idx){
			return idx > _cur_played_track;
		});


	int i=0;
	for(auto it=this->begin(); it!=this->end(); it++, i++)
	{
		const MetaData& md = *it;
		it->pl_playing = (i == _cur_played_track);

		bool contains_i = indexes.contains(i);

		if(!contains_i && i < tgt_idx){
			v_md_before_tgt << std::move( md );
		}

		else if(!contains_i && i >= tgt_idx){
			v_md_after_tgt << std::move( md );
		}

		else if(contains_i){
			v_md_to_move << std::move( md );
		}
	}

	int start_idx = 0;

	std::move(v_md_before_tgt.begin(), v_md_before_tgt.end(), this->begin());

	start_idx += v_md_before_tgt.size();

	std::move(v_md_to_move.begin(), v_md_to_move.end(), this->begin() + start_idx);

	start_idx += v_md_to_move.size();

	std::move(v_md_after_tgt.begin(), v_md_after_tgt.end(), this->begin() + start_idx);

	i=0;
	for(auto it=this->begin(); it!=this->end(); it++, i++)
	{
		if(it->pl_playing){
			_cur_played_track = i;
			break;
		}
	}

	if(tgt_idx < _cur_played_track){
		_cur_played_track += n_tracks_after_cur_idx;
	}

	else if(tgt_idx > _cur_played_track){
		_cur_played_track -= n_tracks_before_cur_idx;
	}


	return *this;
}

MetaDataList& MetaDataList::randomize(){
	RandomGenerator generator;

	for(auto it=this->begin(); it != this->end(); it++){
		int rnd_number = generator.get_number(0, this->size()-1);
		std::swap(*it, *(this->begin() + rnd_number));
	}

	return *this;
}

MetaDataList MetaDataList::extract_tracks(std::function<bool (const MetaData &)> func) const
{
	if(this->isEmpty()){
		return MetaDataList();
	}

	MetaDataList v_md;
	auto it = this->begin();

	while(it != this->end()){

		it = std::find_if(it, this->end(), func);
		if(it == this->end()){
			break;
		}

		else{
			v_md << *it;
			it++;
		}
	}

	return v_md;
}

MetaDataList MetaDataList::extract_tracks(const SP::Set<int>& indexes) const
{
	MetaDataList v_md;

	std::for_each(indexes.begin(), indexes.end(), [this, &v_md](int row){
		v_md << this->operator[](row);
	});

	return v_md;
}


MetaDataList& MetaDataList::remove_track(int idx)
{
	return remove_tracks(idx, idx);
}

MetaDataList& MetaDataList::remove_tracks(int first, int last)
{
	if(!between(first, this)){
		return *this;
	}

	if(!between(last, this)){
		return *this;
	}

	for(int i=last; i>=first; i--){
		this->removeAt(first);
	}

	if(_cur_played_track >= first && _cur_played_track <= last){
		set_cur_play_track(-1);
	}

	if(_cur_played_track > last){
		set_cur_play_track( _cur_played_track - (last - first + 1) );
	}

	return *this;
}

MetaDataList& MetaDataList::remove_tracks(const SP::Set<int>& indexes){

	for(auto it=indexes.rbegin(); it != indexes.rend(); it++){
		this->removeAt(*it);
	}

	return *this;
}

int MetaDataList::get_cur_play_track() const {
	return _cur_played_track;
}


bool MetaDataList::contains(const MetaData& md) const {

	auto it = std::find_if(this->begin(), this->end(), [&md](const MetaData& md_tmp){
		return md.is_equal(md_tmp);
	});

	return (it != this->end());
}

IdxList MetaDataList::findTracks(int id) const {

	IdxList ret;
	int idx=0;

	if(id == -1) {
		return ret;
	}

	auto lambda = [&id, &idx, &ret](const MetaData& md) {
		if(md.id == id){
			ret << idx;
		}

		idx++;
	};

	std::for_each(this->begin(), this->end(), lambda);

	return ret;
}

IdxList MetaDataList::findTracks(const QString& path) const {

	IdxList ret;
	int idx=0;

#ifdef Q_OS_UNIX
	Qt::CaseSensitivity sensitivity = Qt::CaseSensitive;
#else
	Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive;
#endif

	auto lambda = [&ret, &idx, &path, &sensitivity](const MetaData& md){
		if(md.filepath().compare(path, sensitivity) == 0){
			ret << idx;
		}

		idx++;
	};

	std::for_each(this->begin(), this->end(), lambda);

	return ret;
}


QStringList MetaDataList::toStringList() const {

	QStringList lst;

	auto lambda = [&lst](const MetaData& md){
		if(md.id >= 0){
			lst << QString::number(md.id);
		}
		else{
			lst << md.filepath();
		}
	};

	std::for_each(this->begin(), this->end(), lambda);

	return lst;
}


MetaDataList& MetaDataList::operator <<(const MetaDataList& v_md)
{
	this->append(v_md);

	return *this;
}


MetaDataList& MetaDataList::operator <<(const MetaData& md)
{
	this->push_back(md);
	return *this;
}

bool MetaDataList::contains(qint32 id) const
{
	auto it = std::find_if(this->begin(), this->end(), [&id](const MetaData& md){
		return (id == md.id);
	});

	return (it != this->end());
}
