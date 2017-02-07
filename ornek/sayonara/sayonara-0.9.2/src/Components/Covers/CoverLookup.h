/* CoverLookup.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * CoverLookup.h
 *
 *  Created on: Apr 4, 2011
 *      Author: Lucio Carreras
 */

#ifndef COVERLOOKUP_H_
#define COVERLOOKUP_H_

#include <QThread>
#include <QPixmap>
#include <QDir>

#include <memory>

class Album;
class Artist;
class CoverLocation;
/**
 * @brief The CoverLookupInterface class
 * @ingroup Covers
 */
class CoverLookupInterface : public QObject{

	Q_OBJECT

signals:
	void sig_cover_found(const QString& file_path);
	void sig_finished(bool success);

public slots:
	virtual void stop()=0;

public:
	CoverLookupInterface(QObject* parent=nullptr);

};

/**
 * @brief The CoverLookup class
 * @ingroup Covers
 */
class CoverLookup : public CoverLookupInterface {

	Q_OBJECT


private slots:
	/**
	 * @brief called when CoverFetchThread has found cover
	 * @param cl
	 */
	void cover_found(const QString& cover_path);

	/**
	 * @brief called when CoverFetchThread has finished
	 */
	void finished(bool);


public:


	CoverLookup(QObject* parent=nullptr, int n_covers=1);
	virtual ~CoverLookup();

	/**
	 * @brief fetches cover for a CoverLocation.
	 *   1. Looks up CoverLocation::cover_path
	 *   2. Looks up CoverLocation::local_paths
	 *   3. Starts a CoverFetchThread
	 * @param cl CoverLocation of interest
	 * @return always true
	 */
	bool fetch_cover(const CoverLocation& cl);

	/**
	 * @brief uses CoverLocation(const QString& artist, const QString& album_name)
	 * @param artist Artist name
	 * @param album_name Album name
	 * @return true
	 */
	bool fetch_album_cover_standard(const QString& artist, const QString& album_name);

	/**
	 * @brief uses CoverLocation(const QString& album_name, const QStringList& artists)
	 * @param artists List of artist names
	 * @param album_name Album name
	 * @return true
	 */
	bool fetch_album_cover_sampler(const QStringList& artists, const QString& album_name);

	/**
	 * @brief uses CoverLocation(const Album& album)
	 * @param album Album object
	 * @return true
	 */
	bool fetch_album_cover(const Album& album);

	/**
	 * @brief uses CoverLocation(int album_id, int db_id);
	 * @param album_id Album ID
	 * @param db_id Database ID
	 * @return true
	 */
	bool fetch_album_cover_by_id(int album_id, quint8 db_id);

	/**
	 * @brief uses CoverLocation(const QString& artist);
	 * @param artist Artist name
	 * @return true
	 */
	bool fetch_artist_cover_standard(const QString& artist);

	/**
	 * @brief uses CoverLocation(const Artist& artist)
	 * @param artist Artist object
	 * @return true
	 */
	bool fetch_artist_cover(const Artist& artist);


	/**
	 * @brief Sets CoverLocation::cover_path and CoverLocation::search_url and starts new CoverFetchThread
	 * @param searchstring a Google search string
	 * @param target_name target cover path
	 * @return true
	 */
	bool fetch_cover_by_searchstring(const QString& searchstring, const QString& target_name);

	/**
	 * @brief Emits the Sayonara logo
	 */
	void emit_standard_cover();

	/**
	 * @brief not implemented
	 */
	void stop() override;


private:

	int _n_covers;

	/**
	 * @brief Starts a new CoverFetchThread
	 * @param cl CoverLocation object
	 */
	void start_new_thread(const CoverLocation& cl);
};

/**
 * @brief CoverLookupPtr
 * @ingroup Covers
 */
typedef std::shared_ptr<CoverLookup> CoverLookupPtr;

#endif /* COVERLOOKUP_H_ */
