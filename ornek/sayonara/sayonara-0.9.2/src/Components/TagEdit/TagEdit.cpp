/* TagEdit.cpp */

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



#include "TagEdit.h"
#include "MetaDataChangeNotifier.h"
#include "Helper/Tagging/Tagging.h"
#include "Database/DatabaseConnector.h"

#include <algorithm>

TagEdit::TagEdit(QObject *parent) :
	QThread(parent),
	SayonaraClass()
{
	_ldb = DB::getInstance()->get_std();
	_notify = true;

	connect(this, &QThread::finished, this, &TagEdit::thread_finished);
}

TagEdit::TagEdit(const MetaDataList& v_md, QObject* parent) :
	TagEdit(parent)
{
	set_metadata(v_md);
}

TagEdit::~TagEdit(){

}


void TagEdit::update_track(int idx, const MetaData& md){

	_changed_md[idx] = !( md.is_equal_deep(_v_md_orig[idx]) );
	_v_md[idx] = md;
}

void TagEdit::undo(int idx){
	_v_md[idx] = _v_md_orig[idx];
}


void TagEdit::undo_all(){
	_v_md = _v_md_orig;
}

const MetaData& TagEdit::get_metadata(int idx) const
{
	return _v_md[idx];
}

const MetaDataList& TagEdit::get_all_metadata() const
{
	return _v_md;
}


int TagEdit::get_n_tracks() const
{
	return _v_md.size();
}

void TagEdit::add_genre_to_metadata(const QString &genre)
{
	int i=0;
	for(MetaData& md : _v_md) {

		bool already_there;

		already_there = std::any_of(md.genres.cbegin(), md.genres.cend(), [&genre](const QString& g)
		{
			return (g.compare(genre, Qt::CaseInsensitive) == 0);
		});

		if(!already_there){
			md.genres << genre;
			_changed_md[i] = true;
		}

		i++;
	}
}

void TagEdit::remove_genre_from_metadata(const QString& genre){

	for(int i=0; i<_v_md.size(); i++){

		int entries_removed;
		entries_removed = _v_md[i].genres.removeAll(genre);

		if(entries_removed > 0){
			_changed_md[i] = true;
		}
	}
}


void TagEdit::set_metadata(const MetaDataList& v_md){
	_v_md = v_md;
	_v_md_orig = v_md;

	_cover_map.clear();
	_changed_md.clear();

	if(v_md.size() > 0){
		_ldb = DB::getInstance()->get(v_md[0]);
	}

	for(int i=0; i<v_md.size(); i++){
		_changed_md << false;
	}

	emit sig_metadata_received(_v_md);
}

void TagEdit::set_auto_notify(bool b)
{
	_notify = b;
}

void TagEdit::check_for_new_artists_and_albums(QStringList& new_artists, QStringList& new_albums){

	QStringList artists;
	QStringList albums;

	// first gather all artists and albums
	for(const MetaData& md : _v_md){

		if(md.is_extern) continue;

		if(!artists.contains(md.artist)){
			artists << md.artist;
		}

		if(!albums.contains(md.album)){
			albums << md.album;
		}
	}

	for(const QString& album_name : albums){
		int id = _ldb->getAlbumID(album_name);
		//sp_log(Log::Debug) << "Album: " << album_name << ": " << id;
		if(id < 0) {
			new_albums << album_name;
		}
	}

	for(const QString& artist_name : artists){
		int id = _ldb->getArtistID(artist_name);
		if(id < 0) {
			new_artists << artist_name;
		}
	}
}


void TagEdit::insert_new_artists(const QStringList& artists){

	for(const QString& a : artists){
		_ldb->insertArtistIntoDatabase(a);
	}
}

void TagEdit::insert_new_albums(const QStringList& albums){

	for(const QString& a : albums){
		_ldb->insertAlbumIntoDatabase(a);
	}
}

void TagEdit::apply_artists_and_albums_to_md(){

	for(int i=0; i<_v_md.size(); i++){

		if( _changed_md[i] == false ) {
			continue;
		}

		int artist_id, album_id;
		artist_id = _ldb->getArtistID(_v_md[i].artist);
		album_id = _ldb->getAlbumID(_v_md[i].album);

		_v_md[i].album_id = album_id;
		_v_md[i].artist_id = artist_id;
	}
}


void TagEdit::update_cover(int idx, const QImage& cover){

	if(!between(idx, _v_md) ){
		return;
	}

	Tagging::TagType t = Tagging::get_tag_type(_v_md[idx]);
	if(t != Tagging::TagType::ID3v2) {
		return;
	}

	if(!is_id3v2_tag(idx)){
		return;
	}

	_cover_map[idx] = cover;
}

void TagEdit::remove_cover(int idx){
	_cover_map.remove(idx);
}

bool TagEdit::has_cover_replacement(int idx) const
{
	return _cover_map.contains(idx);
}

bool TagEdit::is_id3v2_tag(int idx) const
{
	if(!between(idx, _v_md)){
		return false;
	}

	Tagging::TagType t = Tagging::get_tag_type(_v_md[idx]);

	return (t == Tagging::TagType::ID3v2);
}

void TagEdit::run()
{

	MetaDataList v_md;
	MetaDataList v_md_orig;
	DatabaseConnector* db;
	QStringList new_artists, new_albums;

	check_for_new_artists_and_albums(new_artists, new_albums);

	insert_new_albums(new_albums);
	insert_new_artists(new_artists);

	apply_artists_and_albums_to_md();

	int i=0;
	int n_operations = _v_md.size() + _cover_map.size();
	for(i=0; i<_v_md.size(); i++){

		MetaData md = _v_md[i];
		emit sig_progress( (i * 100) / n_operations);

		if( _changed_md[i] == false ) {
			continue;
		}

		bool success = Tagging::setMetaDataOfFile(md);

		if( !success ) {
			continue;
		}

		if( !md.is_extern && md.id >= 0 ){
			success = _ldb->updateTrack(md);
		}

		if(success){
			v_md << std::move(md);
			v_md_orig.push_back(_v_md_orig[i]);
		}
	}

	for(int idx : _cover_map.keys()){
		
		Tagging::write_cover(_v_md[idx], _cover_map[idx]);
		emit sig_progress( (i++ * 100) / n_operations);
	}

	_ldb->createIndexes();

	db = DatabaseConnector::getInstance();
	db->clean_up();

	_v_md_after_change = v_md;
	_v_md_before_change = v_md_orig;
	_v_md_orig = _v_md;

	emit sig_progress(-1);
}

void TagEdit::thread_finished(){
	if(_notify){
		MetaDataChangeNotifier::getInstance()->change_metadata(_v_md_before_change, _v_md_after_change);
	}
}

void TagEdit::commit(){
	this->start();
}


