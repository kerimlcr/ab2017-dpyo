/* $Id$
 *
 * GNOME Internet Radio Locator -- Canvas
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

#ifndef GIRL_CANVAS_H
#define GIRL_CANVAS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gnome.h>

extern GnomeCanvasGroup *girl_canvas_root;
extern GnomeCanvasItem *girl_canvas_item;
extern GtkWidget *girl_canvas;

GtkWidget *girl_canvas_create(char *image, guint x1, guint x2, guint y1, guint y2, gpointer data);

#endif
