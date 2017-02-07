/* id3.h */

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


#ifndef TAGGING_H_
#define TAGGING_H_

#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/fileref.h>

#include <QtGlobal>
#include <QImage>

class MetaData;
/**
 * @brief Tagging namespace
 * @ingroup Tagging
 */
namespace Tagging
{

	/**
	 * @brief The Quality enum
	 */
	enum class Quality : quint8 {

		Fast=TagLib::AudioProperties::Fast,
		Standard=TagLib::AudioProperties::Average,
		Quality=TagLib::AudioProperties::Accurate,
		Dirty
	};

	enum class TagType : quint8 {
		ID3v1=0,
		ID3v2,
		APE,
		Other,
		Unknown
	};

	/**
	 * @brief get metadata of file. Filepath should be given within the MetaData struct
	 * @param md MetaData that will be filled
	 * @param quality fast, normal, accurate
	 * @return true, if metadata could be filled. false else
	 */
	bool getMetaDataOfFile(MetaData& md, Tagging::Quality quality=Tagging::Quality::Standard);

	/**
	 * @brief writes metadata into file specivied in MetaData::_filepath
	 * @param md MetaData struct to write
	 * @return true if metadata could be written. false else
	 */
	bool setMetaDataOfFile(const MetaData& md);

	bool write_cover(const MetaData& md, const QImage& image);
	bool write_cover(const MetaData& md, const QString& image_path);

	bool extract_cover(const MetaData& md, QByteArray& cover_data, QString& mime_type);

	Tagging::TagType get_tag_type(const MetaData& md);
	Tagging::TagType get_tag_type(const QString& filepath);

	bool is_valid_file(const TagLib::FileRef& f);
}


#endif
