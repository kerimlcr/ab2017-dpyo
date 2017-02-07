
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




#ifndef PLAYLISTMENUENTRY_H
#define PLAYLISTMENUENTRY_H

#include <QObject>
#include <QtGlobal>

typedef quint16 PlaylistMenuEntries;

enum PlaylistMenuEntry {
	None=0,
	Delete=			(1 << 0),
	Save=			(1 << 1),
	SaveAs=			(1 << 2),
	Close=			(1 << 3),
	CloseOthers =	(1 << 4),
	Reset=			(1 << 5),
	Rename=			(1 << 6),
	Clear=			(1 << 7),
	OpenFile=		(1 << 8),
	OpenDir=		(1 << 9)
};


#endif // PLAYLISTMENUENTRY_H
