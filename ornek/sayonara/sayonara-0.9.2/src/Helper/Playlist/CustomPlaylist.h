/* CustomPlaylist.h */

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



#ifndef CUSTOMPLAYLIST_H
#define CUSTOMPLAYLIST_H

#include "Helper/MetaData/MetaDataList.h"

#include <QList>

/**
 * @brief The CustomPlaylistSkeleton class
 * @ingroup PlaylistHelper
 */
class CustomPlaylistSkeleton {

public:
	int				id;
	QString			name;
	bool			is_temporary;
	int				n_tracks;

	CustomPlaylistSkeleton();
	CustomPlaylistSkeleton(const CustomPlaylistSkeleton& s);
};

/**
 * @brief The CustomPlaylist class
 * @ingroup PlaylistHelper
 */
class CustomPlaylist : public CustomPlaylistSkeleton{

public:
	MetaDataList	tracks;
	bool			is_valid;

	CustomPlaylist();
	CustomPlaylist(const CustomPlaylistSkeleton& skeleton);
};

typedef QList<CustomPlaylist> CustomPlaylists;
typedef QList<CustomPlaylistSkeleton> CustomPlaylistSkeletons;


#endif // CUSTOMPLAYLIST_H
