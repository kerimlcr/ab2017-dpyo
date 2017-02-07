/* DatabaseConnector.h */

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



#ifndef DatabaseConnector_H
#define DatabaseConnector_H

#include "Database/LibraryDatabase.h"

#include "Database/DatabaseBookmarks.h"
#include "Database/DatabaseModule.h"
#include "Database/DatabasePlaylist.h"
#include "Database/DatabasePodcasts.h"
#include "Database/DatabaseSettings.h"
#include "Database/DatabaseStreams.h"
#include "Database/DatabaseVisStyles.h"


#define INDEX_SIZE 3


class DatabaseConnector :
		public LibraryDatabase,
		public DatabaseBookmarks,
		public DatabasePlaylist,
		public DatabasePodcasts,
		public DatabaseSettings,
		public DatabaseStreams,
		public DatabaseVisStyles
{

	SINGLETON(DatabaseConnector)


protected:
	bool updateAlbumCissearchFix();
	bool updateArtistCissearchFix();
	bool updateTrackCissearchFix();

	virtual bool apply_fixes();

public:
	virtual void clean_up();
};

#endif // DatabaseConnector_H
