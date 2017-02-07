/* LFMGlobals.h

 * Copyright (C) 2012-2016 Lucio Carreras  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Jul 18, 2012 
 *
 */

#ifndef LFMGLOBALS_H_
#define LFMGLOBALS_H_

#define LFM_RADIO_MODE_ARTIST 0
#define LFM_RADIO_MODE_TAG 1
#define LFM_RADIO_MODE_RECOMMENDED 2
#define LFM_RADIO_MODE_USER_LIBRARY 3


#define LFM_TAG_TRACK_TITLE "track.name"
#define LFM_TAG_TRACK_ALBUM "track.album.title"
#define LFM_TAG_TRACK_ARTIST "track.artist.name"
#define LFM_TAG_TRACK_DURATION "track.duration"
#define LFM_TAG_TRACK_LOVED "track.userloved"
#define LFM_TAG_TRACK_USERPLAYCOUNT "track.userplaycount"


#define LFM_API_KEY 	QByteArray("51d6f9eaef806f603f346844bef326ba")
#define LFM_API_SECRET	QByteArray("1093d769e54858cb0d21d42b35a8f603")


#include <QMap>
#include <QByteArray>


#endif /* LFMGLOBALS_H_ */
