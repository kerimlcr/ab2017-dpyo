/* SoundcloudWebAccess.cpp */

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

#include "SoundcloudWebAccess.h"
#include "SoundcloudGlobal.h"
#include "Helper/Logger/Logger.h"


QString	SoundcloudWebAccess::create_dl_get_artist(const QString& name){

	QString ret = "";

	if(name.isEmpty()) {
		return ret;
	}

	ret = QString("http://api.soundcloud.com/users?") +
			CLIENT_ID_STR +
			"&q=" +
			name;

	sp_log(Log::Debug) << "Get Artist info from " << ret;

	return ret;
}

QString	SoundcloudWebAccess::create_dl_get_artist(int artist_id){

	QString ret = "";

	if(artist_id <= 0){
		return ret;
	}

	ret = QString("http://api.soundcloud.com/users/") +
			QString::number(artist_id) + "?" +
			CLIENT_ID_STR;

	sp_log(Log::Debug) << "Get Artist info from " << ret;

	return ret;
}

QString	SoundcloudWebAccess::create_dl_get_playlist(int playlist_id){

	QString ret;

	ret = QString("http://api.soundcloud.com/playlists/") +
			QString::number(playlist_id) + "?" +
			CLIENT_ID_STR;

	sp_log(Log::Debug, "Soundcloud") << "Get playlist from " << ret;

	return ret;
}

QString	SoundcloudWebAccess::create_dl_get_playlists(int artist_id){

	QString ret;

	ret = QString("http://api.soundcloud.com/users/") +
			QString::number(artist_id) + "/" +
			"playlists?" +
			CLIENT_ID_STR;

	sp_log(Log::Debug, "Soundcloud") << "Get artist playlists from " << ret;

	return ret;
}

QString	SoundcloudWebAccess::create_dl_get_track(int track_id){

	QString ret;

	ret = QString("http://api.soundcloud.com/tracks/") +
			QString::number(track_id) + "?" +
			CLIENT_ID_STR;

	sp_log(Log::Debug, "Soundcloud") << "Get track from " << ret;

	return ret;
}

QString	SoundcloudWebAccess::create_dl_get_tracks(int artist_id){

	QString ret;

	ret = QString("http://api.soundcloud.com/users/") +
			QString::number(artist_id) + "/" +
			"tracks?" +
			CLIENT_ID_STR;

	sp_log(Log::Debug, "Soundcloud") << "Get Artist tracks from " << ret;

	return ret;
}

