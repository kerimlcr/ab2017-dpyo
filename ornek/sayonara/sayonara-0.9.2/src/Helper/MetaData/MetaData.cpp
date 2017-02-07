/* MetaData.cpp */

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

#include "Helper/MetaData/MetaData.h"

#include <QDir>
#include <QUrl>
#include <QVariant>


#define MD_DO_COPY \
	id = other.id; \
	artist_id = other.artist_id; \
	album_id = other.album_id; \
	title = other.title; \
	artist = other.artist; \
	album = other.album; \
	genres = other.genres; \
	rating = other.rating; \
	length_ms = other.length_ms; \
	year = other.year; \
	_filepath = other.filepath(); \
	track_num = other.track_num; \
	bitrate = other.bitrate; \
	is_extern = other.is_extern; \
	_radio_mode = other.radio_mode(); \
	filesize = other.filesize; \
	discnumber = other.discnumber; \
	n_discs = other.n_discs; \
	pl_playing = other.pl_playing; \
	is_disabled = other.is_disabled; \
	played = other.played;



MetaData::MetaData() :
	LibraryItem()
{

	id = -1;
	artist_id = -1;
	album_id = -1;
	rating = 0;
	length_ms = 0;
	year = 0;
	track_num = 0;
	bitrate = 0;
	is_extern = false;
	_radio_mode = RadioMode::Off;
	filesize = 0;
	discnumber = 0;
	n_discs = 0;

	pl_playing = false;

	is_disabled = false;
	played = false;

}

MetaData::MetaData(const MetaData & other) :
	LibraryItem(other)
{
	MD_DO_COPY
}


MetaData::MetaData(MetaData&& other) :
	LibraryItem(other)
{
	MD_DO_COPY
}

MetaData::MetaData(const QString& path)
{
	this->set_filepath(path);
}

MetaData::~MetaData() {

}

QString MetaData::to_string() const {
	return	title + " by " +
			artist + " from " +
			album +
			//" (" + QString::number(length_ms) + " m_sec) :: " +
			//_filepath +
			"";

}

QVariant MetaData::toVariant(const MetaData& md) {

	QVariant v;

	v.setValue<MetaData>(md);

	return v;
}

bool MetaData::fromVariant(const QVariant& v, MetaData& md) {

	if(! v.canConvert<MetaData>() ) {
		return false;
	}

	md = v.value<MetaData>() ;
	return true;
}

MetaData& MetaData::operator=(const MetaData& other)
{
	LibraryItem::operator=(other);
	MD_DO_COPY
	return *this;
}



bool MetaData::operator==(const MetaData& md) const {
	return this->is_equal(md);
}


bool MetaData::operator!=(const MetaData& md) const {
	return !(this->is_equal(md));
}



bool MetaData::is_equal(const MetaData& md) const {

	QDir first_path(_filepath);
	QDir other_path(md.filepath());

	QString s_first_path = first_path.absolutePath();
	QString s_other_path = other_path.absolutePath();



#ifdef Q_OS_UNIX
	return (s_first_path.compare(s_other_path) == 0);
#else
	return (s_first_path.compare(s_other_path, Qt::CaseInsensitive) == 0);
#endif

}

bool MetaData::is_equal_deep(const MetaData& md) const{

	return ( (id == md.id)  &&
			 ( artist_id == md.artist_id ) &&
			 ( album_id == md.album_id ) &&
			 ( title == md.title ) &&
			 ( artist == md.artist ) &&
			 ( album == md.album ) &&
			 ( genres == md.genres ) &&
			 ( rating == md.rating ) &&
			 ( length_ms == md.length_ms ) &&
			 ( year == md.year ) &&
			 ( filepath() == md.filepath() ) &&
			 ( track_num == md.track_num ) &&
			 ( bitrate == md.bitrate ) &&
			 ( is_extern == md.is_extern ) &&
			 ( _radio_mode == md.radio_mode() ) &&
			 ( filesize == md.filesize ) &&
			 //( comment == md.comment ) &&
			 ( discnumber == md.discnumber ) &&
			 ( n_discs == md.n_discs ) &&

			 ( pl_playing == md.pl_playing ) &&

			 ( is_disabled == md.is_disabled )
			 );
}

QString MetaData::filepath() const{
	return _filepath;
}


QString MetaData::set_filepath(QString filepath){

	bool is_local_path = false;

#ifdef Q_OS_UNIX
	if(filepath.startsWith("/")){
		is_local_path = true;
	}
#else
	if(filepath.contains(":\\") || filepath.contains("\\\\")){
		is_local_path = true;
	}
#endif

	if(is_local_path){
		QDir dir(filepath);
		_filepath = dir.absolutePath();
		_radio_mode = RadioMode::Off;
	}

	else if(filepath.contains("soundcloud.com")){
		_filepath = filepath;
		_radio_mode = RadioMode::Soundcloud;
	}

	else{
		_filepath = filepath;
		_radio_mode = RadioMode::Station;
	}

	return _filepath;
}

RadioMode MetaData::radio_mode() const {
	return _radio_mode;
}

