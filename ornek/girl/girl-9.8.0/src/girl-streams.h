/* $Id$
 *
 * GNOME Internet Radio Locator
 *
 * Copyright (C) 2014  Ole Aamot Software
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

#ifndef GIRL_STREAMS_H
#define GIRL_STREAMS_H

#include "girl.h"

typedef struct _GirlStreamsInfo GirlStreamsInfo;
typedef struct _GirlEncoderInfo GirlEncoderInfo;

struct _GirlStreamsInfo {
	GtkWidget *widget;
	gchar *mime;
	gchar *uri;
	gchar *codec;
	gchar *samplerate;
	gchar *streams;
	gchar *bitrate;
	GirlChannels channels;
	GirlEncoderInfo *encoder;
	GirlStreamsInfo *next;
	GirlStreamsInfo *prev;
};

struct _GirlEncoderInfo {
	gchar *id;
	gchar *name;
	GirlEncoderInfo *next;
	gchar *mimetype;
	glong bitrate;
	glong samplerate;
	GirlChannels channels;
	gchar *uri;
};

GirlStreamsInfo *girl_streams_new (GirlStreamsInfo * head,
				   gchar *mime,
				   gchar *uri,
				   gchar *codec,
				   gchar *samplerate,
				   gchar *channels,
	                           gchar *bitrate);

void girl_streams_free(GirlStreamsInfo * info);
GirlStreamsInfo *girl_streams_load_from_file(GirlStreamsInfo * head,
					     char *filename);
GirlStreamsInfo *girl_streams_load_from_http(GirlStreamsInfo * head,
					     gpointer data);

#endif /* GIRL_STREAMS_H */
