/* CoverLookupAll.h */

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



#ifndef COVERLOOKUPALL_H
#define COVERLOOKUPALL_H

#include "CoverLookup.h"

#include "Helper/MetaData/Album.h"

class CoverLocation;
/**
 * @brief The CoverLookupAll class. The complete work is done by the CoverLookupThread
 * @ingroup Covers
 */
class CoverLookupAll : public CoverLookupInterface
{
    Q_OBJECT

private:
	CoverLookup*	_cl=nullptr;
	AlbumList		_album_list;
	bool			_run;


private slots:

	/**
	 * @brief Triggered, when CoverFetchThread found a cover
	 * @param cl CoverLocation object
	 */
	void cover_found(const QString& cl);

	/**
	 * @brief Triggered, when CoverFetchThread has finished
	 */
    void finished(bool);


public:
	/**
	 * @brief Constructor
	 * @param parent
	 * @param album_list List of all albums to search covers for
	 */
    CoverLookupAll(QObject* parent, const AlbumList& album_list);
    ~CoverLookupAll();

	/**
	 * @brief Stops CoverLookupThread
	 */
	void stop() override;

	/**
	 * @brief Runs the search
	 */
    void start();

};

#endif // COVERLOOKUPALL_H
