/* CoverLocation.h */

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



#ifndef COVERLOCATION_H
#define COVERLOCATION_H

#include <QString>
#include <QStringList>
#include <QMetaType>
#include <QUrl>

class MetaData;
class Album;
class Artist;
/**
 * @brief The CoverLocation class
 * @ingroup Covers
 */
class CoverLocation
{

private:

	/**
	 * @brief The search engine term what to search
	 */
	QString			_search_term;

	/**
	 * @brief search_url url where to fetch covers
	 */
	QString			_search_url;

	/**
	 * @brief cover_path path, in .Sayonara, where cover is stored. Ignored if local_paths are not empty
	 */
	QString			_cover_path;

	/**
	 * @brief local_paths paths where images can be fetched from if they should not be fetched from the .Sayonara directory
	 */
	QStringList		_local_paths;

	/**
	 * @brief valid if CoverLocation object contains a valid download url
	 */
	bool			_valid;


public:
	CoverLocation();
	CoverLocation(const CoverLocation&);

	void print() const;
    QString toString() const;


	bool valid() const;
	QStringList local_paths() const;
	QString local_path(int idx) const;
	QString cover_path() const;
	QString search_url() const;
	QString search_term() const;
	void set_search_term(const QString& search_term);


	/**
	 * @brief creates CoverLocation by taking the md5 sum between album_name and artist_name
	 * @param album_name Album name
	 * @param artist_name Artist name
	 * @return CoverLocation object
	 */
	static CoverLocation get_cover_location(const QString& album_name, const QString& artist_name);

	/**
	 * @brief overloaded. Picks major artist out of artists and calls
	 *   get_cover_location(const QString& album_name, const QString& artist_name)
	 * @param album_name Album name
	 * @param artists List of artists
	 * @return CoverLocation object
	 */
	static CoverLocation get_cover_location(const QString& album_name, const QStringList& artists);


	/**
	 * @brief overloaded. Calls
	 *   get_cover_location(const QString& album_name, const QStringList& artists)
	 * @param album
	 * @return CoverLocation object
	 */
	static CoverLocation get_cover_location(const Album& album);


	/**
	 * @brief overloaded. Fetches album from database and calls
	 *  get_cover_location(const QString& album_name, const QStringList& artists)
	 * @param album_id Album ID
	 * @param db_id Database ID for album
	 * @return CoverLocation object
	 */
	static CoverLocation get_cover_location(int album_id, quint8 db_id);


	/**
	 * @brief Creates cover token of the form artist_<md5sum of artist>
	 * @param artist Artist name
	 * @return CoverLocation object
	 */
	static CoverLocation get_cover_location(const QString& artist);


	/**
	 * @brief overloaded. extracts artist name and calls
	 *   get_cover_location(const QString& artist)
	 * @param artist Artist object
	 * @return CoverLocation object
	 */
	static CoverLocation get_cover_location(const Artist& artist);


	/**
	 * @brief overloaded.
	 *   if MetaData::album_id < 0 calls
	 *     get_cover_location(const QString& album_name, const QString& artist_name)
	 *   else extract Album from database and calls
	 *     get_cover_location(const Album& album)
	 * @param Metadata object
	 * @return  CoverLocation object
	 */
	static CoverLocation get_cover_location(const MetaData& md);


	/**
	 * @brief fetch a cover from a specific url
	 * @param url url, the cover has to be fetched from
	 * @param target_path path where the found image has to be saved
	 * @return CoverLocation object
	 */
	static CoverLocation get_cover_location(const QUrl& url, const QString& target_path);


	/**
	 * @brief fetch a cover for a specific search string
	 * @param search_string the string where the cover has to be fetched for
	 * @param target_path path where the found image has to be saved
	 * @return CoverLocation object
	 */
	static CoverLocation get_cover_location_by_searchstring(const QString& search_string, const QString& target_path);


	/**
	 * @brief returns an invalid location
	 * @return  CoverLocation object
	 */
	static CoverLocation getInvalidLocation();

	/**
	 * @brief returns if path is the same as the invalid location path
	 * @param cover_path the path to be compared
	 * @return
	 */
	static bool isInvalidLocation(const QString& cover_path);


	/**
	 * @brief returns the standard cover directory
	 * @return usually ~/.Sayonara/covers
	 */
	static QString get_cover_directory();

};

Q_DECLARE_METATYPE(CoverLocation)
#endif // COVERLOCATION_H
