/* UrlHelper.h */

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



#ifndef URLHELPER_H
#define URLHELPER_H

#include <QString>

namespace Helper
{
	/**
	 * @brief Helper function for Url handling
	 * @ingroup Helper
	 */
	namespace Url {
		/**
		 * @brief get protocol string without ://
		 * @param url source url
		 * @return the first letters before ://
		 */
		QString get_protocol(const QString& url);

		/**
		 * @brief get url until the first /
		 * @param url source url
		 * @return the first letters before first /, url if there is no /
		 */
		QString get_base_url(const QString& url);

		/**
		 * @brief get everything behind the last /
		 * @param url source filename
		 * @return everything behind last /, or empty string, if there is no /
		 */
		QString get_filename(const QString& url);
	}
}

#endif // URLHELPER_H
