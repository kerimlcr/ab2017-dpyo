/* Bookmark.h */

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



#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QString>

/**
 * @brief The Bookmark helper class
 * @ingroup Bookmarks
 */
class Bookmark {

	private:
		quint32		_time;
		QString		_name;
		bool		_valid;

	public:

		Bookmark(quint32 time=0, const QString& name=QString(), bool valid=false);

		quint32 get_time() const;
		QString get_name() const;
		bool is_valid() const;
};

#endif // BOOKMARK_H
