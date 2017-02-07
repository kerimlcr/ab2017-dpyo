/* $Id$
 *
 * GNOME Internet Radio Locator
 *
 * Copyright (C) 2014, 2015  Ole Aamot Software
 *
 * Author: Ole Aamot <oka@oka.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef GIRL_STATION_H
#define GIRL_STATION_H

#include "girl.h"

typedef struct _GirlStationInfo GirlStationInfo;
typedef struct _GirlStreamInfo GirlStreamInfo;

typedef enum {
	STATION_LANG = 11,
	STATION_ID = 10,
	STATION_NAME = 0,
	STATION_RANK = 7,
	STATION_TYPE = 6,
	STATION_BAND = 5,
	STATION_FREQUENCY = 4,
	STATION_LOCATION = 1,
	STATION_DESCRIPTION = 3,
	STATION_BITRATE = 8,
	STATION_SAMPLERATE = 9,
	STATION_URI = 2
} GirlStationColumn;

struct _GirlStationInfo {
	GtkWidget *widget;
	gchar *id;
	gchar *name;
	gchar *rank;
	gchar *type;
	gchar *band;
	gchar *frequency;
	gchar *location;
	gchar *description;
	gchar *bitrate;
	gchar *samplerate;
	gchar *uri;
	gchar *lang;
	/*   gchar *category; */
	GirlStreamInfo *stream;
	GirlStationInfo *next;
	GirlStationInfo *prev;
};

struct _GirlStreamInfo {
	gchar *id;
	gchar *name;
	GirlStreamInfo *next;
	gchar *mimetype;
	glong bitrate;
	glong samplerate;
	GirlChannels channels;
	gchar *uri;
};

void show_error(gchar * msg);
void girl_station_free(GirlStationInfo * info);
GirlStationInfo *girl_station_load_from_file(GirlStationInfo * head,
					     char *filename);
GirlStationInfo *girl_station_load_from_http(GirlStationInfo * head,
					     gpointer data);
void girl_helper_run(char *url, char *name, GirlStreamType type, GirlHelperType girl);

gint girl_station_update (GirlStationInfo *head, gchar *station_band, gchar *station_description, gchar *station_name, gchar *station_location, gchar *station_uri, gchar *station_website);

#endif /* GIRL_STATION_H */
