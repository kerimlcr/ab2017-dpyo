/* id3.cpp */

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


#include "Tagging.h"
#include "Frames/Popularimeter.h"
#include "Frames/Discnumber.h"
#include "Frames/Cover.h"

#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaData.h"

#include <taglib/tbytevector.h>
#include <taglib/tbytevectorstream.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>

#include <QFile>

bool Tagging::is_valid_file(const TagLib::FileRef& f)
{
	if( f.isNull() ||
		!f.tag() ||
		!f.file() ||
		!f.file()->isValid() )
	{
		return false;
	}

	return true;
}

bool Tagging::getMetaDataOfFile(MetaData& md, Tagging::Quality quality) {

	bool success;

	QFileInfo fi(md.filepath());
	md.filesize = fi.size();

	TagLib::AudioProperties::ReadStyle read_style;
	bool read_audio_props=true;

	switch(quality){
		case Tagging::Quality::Quality:
			read_style = TagLib::AudioProperties::Accurate;
			break;
		case Tagging::Quality::Standard:
			read_style = TagLib::AudioProperties::Average;
			break;
		case Tagging::Quality::Fast:
			read_style = TagLib::AudioProperties::Fast;
			break;
		case Tagging::Quality::Dirty:
			read_style = TagLib::AudioProperties::Fast;
			read_audio_props = false;
			break;
		default:
			read_style = TagLib::AudioProperties::Average;
	};

	TagLib::FileRef f(
			TagLib::FileName(md.filepath().toUtf8()),
			read_audio_props,
			read_style
	);

	if(!is_valid_file(f)){
		sp_log(Log::Warning) << "Cannot open tags for " << md.filepath();
		return false;
	}

	TagLib::Tag* tag = f.tag();

	QString artist = QString::fromUtf8(tag->artist().toCString(true));
	QString album = QString::fromUtf8(tag->album().toCString(true));
	QString title = QString::fromUtf8(tag->title().toCString(true));
	QString genre = QString::fromUtf8(tag->genre().toCString(true));
	QString comment = QString::fromUtf8(tag->comment().toCString(true));

	ID3v2Frame::PopularimeterFrame popularimeter_frame(&f);
	ID3v2Frame::Popularimeter popularimeter;
	success = popularimeter_frame.read(popularimeter);
	if(success){
		md.rating = popularimeter.get_sayonara_rating();
	}

	ID3v2Frame::DiscnumberFrame discnumber_frame(&f);
	ID3v2Frame::Discnumber discnumber;
	success = discnumber_frame.read(discnumber);
	if(success){
		md.discnumber = discnumber.disc;
		md.n_discs = discnumber.n_discs;
	}

	uint year = tag->year();
	uint track = tag->track();

	int bitrate=0;
	int length=0;

	if( quality != Tagging::Quality::Dirty ){
		bitrate = f.audioProperties()->bitrate() * 1000;
		length = f.audioProperties()->length() * 1000;
	}


	QStringList genres;
	QString genre_str = Helper::cvt_str_to_first_upper(genre);
	genres = genre_str.split(QRegExp(",|/|;"));
	for(int i=0; i<genres.size(); i++) {
		genres[i] = genres[i].trimmed();
	}

	genres.removeDuplicates();
	genres.removeAll("");

	md.album = album;
	md.artist = artist;
	md.title = title;
	md.length_ms = length;
	md.year = year;
	md.track_num = track;
	md.bitrate = bitrate;
	md.genres = genres;
	md.discnumber = discnumber.disc;
	md.n_discs = discnumber.n_discs;
	//md.comment = comment;
	md.rating = popularimeter.get_sayonara_rating();

	if(md.title.length() == 0) {
		QString dir, filename;
		Helper::File::split_filename(md.filepath(), dir, filename);

		if(filename.size() > 4){
			filename = filename.left(filename.length() - 4);
		}

		md.title = filename;
	}

	return true;
}


bool Tagging::setMetaDataOfFile(const MetaData& md) {

	QString filepath = md.filepath();
	TagLib::FileRef f(TagLib::FileName(filepath.toUtf8()));
	if(!is_valid_file(f)){
		sp_log(Log::Warning) << "Cannot open tags for " << md.filepath();
		return false;
	}


	TagLib::String album(md.album.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String artist(md.artist.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String title(md.title.toUtf8().data(), TagLib::String::UTF8);
	TagLib::String genre(md.genres.join(",").toUtf8().data(), TagLib::String::UTF8);

	f.tag()->setAlbum(album);
	f.tag()->setArtist(artist);
	f.tag()->setTitle(title);
	f.tag()->setGenre(genre);
	f.tag()->setYear(md.year);
	f.tag()->setTrack(md.track_num);

	ID3v2Frame::Popularimeter popularimeter("sayonara player", 0, 0);
	ID3v2Frame::PopularimeterFrame popularimeter_frame(&f);
	popularimeter.set_sayonara_rating(md.rating);
	popularimeter_frame.write(popularimeter);

	ID3v2Frame::Discnumber discnumber(md.discnumber, md.n_discs);
	ID3v2Frame::DiscnumberFrame discnumber_frame(&f);
	discnumber_frame.write(discnumber);

	f.save();

	return true;
}



bool Tagging::write_cover(const MetaData& md, const QImage& cover){
	bool success;
	QString filepath = Helper::get_sayonara_path() + "tmp.png";

	success = cover.save(filepath);
	if(!success){
		sp_log(Log::Warning) << "Can not save temporary cover: " << filepath;
		sp_log(Log::Warning) << "Is image valid? " << !cover.isNull();
		return false;
	}

	success = write_cover(md, filepath);
	QFile::remove(filepath);

	return success;
}


bool Tagging::write_cover(const MetaData& md, const QString& cover_image_path){

	QString error_msg = "Cannot save cover. ";
	QString filepath = md.filepath();
	TagLib::FileRef f(TagLib::FileName(filepath.toUtf8()));
	if(!is_valid_file(f)){
		sp_log(Log::Warning) << "Cannot open tags for " << md.filepath();
		return false;
	}


	QByteArray data;
	bool success = Helper::File::read_file_into_byte_arr(cover_image_path, data);
	if(data.isEmpty() || !success){
		sp_log(Log::Warning) << error_msg << "No image data available: " << cover_image_path;
		return false;
	}

	QString mime_type = "image/";
	QString ext = Helper::File::get_file_extension(cover_image_path);
	if(ext.compare("jpg", Qt::CaseInsensitive) == 0){
		mime_type += "jpeg";
	}

	else if(ext.compare("png", Qt::CaseInsensitive) == 0){
		mime_type += "png";
	}

	else{
		sp_log(Log::Warning) << error_msg << "Unknown mimetype: '" << ext << "'";
		return false;
	}

	ID3v2Frame::Cover cover(mime_type, data);
	ID3v2Frame::CoverFrame cover_frame(&f);

	cover_frame.write(cover);
	return f.save();
}

bool Tagging::extract_cover(const MetaData &md, QByteArray& cover_data, QString& mime_type){

	QString error_msg = "Cannot fetch cover. ";
	QString filepath = md.filepath();
	TagLib::FileRef f(TagLib::FileName(filepath.toUtf8()));

	if(!is_valid_file(f)){
		sp_log(Log::Warning) << "Cannot open tags for " << md.filepath();
		return false;
	}

	ID3v2Frame::Cover cover;
	ID3v2Frame::CoverFrame cover_frame(&f);

	if(!cover_frame.is_frame_found()){
		return false;
	}

	cover_frame.read(cover);
	cover_data = cover.image_data;
	mime_type = cover.mime_type;

	return !(cover_data.isEmpty());
}


Tagging::TagType Tagging::get_tag_type(const QString& filepath){
	TagLib::FileRef f(TagLib::FileName(filepath.toUtf8()));
	if(!is_valid_file(f)){
		sp_log(Log::Warning) << "Cannot open tags for " << filepath;
		return Tagging::TagType::Unknown;
	}

	TagLib::MPEG::File* mpeg_file;
	mpeg_file = dynamic_cast<TagLib::MPEG::File*>(f.file());
	if(!mpeg_file){
		return Tagging::TagType::Other;
	}

	if(mpeg_file->hasID3v2Tag()){
		return Tagging::TagType::ID3v2;
	}

	if(mpeg_file->hasID3v1Tag()){
		return Tagging::TagType::ID3v1;
	}

	if(mpeg_file->hasAPETag()){
		return Tagging::TagType::APE;
	}

	return Tagging::TagType::Other;
}


Tagging::TagType Tagging::get_tag_type(const MetaData& md){

	QString filepath = md.filepath();
	return get_tag_type(filepath);
}
