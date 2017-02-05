/* $Id: girl.c,v 1.4 2002/10/04 20:20:30 oka Exp $
 *
 * GNOME Internet Radio Locator
 *
 * Copyright (C) 2015  Ole Aamot Software
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
 *
 */

/* 
 * Essential parts of the source code below was based on
 * gnome-control-center/panels/datetime/cc-timezone-map.h
 *
 * Copyright (C) 2010 Intel, Inc
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Thomas Wood <thomas.wood@intel.com>
 *
 */


#ifndef _GIRL_STATIONS_MAP_H
#define _GIRL_STATIONS_MAP_H

#include <gtk/gtk.h>
#include <gtk/gtkenums.h>

#include "girl-tz.h"

G_BEGIN_DECLS

#define GIRL_TYPE_STATIONS_MAP girl_stations_map_get_type()

#define GIRL_STATIONS_MAP(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  GIRL_TYPE_STATIONS_MAP, GirlStationsMap))

#define GIRL_STATIONS_MAP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  GIRL_TYPE_STATIONS_MAP, GirlStationsMapClass))

#define GIRL_IS_STATIONS_MAP(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  GIRL_TYPE_STATIONS_MAP))

#define GIRL_IS_STATIONS_MAP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  GIRL_TYPE_STATIONS_MAP))

#define GIRL_STATIONS_MAP_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  GIRL_TYPE_STATIONS_MAP, GirlStationsMapClass))

typedef struct _GirlStationsMap GirlStationsMap;
typedef struct _GirlStationsMapClass GirlStationsMapClass;
typedef struct _GirlStationsMapPrivate GirlStationsMapPrivate;

struct _GirlStationsMap
{
  GtkWidget parent;

  GirlStationsMapPrivate *priv;
};

struct _GirlStationsMapClass
{
  GtkWidgetClass parent_class;
};

GType girl_stations_map_get_type (void) G_GNUC_CONST;

GirlStationsMap *girl_stations_map_new (void);

gboolean girl_stations_map_set_location (GirlStationsMap *map,
					 const gchar   *timezone);
void girl_stations_map_set_bubble_text (GirlStationsMap *map,
                                      const gchar   *text);

TzLocation *girl_stations_map_get_location (GirlStationsMap *map);

G_END_DECLS

#endif /* _GIRL_STATIONS_MAP_H */
