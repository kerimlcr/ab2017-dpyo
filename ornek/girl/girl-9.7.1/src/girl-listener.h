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

#ifndef GIRL_LISTENER_H
#define GIRL_LISTENER_H

#include "girl.h"

typedef struct _GirlListenerInfo GirlListenerInfo;
typedef struct _GirlLocationInfo GirlLocationInfo;

struct _GirlListenerInfo {
	gchar *id;
	gchar *location;
	gchar *mail;
	gchar *name;
	gchar *pass;
	gchar *uri;
	gchar *band;
	gchar *description;
	GirlLocationInfo *locationinfo;
	GirlListenerInfo *next;
};

struct _GirlLocationInfo {
	gchar *name;
	gchar *link;
	gchar *glat;
	gchar *glon;
	gchar *grad;
	gchar *vote;
	gchar *rack;
};

GirlListenerInfo *girl_listener_load_from_file(GirlListenerInfo * head,
					       char *filename);
GirlListenerInfo *girl_listener_load_from_http(GirlListenerInfo * head,
					       gpointer data);

#endif				/* GIRL_LISTENER_H */
