/* CoverHelper.h */

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



#ifndef COVERHELPER_H
#define COVERHELPER_H

#include <QString>
namespace CoverHelper
{

	QString calc_cover_token(const QString& artist, const QString& album);

	QString calc_google_image_search_address(const QString& searchstring);
	QString calc_google_artist_address(const QString& artist);
	QString calc_google_album_address(const QString& artist, const QString& album);

}

#endif // COVERHELPER_H
