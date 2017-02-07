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

#ifndef GIRL_PROGRAM_H
#define GIRL_PROGRAM_H

#include <gtk/gtk.h>
#include "girl.h"

typedef struct _GirlProgramInfo GirlProgramInfo;
typedef struct _GirlArchiveInfo GirlArchiveInfo;

struct _GirlProgramInfo {
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
	/*   gchar *category; */
	GirlArchiveInfo *archive;
	GirlProgramInfo *next;
	GirlProgramInfo *prev;
};

struct _GirlArchiveInfo {
	gchar *id;
	gchar *name;
	GirlArchiveInfo *next;
	gchar *mimetype;
	glong bitrate;
	glong samplerate;
	GirlChannels channels;
	gchar *uri;
};

GirlProgramInfo *girl_program_new (GirlProgramInfo * head,
				   gchar *id,
				   gchar *name,
				   gchar *date,
				   gchar *time,
				   gchar *file);

void girl_program_free(GirlProgramInfo * info);
GirlProgramInfo *girl_program_load_from_file(GirlProgramInfo * head,
					     char *filename);
GirlProgramInfo *girl_program_load_from_http(GirlProgramInfo * head,
					     gpointer data);

#endif /* GIRL_PROGRAM_H */
