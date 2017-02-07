/* AbstractPlaylistParser.cpp */

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



#include "AbstractPlaylistParser.h"
#include "Helper/FileHelper.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "PlaylistParser.h"
#include "Helper/MetaData/MetaDataList.h"


AbstractPlaylistParser::AbstractPlaylistParser(const QString& filename)
{
	QString pure_file;

	Helper::File::split_filename(filename, _directory, pure_file);
	Helper::File::read_file_into_str(filename, _file_content);
	_parsed = false;
}

AbstractPlaylistParser::~AbstractPlaylistParser(){

}

MetaDataList AbstractPlaylistParser::get_md(bool force_parse) {

	if(force_parse){
		_parsed = false;
	}

	if(!_parsed){
		_v_md.clear();
		parse();
	}

	_parsed = true;


	return _v_md;
}


QString AbstractPlaylistParser::get_absolute_filename(const QString& filename) const
{
	QString ret;

	if(filename.isEmpty()){
		return "";
	}

	if(Helper::File::is_www(filename)){
		return filename;
	}

	if(!Helper::File::is_absolute(filename)){
		ret = _directory + QDir::separator() + filename;
	}
	else{
		ret = filename;
	}

	if(!QFile::exists(ret)){
		ret = "";
	}

	return ret;
}
