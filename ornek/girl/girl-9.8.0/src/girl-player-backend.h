/* $Id$
 *
 * GNOME Internet Radio Locator
 *
 * Copyright (C) 2016  Ole Aamot Software
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

/* Based on gstplayer from
 * https://github.com/sreerenjb/gstplayer/blob/master/gst-backend.h
 *
 * Copyright (C) 2010 Sreerenj Balachandran
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __GIRL_PLAYER_BACKEND__
#define __GIRL_PLAYER_BACKEND__

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gst/gst.h>

typedef struct GirlMedia {
  GstElement *pipeline;
  GstBus *bus;
  gchar *uri;
} GirlMedia;


gboolean girl_player_backend_init (int *argc, char **argv[]);

gboolean girl_player_backend_start (gchar *uri, gchar *name);

static gboolean handle_message (GstBus *bus, GstMessage *message , gpointer data);

GstBusSyncReply CreateWindow (GstBus *bus, GstMessage *message, gpointer data);

void girl_player_backend_play ();
void girl_player_backend_pause ();
void girl_player_backend_seek ();

void girl_player_backend_stop (GMainLoop *loop);

#endif
