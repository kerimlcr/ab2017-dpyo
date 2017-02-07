/* UrlHelper.cpp */

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



#include "UrlHelper.h"
#include <QRegExp>

QString Helper::Url::get_protocol(const QString &url){
	QRegExp re("(.+)://.*");

	if(re.indexIn(url) > 0){
		return re.cap(1);
	}

	return "";
}

QString Helper::Url::get_base_url(const QString &url){
	QRegExp re("(.+://[^/]+).*");

	int idx = re.indexIn(url);
	if(idx < 0){
		return url;
	}

	return re.cap(1);
}

QString Helper::Url::get_filename(const QString &url){
	QRegExp re(".+://.+/(.*)");

	int idx = re.indexIn(url);
	if(idx < 0){
		return "";
	}

	return re.cap(1);
}
