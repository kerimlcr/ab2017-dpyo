/* SoundcloudHelper.h */

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



#ifndef SHOUTCASTHELPER_H
#define SHOUTCASTHELPER_H

#include <QString>

namespace SoundcloudWebAccess
{

	QString				create_dl_get_artist(const QString& name);
	QString				create_dl_get_artist(int artist_id);

	QString				create_dl_get_playlist(int playlist_id);
	QString				create_dl_get_playlists(int artist_id);

	QString				create_dl_get_track(int track_id);
	QString				create_dl_get_tracks(int artist_id);

}

#endif // SHOUTCASTHELPER_H
