
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




#include "MTP_Track.h"

#include "Helper/FileHelper.h"
#include "Helper/Tagging/Tagging.h"



MTP_Track::MTP_Track(const MetaData &md){
	_track = nullptr;
	read_metadata(md);
}

MTP_Track::MTP_Track(const QString& filename){

	MetaData md;
	bool success;

	md.set_filepath(filename);
	success = Tagging::getMetaDataOfFile(md);

	if(success){
		read_metadata(md);
	}
	else{
		_track = nullptr;
	}
}

MTP_Track::~MTP_Track(){
	if(_track){
		LIBMTP_destroy_track_t(_track);
		_track = nullptr;
	}
}


void MTP_Track::read_metadata(const MetaData &md){
	_src_filename = md.filepath();

	_track = LIBMTP_new_track_t();

	_track->album = strndup(md.album.toUtf8().data(), md.album.size());
	_track->artist = strndup(md.artist.toUtf8().data(), md.artist.size());
	_track->bitrate = md.bitrate;
	_track->bitratetype = 1; //constant
	_track->composer = strndup(_track->artist, md.artist.size());
	_track->date = strndup("no", 2);
	_track->duration = (quint32) md.length_ms;
	_track->filesize = md.filesize;

	QString dir, filename;
	Helper::File::split_filename(md.filepath(), dir, filename);
	sp_log(Log::Debug) << "New filename : " << filename;

	_track->filename = strndup(filename.toUtf8().data(), filename.size());

	QString genre = md.genres.join(",");
	_track->genre = strndup(genre.toUtf8().data(), genre.size());

	// _track->item_id
	_track->next = nullptr;
	_track->nochannels = 2;
	// _track->parent_id
	_track->rating = md.rating;
	_track->samplerate = 44100;
	// _track->storage_id
	_track->title = strndup(md.title.toUtf8().data(), md.title.size());
	_track->tracknumber = md.track_num;
	_track->usecount = 0;
	_track->filetype = LIBMTP_FILETYPE_MP3;
	// WAVE = 0x57 0x41 0x56 0x45 = 1463899717
	// RIFF = 0x52 0x49 0x46 0x46 = 1380533830
	_track->wavecodec = 1380533830;
}

QString MTP_Track::src_filename() const
{
	return _src_filename;
}

MTPIntern_Track* MTP_Track::metadata() const
{
	return _track;
}

void MTP_Track::set_folder_id(quint32 folder_id){
	if(!_track){
		return;
	}

	_track->parent_id = folder_id;
}

void MTP_Track::set_storage_id(quint32 storage_id){
	if(!_track){
		return;
	}

	_track->storage_id = storage_id;
}
