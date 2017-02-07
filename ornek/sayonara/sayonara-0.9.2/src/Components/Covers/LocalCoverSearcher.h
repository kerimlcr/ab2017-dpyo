/* LocalCoverSearcher.h */

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

#ifndef LOCALCOVERSEARCHER_H
#define LOCALCOVERSEARCHER_H

#include <QStringList>

/**
 * @brief Helper Namespace for finding covers in a specific directory
 * @ingroup Covers
 */
namespace LocalCoverSearcher
{

	/**
	 * @brief Determines if filepath is a dir or a file and calls
	 *   get_local_cover_paths_from_filename or get_local_cover_paths_from_dirname
	 * @param filepath Filepath may be a directory or a filename
	 * @return Paths for all covers in that directory
	 */
	QStringList get_local_cover_paths(const QString& filepath);

	/**
	 * @brief Extracts dirname and calls get_local_cover_paths_from_dirname
	 * @param filename filename of a file within the same directory. E.g. a mp3 file
	 * @return Paths for all covers in that directory
	 */
	QStringList get_local_cover_paths_from_filename(const QString& filename);

	/**
	 * @brief Searches the best cover by name from the directory
	 * @param dirname directory where to search the cover
	 * @return Paths for all covers in that directory
	 */
	QStringList get_local_cover_paths_from_dirname(const QString& dirname);
}

#endif // LOCALCOVERSEARCHER_H
