/* CoverHelper.cpp */

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



#include "CoverHelper.h"
#include "Helper/Helper.h"
#include <QRegExp>
#include <QUrl>

QString CoverHelper::calc_google_image_search_address(const QString& str) {

	QString searchstring = str;
	searchstring.replace(" ", "%20");
	searchstring.replace("/", "%2F");
	searchstring.replace("&", "%26");
	searchstring.replace("$", "%24");

	QString url = QString("https://www.google.de/search?num=20&hl=de&site=imghp&tbm=isch&source=hp");

	url += QString("&q=") + searchstring;
	url += QString("&oq=") + searchstring;

	return url;
}


QString CoverHelper::calc_google_artist_address(const QString& artist) {

	return calc_google_image_search_address(QUrl::toPercentEncoding(artist));
}


QString CoverHelper::calc_google_album_address(const QString& artist, const QString& album) {

	QString new_album, searchstring;
	QRegExp regex;

	searchstring = QUrl::toPercentEncoding(artist);
	new_album = album;

	regex = QRegExp(QString("(\\s)?-?(\\s)?((cd)|(CD)|((d|D)((is)|(IS))(c|C|k|K)))(\\d|(\\s\\d))"));

	new_album = new_album.toLower();
	new_album = new_album.remove(regex);
	new_album = new_album.replace("()", "");
	new_album = new_album.replace("( )", "");
	new_album = new_album.trimmed();
	new_album = QUrl::toPercentEncoding(album);

	if(searchstring.size() > 0) {
		searchstring += "+";
	}

	searchstring += new_album;

	return calc_google_image_search_address(searchstring);
}


QString CoverHelper::calc_cover_token(const QString& artist, const QString& album) {

	QByteArray str = QString(artist.trimmed() + album.trimmed()).toLower().toUtf8();

	return Helper::calc_hash(str);
}


