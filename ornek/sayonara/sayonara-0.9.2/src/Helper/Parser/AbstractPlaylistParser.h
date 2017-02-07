/* AbstractPlaylistParser.h */

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



#ifndef ABSTRACTPLAYLISTPARSER_H
#define ABSTRACTPLAYLISTPARSER_H


#include <QString>
#include "Helper/MetaData/MetaDataList.h"

/**
 * @brief The AbstractPlaylistParser class
 * @ingroup PlaylistParser
 */

class AbstractPlaylistParser
{

public:
	AbstractPlaylistParser(const QString& filepath);
	virtual ~AbstractPlaylistParser();

	/**
	 * @brief parse playlist and return found metadata
	 * @param force_parse once if parsed, this function won't parse again and just return the metadata. \n
	 * Set to true if you want to force parsing again
	 * @return list of MetaData
	 */
	virtual MetaDataList get_md(bool force_parse=false) final;


protected:
	MetaDataList		_v_md;
	QString				_file_content;
	QString				_directory;
	QString				_download_target;
	bool				_parsed;


protected:

	/**
	 * @brief here the parsing is done\n
	 * Called by MetaDataList get_md(bool force_parse=false)
	 */
	virtual void parse()=0;

	/**
	 * @brief calculates the absolute filename for a track depending on the path of the playlist file
	 * @param filename as seen in the playlist
	 * @return absolute filename if local file. filename else
	 */
	QString get_absolute_filename(const QString& filename) const;
};

#endif // ABSTRACTPLAYLISTPARSER_H
