/* SoundcloudData.h */

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



#ifndef SOUNDCLOUDDATA_H
#define SOUNDCLOUDDATA_H

#include "Helper/globals.h"
#include "SoundcloudJsonParser.h"
#include "SoundcloudWebAccess.h"
#include "Database/LibraryDatabase.h"


/* this is the database interface
 * TODO: make database connector my parent
 * TODO: create real (new) database
 */

class SoundcloudData :
		public LibraryDatabase
{

	Q_OBJECT
	SINGLETON(SoundcloudData)


public:
	bool db_fetch_tracks(SayonaraQuery& q, MetaDataList& result) override;
	bool db_fetch_albums(SayonaraQuery& q, AlbumList& result) override;
	bool db_fetch_artists(SayonaraQuery& q, ArtistList& result) override;

	int updateArtist(const Artist& artist) override;
	int insertArtistIntoDatabase (const Artist& artist) override;
	int insertArtistIntoDatabase (const QString& artist) override;

	int updateAlbum(const Album& album) override;
	int insertAlbumIntoDatabase (const Album& album) override;
	int insertAlbumIntoDatabase (const QString& album) override;

	bool updateTrack(const MetaData& md) override;
	bool storeMetadata(const MetaDataList& v_md) override;
	bool insertTrackIntoDatabase(const MetaData &md, int artistID, int albumID) override;

	bool apply_fixes() override;
};

#endif // SOUNDCLOUDDATA_H
