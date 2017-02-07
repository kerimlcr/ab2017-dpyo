/* FileHelper.h */

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

#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>

namespace Helper
{

	/**
	 * @brief Filehelper functions
	 * @ingroup Helper
	 */
	namespace File
	{
		/**
		 * @brief formatter for filepaths. Removes double separators and replaces them with os specific separators.
		 * @param filename input filename
		 * @return nicely formatted filename
		 */
		QString			clean_filename(const QString& filename);

		/**
		 * @brief calc_file_extension
		 * @param filename
		 * @return
		 */
		QString			calc_file_extension(const QString& filename);

		/**
		 * @brief Remove all files from directory
		 * @param dir_name directory name
		 * @param filters file name filters
		 */
		void			remove_files_in_directory(const QString& dir_name, const QStringList& filters=QStringList());

		/**
		 * @brief Remove all given files (also directories can be specified)
		 * @param files list of files
		 */
		void			delete_files(const QStringList& files);

		/**
		 * @brief get parent directory of a filepath
		 * @param path File- or directory path
		 * @return
		 */
		QString			get_parent_directory(const QString& path);

		/**
		 * @brief extract pure filename from a complete file path
		 * @param path complete file path
		 * @return pure filename
		 */
		QString			get_filename_of_path(const QString& path);

		/**
		 * @brief split filename into the dir and filename
		 * @param src
		 * @param path
		 * @param filename
		 */
		void			split_filename(const QString& src, QString& dir, QString& filename);

		/**
		 * @brief get file extension
		 * @param filename filename to get the extension for
		 * @return extension string
		 */
		QString			get_file_extension(const QString& filename);

		/**
		 * @brief extract parent folder of a file list (see also get_parent_directory(const QString& path)
		 * @param list file list
		 * @return List of parent folders
		 */
		QStringList		get_parent_directories(const QStringList& list);

		/**
		 * @brief get absolute filename of file
		 * @param filename
		 * @return
		 */
		QString			get_absolute_filename(const QString& filename);

		/**
		 * @brief create all directories necessary to access path
		 * @param path full target path
		 */
		bool			create_directories(const QString& path);


		/**
		 * @brief convert filesize to string
		 * @param filesize in bytes
		 * @return converted string
		 */
		QString			calc_filesize_str(quint64 filesize);


		/**
		 * @brief Tell whether filename is absolute
		 * @param filename the filename to check
		 * @return true if filename is absolute, false else
		 */
		bool			is_absolute(const QString& filename);


		/**
		 * @brief Write raw data to file
		 * @param raw_data raw data
		 * @param filename target_filename
		 * @return true if successful, false else
		 */
		bool			write_file(const QByteArray& raw_data, const QString& filename);

		/**
		 * @brief read a complete file into a string
		 * @param filename filename
		 * @param content target reference to content
		 * @return true if file could be read, false else
		 */
		bool read_file_into_str(const QString& filename, QString& content);

		/**
		 * @brief read a complete file into a byte array
		 * @param filename filename
		 * @param content target reference to content
		 * @return true if file could be read, false else
		 */
		bool read_file_into_byte_arr(const QString& filename, QByteArray& content);

		/**
		 * @brief Check, if file is valid. Web URLs are always valid
		 * @param filepath path to file or resource
		 * @return true, if file exists or if Web URL. false else
		 */
		bool check_file(const QString& filepath);

		// Everything clear
		bool is_url(const QString& str);
		bool is_www(const QString& str);
		bool is_file(const QString& filename);
		bool is_dir(const QString& filename);
		bool is_soundfile(const QString& filename);
		bool is_playlistfile(const QString& filename);
		bool is_podcastfile(const QString& filename, const QByteArray& content);
	}
}

#endif // FILEHELPER_H
