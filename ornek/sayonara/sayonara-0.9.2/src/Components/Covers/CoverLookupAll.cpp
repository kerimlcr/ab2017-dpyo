/* CoverLookupAll.cpp */

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

#include "CoverLookupAll.h"
#include "CoverLocation.h"
#include "Helper/Helper.h"

CoverLookupAll::CoverLookupAll(QObject* parent, const AlbumList& album_list) :
    CoverLookupInterface(parent),
    _album_list(album_list),
    _run(true)
{
    _cl = new CoverLookup(this);
	connect(_cl, &CoverLookup::sig_cover_found, this, &CoverLookupAll::cover_found);
	connect(_cl, &CoverLookup::sig_finished, this, &CoverLookupAll::finished);
}


CoverLookupAll::~CoverLookupAll() {

    _cl->stop();
}


void CoverLookupAll::start() {

    Album album = _album_list.back();
    _cl->fetch_album_cover(album);
}


void CoverLookupAll::stop() {
    _run = false;
    _cl->stop();
}


void CoverLookupAll::cover_found(const QString& cover_path) {

    _album_list.pop_back();
	emit sig_cover_found(cover_path);

	if(!_run) {
		return;
	}

    // Google and other webservices block, if looking too fast
	Helper::sleep_ms(1000);

    Album album = _album_list.back();
    _cl->fetch_album_cover(album);
}

void CoverLookupAll::finished(bool success) {
    emit sig_finished(success);
}
