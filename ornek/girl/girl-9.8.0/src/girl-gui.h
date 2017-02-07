/* $Id: girl-gui.h,v 1.2 2002/09/22 20:19:50 oka Exp $ */

#ifndef GIRL_GUI_H
#define GIRL_GUI_H

#include <libgnome/libgnome.h>

GtkWidget *create_girl_app(void);

GtkWidget *create_listeners_selector(char *selected_listener_uri,
				     char *filename);
GtkWidget *create_stations_selector(char *selected_station_uri,
				    char *filename);
GtkWidget *create_streams_selector(char *selected_streams_uri,
				   char *filename);
GtkWidget *create_search_selector(void);

GtkWidget *create_new_station_selector(void);

#endif /* GIRL_GUI_H */
