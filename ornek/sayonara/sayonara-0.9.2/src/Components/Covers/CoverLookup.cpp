/* CoverLookup.cpp */

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
 * CoverLookup.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: Lucio Carreras
 */


#include "CoverLookup.h"
#include "CoverHelper.h"
#include "CoverFetchThread.h"
#include "CoverLocation.h"

#include "Database/DatabaseConnector.h"

#include "Helper/MetaData/MetaData.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"


#include <QFile>
#include <QImage>

CoverLookupInterface::CoverLookupInterface(QObject* parent):
	QObject(parent)
{
}


CoverLookup::CoverLookup(QObject* parent, int n_covers) :
	CoverLookupInterface(parent)
{
	_n_covers = n_covers;
}

CoverLookup::~CoverLookup() {

}


void CoverLookup::start_new_thread(const CoverLocation& cl ) {

	CoverFetchThread* cft = new CoverFetchThread(this, cl, _n_covers);
	connect(cft, &CoverFetchThread::sig_cover_found, this, &CoverLookup::cover_found);
	connect(cft, &CoverFetchThread::sig_finished, this, &CoverLookup::finished);

	cft->start();
}


bool CoverLookup::fetch_cover(const CoverLocation& cl) {

	// Look, if cover exists in .Sayonara/covers
	if( QFile::exists(cl.cover_path()) && _n_covers == 1 ) {

		emit sig_cover_found(cl.cover_path());
		return true;
	}

	// For one cover, we also can use the local cover path
	if(!cl.local_paths().isEmpty() && _n_covers == 1){

		emit sig_cover_found(cl.local_path(0));
		return true;
	}

	// we have to fetch the cover from the internet
	start_new_thread( cl );

	return true;
}


bool CoverLookup::fetch_album_cover_standard(const QString& artist_name, const QString& album_name) {

	CoverLocation cl = CoverLocation::get_cover_location(album_name, artist_name);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_album_cover_sampler(const QStringList& artists, const QString& album_name) {

	CoverLocation cl = CoverLocation::get_cover_location(album_name, artists);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_album_cover_by_id(int album_id, quint8 db_id) {

	CoverLocation cl = CoverLocation::get_cover_location(album_id, db_id);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_album_cover(const Album& album) {

	CoverLocation cl = CoverLocation::get_cover_location(album);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_artist_cover_standard(const QString& artist) {

	CoverLocation cl = CoverLocation::get_cover_location(artist);
	return fetch_cover(cl);
}



bool CoverLookup::fetch_artist_cover(const Artist& artist) {

	CoverLocation cl = CoverLocation::get_cover_location(artist);
	return fetch_cover(cl);
}


bool CoverLookup::fetch_cover_by_searchstring(const QString& searchstring, const QString& target_name) {

	CoverLocation cl =
			CoverLocation::get_cover_location_by_searchstring(searchstring, target_name);

	start_new_thread( cl );
	return true;
}


void CoverLookup::finished(bool success) {

    emit sig_finished(success);
}

void CoverLookup::cover_found(const QString& file_path) {

	CoverFetchThread* cft = static_cast<CoverFetchThread*>(sender());
    emit sig_cover_found(file_path);

	if(!cft->more()){
		emit sig_finished(true);
	}
}

void CoverLookup::emit_standard_cover() {

	CoverLocation cl = CoverLocation::getInvalidLocation();
	emit sig_cover_found(cl.cover_path());
}

void CoverLookup::stop()
{

}


