/* LibColumns.h */

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



#ifndef LIBCOLUMNS_H
#define LIBCOLUMNS_H

enum LibColumn {

	LibColumn_ArtistNAlbums=0,
	LibColumn_ArtistName,
	LibColumn_ArtistNTracks,

	LibColumn_AlbumSampler,
	LibColumn_AlbumName,
	LibColumn_AlbumDuration,
	LibColumn_AlbumNTracks,
	LibColumn_AlbumYear,
	LibColumn_AlbumTracks,

	LibColumn_TrackTitle,
	LibColumn_TrackArtist,
	LibColumn_TrackAlbum,
	LibColumn_TrackYear,
	LibColumn_TrackDuration,
	LibColumn_TrackBitrate,
	LibColumn_TrackFilesize,
	LibColumn_TrackRating
};

#endif // LIBCOLUMNS_H
