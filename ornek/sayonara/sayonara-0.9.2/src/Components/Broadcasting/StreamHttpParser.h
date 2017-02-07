/* StreamHttpParser.h */

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



#ifndef STREAMHTTPPARSER_H
#define STREAMHTTPPARSER_H

#include <QByteArray>
#include <QString>

/**
 * @brief Parses requests out of the Client.
 * @ingroup Broadcasting
 */
class StreamHttpParser
{

public:
	/**
	 * @brief The HttpAnswer enum
	 */
	enum class HttpAnswer : quint8 {
		Fail=0,
		OK,
		Reject,
		Ignore,
		Playlist,
		HTML5,
		MP3,
		BG,
		Favicon,
		MetaData
	};


private:

	bool		_icy;
	QString		_host;
	QString		_user_agent;
	HttpAnswer	_status;


public:
	StreamHttpParser();
	~StreamHttpParser();

	HttpAnswer parse(const QByteArray& data);

	QString		get_host() const;
	QString		get_user_agent() const;
	bool		is_icy() const;
};

#endif // STREAMHTTPPARSER_H
