/* SomaFMStation.h */

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



/* SomaFMStation.h */

#ifndef SOMAFMSTATION_H
#define SOMAFMSTATION_H

#include "Helper/MetaData/MetaDataList.h"
#include "Components/Covers/CoverLocation.h"

class SomaFMStation
{
public:

	enum class UrlType : quint8
	{
		AAC=0,
		MP3,
		Undefined
	};

	SomaFMStation();
	SomaFMStation(const QString& content);

	QString get_name() const;
	QStringList get_urls() const;
	QString get_description() const;
	UrlType get_url_type(const QString& url) const;
	CoverLocation get_cover_location() const;
	bool is_valid() const;
	MetaDataList get_metadata() const;
	void set_metadata(const MetaDataList& v_md);

	void set_loved(bool loved);
	bool is_loved() const;


private:

	QString			_content;

	QString			_station_name;
	QMap<QString, UrlType> _urls;
	QString			_description;
	CoverLocation	_cover;
	MetaDataList	_v_md;
	bool			_loved;

private:
	void parse_station_name();
	void parse_urls();
	void parse_description();
	void parse_image();
};

#endif
