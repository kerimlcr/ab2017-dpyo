/* $Id: girl.c,v 1.4 2002/10/04 20:20:30 oka Exp $
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

#include <config.h>
#include <glib/gstdio.h>
#include <glib/glist.h>
#include <gnome.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-application-registry.h>
#include <gio/gcredentials.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <libintl.h>
#include "girl.h"
#include "girl-gui.h"
#include "girl-listener.h"
#include "girl-station.h"
#include "girl-program.h"
#include "girl-player-frontend.h"
#include "girl-player-backend.h"
#include "girl-record-frontend.h"
#include "girl-record-backend.h"

GirlData *girl;

GList *girl_archivers;
GList *girl_listeners;
GList *girl_programs;
GList *girl_stations;
GList *girl_streams;

GtkWidget *girl_app;
GtkWidget *archivers_selector = NULL;
GtkWidget *listeners_selector = NULL;
GtkWidget *programs_selector = NULL;
GtkWidget *stations_selector = NULL;
GtkWidget *streams_selector = NULL;
GtkWidget *search_selector = NULL;

gchar *list_item_data_key ="list_item_data";

int main(int argc, char *argv[])
{
	gchar *iconname;
	gchar msg[256];
	gchar *msg1, *msg2;
#ifdef GETTEXT_PACKAGE
	bindtextdomain(GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
#endif

	if (gnome_vfs_init() == FALSE) {
	        GIRL_DEBUG_MSG(_("GNOME VFS initialization failed!\n"));
		exit(1);
	}

	if (!girl_player_frontend_init (&argc, &argv)) {
		GIRL_DEBUG_MSG (_("Player Frontend initialization failure..."));
		exit (0);
	}

	if (!girl_player_backend_init (&argc, &argv)) {
		GIRL_DEBUG_MSG (_("Player Backend creation failure"));
		exit (0);
	}

	gst_init(&argc, &argv);

	gnome_program_init(PACKAGE, VERSION,
			   LIBGNOMEUI_MODULE,
			   argc, argv,
			   GNOME_PARAM_APP_DATADIR, DATADIR, NULL);

#if defined(G_THREADS_ENABLED) && ! defined(G_THREADS_IMPL_NONE)
	/* g_thread_init (NULL); */
#endif

	girl_app = create_girl_app();

	gtk_widget_show(girl_app);

	/* Set up the channels list */
	stations_selector = create_stations_selector(girl->selected_station_uri,
						     "girl.xml");

	/* /\* Set up the stations list *\/ */
	/* stations_selector = create_stations_selector(girl->selected_station_uri, */
	/* 					     "girl.xml"); */

	/* Set up the streams list */
	/* streams_selector = create_streams_selector(girl->selected_streams_uri, */
	/* 					   "streams.xml"); */

	g_object_add_weak_pointer(G_OBJECT(stations_selector),
				  (void **) &(stations_selector));

	/* g_object_add_weak_pointer(G_OBJECT(stations_selector), */
	/* 			  (void **) &(stations_selector)); */

	/* g_object_add_weak_pointer(G_OBJECT(streams_selector), */
	/* 			  (void **) &(streams_selector)); */
	/* Icons */

	iconname = g_strconcat(GIRL_DATADIR, "/girl.png", NULL);

	if (iconname != NULL) {
		girl->icon = gdk_pixbuf_new_from_file(iconname, NULL);
		g_free(iconname);
	} else {
		girl->icon = NULL;
	}

	if (girl->icon != NULL)
		gtk_window_set_icon(GTK_WINDOW(girl_app), girl->icon);

#ifdef GETTEXT_PACKAGE
	msg1 = ngettext("%i station", "%i stations", girl->station_count);
	msg2 = ngettext("and %i stream found", "and %i streams found",
			girl->stream_count);
	snprintf(msg, sizeof(msg), _("%s %s"), msg1, msg2);
	appbar_send_msg(msg, girl->station_count, girl->stream_count);
#endif /* GETTEXT_PACKAGE */
	gtk_main();
	return 0;
}

void
toggle_in_area (double x1, double y1, double x2, double y2)
{
	/* double nx1,nx2,ny1,ny2; */
	/* nx1 = x1; */
	/* nx2 = x2; */
	/* ny1 = y1; */
	/* ny2 = y2; */
	
}

void appbar_send_msg(const char *a, ...)
{
	gchar foo[2048];
	va_list ap;
	va_start(ap, a);
	vsnprintf(foo, 2000, a, ap);
	gnome_appbar_pop(girl->appbar);
	gnome_appbar_push(girl->appbar, foo);
	GIRL_DEBUG_MSG("%s", foo);
	/* lets update the statusbar widget */
	while (gtk_events_pending())
		gtk_main_iteration();
	va_end(ap);
}

static void cause_movement(int way)
{

}

void on_previous_station_click(GtkWidget * a, gpointer user_data)
{
	GirlStationInfo *station = (GirlStationInfo *)girl->previous_station;
	girl->previous_station = girl->selected_station;
	if (station != NULL) {
		cause_movement(-1);
		GIRL_DEBUG_MSG("Previous Station ID: %s\n", station->id);
		GIRL_DEBUG_MSG("Previous Station Name: %s\n", station->name);
		GIRL_DEBUG_MSG("Previous Station URI: %s\n", station->stream->uri);
		GIRL_DEBUG_MSG("Previous Station Location: %s\n", station->location);
		girl->selected_station_uri = station->stream->uri;
		girl->selected_station_name = station->name;
		girl->selected_station_location = station->location;
		girl->selected_station_band = station->band;
		girl->selected_station_description = station->description;
		GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
			       girl->selected_station_uri);

		girl->selected_station_name = station->name;
		GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
			       girl->selected_station_name);
		girl->selected_station_location = station->location;
		GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
			       girl->selected_station_location);
		girl->selected_station_band = station->band;
		GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
			       girl->selected_station_band);
		girl->selected_station_description = station->description;
		GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
			       girl->selected_station_description);
		appbar_send_msg(_("Selected the previous radio station: %s in %s: %s"),
				girl->selected_station_name,
				girl->selected_station_location,
				girl->selected_station_uri,
				girl->selected_station_band);
		girl_helper_run(girl->selected_station_uri,
				girl->selected_station_name,
				GIRL_STREAM_SHOUTCAST,
				GIRL_STREAM_PLAYER);

		about_station((GirlData*)girl, (GirlStationInfo*)girl->previous_station);
		
	} else {

		GIRL_DEBUG_MSG("Prev Station: At the beginning of Stations list!\n");
	}
	girl->previous_station = station;
}

void on_next_station_click(GtkWidget * a, gpointer user_data)
{
	GList *l = g_list_first(girl_stations);
	girl_player_frontend_stop (girl->player_window, NULL);
	girl->previous_station = (GirlStationInfo*)girl->selected_station;
	if (l != NULL) {
		GirlStationInfo *station = l->data;
		if (station->next != NULL) {
			girl_stations->data = station->next;
		} else {
			GIRL_DEBUG_MSG("Next Station: At the end of Stations list!\n");
		}
		cause_movement(1);
		if (station != NULL) {
			GIRL_DEBUG_MSG("Next Station ID: %s\n", station->id);
			GIRL_DEBUG_MSG("Next Station Name: %s\n", station->name);
			GIRL_DEBUG_MSG("Next Station URI: %s\n", station->stream->uri);
			GIRL_DEBUG_MSG("Next Station Location: %s\n", station->location);

			girl->selected_station_uri = station->stream->uri;

			GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
			    girl->selected_station_uri);

			girl->selected_station_name = station->name;

			GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
			    girl->selected_station_name);

			girl->selected_station_location = station->location;

			GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
			    girl->selected_station_location);

			girl->selected_station_band = station->band;

			GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
			    girl->selected_station_band);

			girl->selected_station_description = station->description;

			GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
			    girl->selected_station_description);

			appbar_send_msg(_("Selected the next radio station %s in %s: %s"),
					girl->selected_station_name,
					girl->selected_station_location,
					girl->selected_station_uri,
					girl->selected_station_band);
			
			girl_helper_run(station->stream->uri,
					station->name,
					GIRL_STREAM_SHOUTCAST,
					GIRL_STREAM_PLAYER);

			girl->selected_station = (GirlStationInfo*)station;

			about_station((GirlData*)girl, (GirlStationInfo*)girl->selected_station);
		}
	}
}

void on_new_station_clicked(GtkWidget *a,
			    gpointer user_data)
{
	GtkWidget *station;
	GirlStationInfo *stationinfo;
	/* GList *l = g_list_first(girl_stations); */

	/* stationinfo = l->data; */
	gint result;
	// appbar_send_msg(_("New radio station"));
	station = create_new_station_selector();
	result = gtk_dialog_run (GTK_DIALOG(station));
	switch (result)  {
	case GTK_RESPONSE_ACCEPT:

		GIRL_DEBUG_MSG("Squeak!\n\n");
		girl->selected_station_uri = g_strdup(g_object_get_data(G_OBJECT(station), "station_uri"));
		GIRL_DEBUG_MSG("on_new_station_select_changed: %s\n", girl->selected_station_uri);
		girl->selected_station_description = g_strdup(g_object_get_data(G_OBJECT(station), "station_description"));
		GIRL_DEBUG_MSG("on_new_station_select_changed: %s\n", girl->selected_station_description);
		girl->selected_station_band = g_strdup(g_object_get_data(G_OBJECT(station), "station_band"));
		GIRL_DEBUG_MSG("on_new_station_select_changed: %s\n", girl->selected_station_band);
		girl->selected_station_website = g_strdup(g_object_get_data(G_OBJECT(station), "station_website"));
		GIRL_DEBUG_MSG("on_new_station_select_changed: %s\n", girl->selected_station_website);
		girl->selected_station_name = g_strdup(g_object_get_data(G_OBJECT(station), "station_name"));
		GIRL_DEBUG_MSG("on_new_station_select_changed: %s\n",
			       girl->selected_station_name);
		girl->selected_station_location = g_strdup(g_object_get_data(G_OBJECT(station), "station_location"));
		GIRL_DEBUG_MSG("on_new_station_select_changed: %s\n",
			       girl->selected_station_location);
		girl_station_update (stationinfo, girl->selected_station_band, girl->selected_station_description, girl->selected_station_name, girl->selected_station_location, girl->selected_station_uri, girl->selected_station_website);
		break;
	default:
		g_print ("Nothing\n\n");
		break;
	}
	gtk_widget_destroy(station);
	/* gtk_widget_show(station); */

}

void on_new_station_selector_changed(GtkWidget *a,
				     gpointer user_data)
{
	/* GtkWidget *station; */

}

void on_listeners_selector_button_clicked(GtkWidget * a,
					  gpointer user_data)
{
	gtk_widget_show(listeners_selector);
	gdk_window_raise(listeners_selector->window);
}

void on_listeners_selector_changed(GtkWidget * a, gpointer user_data)
{
	if (girl->selected_listener_uri != NULL)
		g_free(girl->selected_listener_uri);

	girl->selected_listener_uri =
	    g_strdup(g_object_get_data(G_OBJECT(a), "listener_uri"));
	GIRL_DEBUG_MSG("on_listener_select_changed: %s\n",
	    girl->selected_listener_uri);

	girl->selected_listener_name =
	    g_strdup(g_object_get_data(G_OBJECT(a), "listener_name"));
	GIRL_DEBUG_MSG("on_listener_select_changed: %s\n",
	    girl->selected_listener_name);

	girl->selected_listener_location =
	    g_strdup(g_object_get_data(G_OBJECT(a), "listener_location"));
	GIRL_DEBUG_MSG("on_listener_select_changed: %s\n",
	    girl->selected_listener_location);

	girl->selected_listener_band =
	    g_strdup(g_object_get_data(G_OBJECT(a), "listener_band"));
	GIRL_DEBUG_MSG("on_listener_select_changed: %s\n",
	    girl->selected_listener_band);

	girl->selected_listener_description =
	    g_strdup(g_object_get_data(G_OBJECT(a), "listener_description"));
	GIRL_DEBUG_MSG("on_listener_select_changed: %s\n",
	    girl->selected_listener_description);

	appbar_send_msg(_("Selected %s in %s: %s (%f)"),
			girl->selected_listener_name,
			girl->selected_listener_location,
			girl->selected_listener_uri,
		        girl->selected_listener_band);

	girl_helper_run(girl->selected_listener_uri,
			girl->selected_listener_name,
			GIRL_STREAM_SHOUTCAST,
			GIRL_STREAM_PLAYER);

}

void on_programs_selector_button_clicked(GtkWidget * a, gpointer user_data)
{
	gtk_widget_show(programs_selector);
	gdk_window_raise(programs_selector->window);
}

void on_programs_selector_changed(GtkWidget * a, gpointer user_data)
{
	if (girl->selected_program_uri != NULL)
		g_free(girl->selected_program_uri);

	girl->selected_program_uri =
	    g_strdup(g_object_get_data(G_OBJECT(a), "program_uri"));
	GIRL_DEBUG_MSG("on_program_select_changed: %s\n", girl->selected_program_uri);

	girl->selected_program_name =
	    g_strdup(g_object_get_data(G_OBJECT(a), "program_name"));
	GIRL_DEBUG_MSG("on_program_select_changed: %s\n",
	    girl->selected_program_name);

	girl->selected_program_location =
	    g_strdup(g_object_get_data(G_OBJECT(a), "program_location"));
	GIRL_DEBUG_MSG("on_program_select_changed: %s\n",
	    girl->selected_program_location);

	girl->selected_program_band =
	    g_strdup(g_object_get_data(G_OBJECT(a), "program_band"));
	GIRL_DEBUG_MSG("on_program_select_changed: %s\n",
	    girl->selected_program_band);

	girl->selected_program_description =
	    g_strdup(g_object_get_data(G_OBJECT(a), "program_description"));
	GIRL_DEBUG_MSG("on_program_select_changed: %s\n",
	    girl->selected_program_description);

	appbar_send_msg(_("Selected %s in %s: %s"),
			girl->selected_program_name,
			girl->selected_program_location,
			girl->selected_program_uri,
			girl->selected_program_band);

	girl_helper_run(girl->selected_program_uri,
			girl->selected_program_name,
			GIRL_STREAM_SHOUTCAST,
			GIRL_STREAM_PLAYER);
}

void on_stations_selector_button_clicked(GtkWidget * a, gpointer user_data)
{
	gtk_widget_show(stations_selector);
	gdk_window_raise(stations_selector->window);
}

void on_stations_selector_changed(GtkWidget * a, gpointer user_data)
{
	GirlStationInfo *station;

	if (girl->selected_station_uri != NULL)
		g_free(girl->selected_station_uri);

	girl->selected_station_uri = g_strdup(g_object_get_data(G_OBJECT(a), "station_uri"));

	GIRL_DEBUG_MSG("on_station_select_changed: %s\n", girl->selected_station_uri);

	girl->selected_station_name =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_name"));
	GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
	    girl->selected_station_name);

	girl->selected_station_location =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_location"));
	GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
	    girl->selected_station_location);

	girl->selected_station_band =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_band"));
	GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
	    girl->selected_station_band);

	girl->selected_station_description =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_description"));
	GIRL_DEBUG_MSG("on_station_select_changed: %s\n",
	    girl->selected_station_description);

	appbar_send_msg(_("Selected the radio station %s in %s: %s"),
			girl->selected_station_name,
			girl->selected_station_location,
			girl->selected_station_uri,
			girl->selected_station_band);

	station->name = g_strdup(g_object_get_data(G_OBJECT(a), "station_name"));
	/* girl_history = g_list_add(GLIST(girl_history), (GirlStationInfo *)station); */
	girl_player_frontend_stop(girl->player_window, NULL);
	girl_helper_run(girl->selected_station_uri,
			girl->selected_station_name,
			GIRL_STREAM_SHOUTCAST,
		        GIRL_STREAM_PLAYER);
}

void on_new_station_changed(GtkWidget * a, gpointer user_data)
{
	GirlStationInfo *stationinfo = NULL;
	/* GList *l = g_list_first(girl_stations); */
	/* stationinfo = l->data; */

	/* if (girl->selected_station_uri != NULL) */
	/* 	g_free(girl->selected_station_uri); */

	girl->selected_station_uri =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_uri"));
	GIRL_DEBUG_MSG("on_new_station_changed: %s\n", girl->selected_station_uri);

	girl->selected_station_name =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_name"));
	GIRL_DEBUG_MSG("on_new_station_changed: %s\n",
	    girl->selected_station_name);

	girl->selected_station_location =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_location"));
	GIRL_DEBUG_MSG("on_new_station_changed: %s\n",
	    girl->selected_station_location);

	girl->selected_station_band =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_band"));
	GIRL_DEBUG_MSG("on_new_station_changed: %s\n",
	    girl->selected_station_band);

	girl->selected_station_description =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_description"));
	GIRL_DEBUG_MSG("on_new_station_changed: %s\n",
	    girl->selected_station_description);

	appbar_send_msg(_("Selected the radio station %s in %s: %s"),
			girl->selected_station_name,
			girl->selected_station_location,
			girl->selected_station_uri,
			girl->selected_station_band);

	girl_station_update(stationinfo, girl->selected_station_band, girl->selected_station_description, girl->selected_station_name,
			  girl->selected_station_location,
			  girl->selected_station_uri, girl->selected_station_website);

	girl_helper_run(girl->selected_streams_uri,
			girl->selected_streams_mime,
			GIRL_STREAM_SHOUTCAST,
		        GIRL_STREAM_PLAYER);
}

void on_streams_selector_button_clicked(GtkWidget * a, gpointer user_data)
{
	gtk_widget_show(streams_selector);
	gdk_window_raise(streams_selector->window);
}

void on_streams_selector_changed(GtkWidget * a, gpointer user_data)
{
	if (girl->selected_streams_uri != NULL)
		g_free(girl->selected_streams_uri);

	girl->selected_streams_uri = g_strdup(g_object_get_data(G_OBJECT(a), "streams_uri"));
	GIRL_DEBUG_MSG("on_streams_select_changed: %s\n", girl->selected_streams_uri);

	girl->selected_streams_mime = g_strdup(g_object_get_data(G_OBJECT(a), "streams_mime"));
	GIRL_DEBUG_MSG("on_streams_select_changed: %s\n",
	    girl->selected_streams_mime);

	girl->selected_streams_codec = g_strdup(g_object_get_data(G_OBJECT(a), "streams_codec"));
	GIRL_DEBUG_MSG("on_streams_select_changed: %s\n",
	    girl->selected_streams_codec);

	girl->selected_streams_samplerate = g_strdup(g_object_get_data(G_OBJECT(a), "streams_samplerate"));
	GIRL_DEBUG_MSG("on_streams_select_changed: %s\n",
	    girl->selected_streams_samplerate);

	girl->selected_streams_bitrate = g_strdup(g_object_get_data(G_OBJECT(a), "streams_bitrate"));
	GIRL_DEBUG_MSG("on_streams_select_changed: %s\n",
	    girl->selected_streams_bitrate);

	girl->selected_streams_stations = g_strdup(g_object_get_data(G_OBJECT(a), "streams_stations"));
	GIRL_DEBUG_MSG("on_streams_select_changed: %s\n",
	    girl->selected_streams_stations);

	appbar_send_msg(_("Selected %s [%s] [%s] [%s]"),
			girl->selected_streams_uri,
			girl->selected_streams_mime,
			girl->selected_streams_codec,
			girl->selected_streams_bitrate);

	girl_helper_run(girl->selected_streams_uri,
			girl->selected_streams_mime,
			GIRL_STREAM_SHOUTCAST,
		        GIRL_STREAM_PLAYER);
}

void quit_app(GtkWidget * a, gpointer user_data)
{
	/* GirlStationInfo *stationinfo; */
	/* GList *l = g_list_first(girl_stations); */

	gnome_config_push_prefix("/girl/General/");
	gnome_config_set_string("selected_listener_uri",
				girl->selected_listener_uri);
	gnome_config_set_string("selected_listener_name",
				girl->selected_listener_name);
	gnome_config_set_string("selected_listener_location",
				girl->selected_listener_location);
	gnome_config_set_string("selected_listener_band",
				girl->selected_listener_band);
	gnome_config_set_string("selected_station_uri",
				girl->selected_station_uri);
	gnome_config_set_string("selected_station_name",
				girl->selected_station_name);
	gnome_config_set_string("selected_station_location",
				girl->selected_station_location);
	gnome_config_set_string("selected_station_band",
				girl->selected_station_band);
	gnome_config_set_string("selected_station_description",
				girl->selected_station_description);
	gnome_config_set_string("selected_streams_uri",
				girl->selected_streams_uri);
	gnome_config_set_string("selected_streams_mime",
				girl->selected_streams_mime);
	gnome_config_set_string("selected_streams_codec",
				girl->selected_streams_codec);
	gnome_config_set_string("selected_streams_samplerate",
				girl->selected_streams_samplerate);
	gnome_config_set_string("selected_streams_bitrate",
				girl->selected_streams_bitrate);
	gnome_config_set_string("selected_streams_stations",
				girl->selected_streams_stations);
	gnome_config_sync();
	gnome_config_pop_prefix();

	gtk_widget_destroy(girl_app);
	/* if (GTK_IS_WIDGET(stations_selector)) { */
	/* 	gtk_widget_destroy(stations_selector); */
	/* } */
	/* if (GTK_IS_WIDGET(streams_selector)) { */
	/* 	gtk_widget_destroy(streams_selector); */
	/* } */
	// stationinfo = l->data;
	// girl_station_save(stationinfo, NULL, NULL, NULL, NULL, NULL, NULL);
	g_spawn_close_pid(girl->record_pid);
	g_spawn_close_pid(girl->player_pid);
	/* g_subprocess_force_exit(girl->player_subprocess); */
	/* g_subprocess_force_exit(girl->record_subprocess); */
	// kill(girl->record_pid,SIGHUP);
	// kill(girl->player_pid,SIGHUP);
	if (GTK_IS_WIDGET(girl->player_window)) {
		gtk_widget_destroy(girl->player_window);
	}
	/* gtk_widget_destroy(girl->record_window); */
	if (g_main_loop_is_running (girl->player_loop)) {
		girl_player_backend_stop(girl->player_loop);
	}
	if (GTK_IS_WIDGET(girl->record_window)) {
		gtk_widget_destroy(girl->record_window);
	}
	if (g_main_loop_is_running (girl->record_loop)) {
		girl_record_backend_stop(girl->record_loop);
	}
	/* girl_record_backend_stop(girl->record_loop); */
	gtk_main_quit();
}

void about_rec(GtkWidget * a, gpointer user_data)
{
	static GtkWindow *about = NULL;
	about = GTK_WINDOW(gtk_message_dialog_new(GTK_WINDOW(girl_app),
				       GTK_DIALOG_DESTROY_WITH_PARENT,
				       GTK_MESSAGE_QUESTION,
				       GTK_BUTTONS_YES_NO,
						  "Recording is not yet implemented.  If you want to implement it, see src/girl-record.c and send an email to ole@src.gnome.org with the patch."));

	gtk_dialog_run(GTK_DIALOG(about));
}

void about_app(GtkWidget * a, gpointer user_data)
{
	static GtkWindow *about = NULL;

	gchar* authors[] = { "Ole Aamot <ole@src.gnome.org> (Maintainer)", NULL };

	gchar* translators = g_strdup(_("Marek Černocký <marek@manet.cz> (Czech translation)\n"
					"Rafael Fontenelle <rafaelff@gnome.org> (Brazilian Portuguese translation)\n"
					"Miroslav Nikolić <miroslavnikolic@rocketmail.com> (Serbian translation)\n"
					"Wolfgang Stöggl <c72578@yahoo.de> (German translation)\n"
					"Piotr Drąg <piotrdrag@gmail.com> (Polish translation)\n"
					"Muhammet Kara <muhammetk@gmail.com> (Turkish translation)\n"
					"Balázs Úr <urbalazs@gmail.com> (Hungarian translation)\n"
					"Andika Triwidada <andika@gmail.com> (Indonesian translation)\n"
					"Daniel Mustieles <daniel.mustieles@gmail.com> (Spanish translation)\n"
					"Matej Urbančič <mateju@svn.gnome.org> (Slovenian translation)\n"
					"Γιάννης Κουτσούκος <giankoyt@gmail.com> (Greek translation)\n"
					"Efstathios Iosifidis <iosifidis@opensuse.org> (Greek translation)\n"
					"Samir Ribić <megaribi@epn.ba> (Bosnian translation)\n"
					"Enrico Nicoletto <liverig@gmail.com> (Brazilian Portuguese translation)\n"
					"Sveinn í Felli <sv1@fellsnet.is> (Icelandic translation)\n"
					"Josef Andersson <josef.andersson@gmail.com> (Swedish translation)\n"
					"Pedro Albuquerque <palbuquerque73@gmail.com> (Portuguese translation)\n"
					"Mario Blättermann <mario.blaettermann@gmail.com> (German translation)\n"
					"Necdet Yücel <necdetyucel@gmail.com> (Turkish translation)\n"
					"Andy Daniel Cruz Campos <god_of_war_2@hotmail.com> (Spanish translation)\n"
					"Mирослав Николић <miroslavnikolic@rocketmail.com> (Serbian translation)\n"
					"Balázs Meskó <meskobalazs@gmail.com> (Hungarian translation)\n"
					"Laudivan Freire de Almeida <laudivan@riseup.net> (Brazilian Portuguese translation)\n"
					"Bernd Homuth <dev@hmt.im> (German translation)\n"
					"Paul Feuvraux and Claude Paroz <claude@2xlibre.net> (French translation)\n"
					"Anders Jonsson <anders.jonsson@norsjovallen.se> (Swedish translation)\n"

					      ));
	gchar* artists[] = { "Wiki Graphic Designer",
			     "Aly Raj",
			     "Mathilde Agostini",
			     NULL };
	gchar* comments = { _("Locate Internet Radio Stations") };
	gchar* copyright = { _("Copyright (C) 2014-2017  Ole Aamot Software") };
	gchar* documenters[] = { _("Mario Blättermann <mario.blaettermann@gmail.com> (German translation)"), _("Marek Černocký <marek@manet.cz> (Czech translation)"), _("Daniel Mustieles <daniel.mustieles@gmail.com> (Spanish translation)"), _("Josef Andersson <josef.andersson@fripost.org> (Swedish translation)"), NULL };

	static GdkPixbuf* logo;

	gchar* name = PACKAGE;
	gchar* version = VERSION;
	gchar* website = "https://people.gnome.org/~ole/girl/";
	gchar* website_label = "https://people.gnome.org/~ole/girl/";

	gchar* license = g_strdup(_("This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.\nYou should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA."));

	if (!logo) {
		GError* error = NULL;
		logo = gdk_pixbuf_new_from_file (GIRL_DATADIR "/girl.png", &error);
		if (error) {
			g_warning (_("Unable to load GIRL logo: %s\n"), error->message);
			g_error_free (error);
		}
	}

	gtk_show_about_dialog (about,
			       "authors", authors,
			       "artists", artists,
			       "comments", comments,
			       "copyright", copyright,
			       "documenters", documenters,
			       "license", license,
			       "logo", logo,
			       "name", name,
			       "translator-credits", translators,
			       "version", version,
			       "website", website,
			       "website-label", website_label,
			       "wrap-license", TRUE,
			       NULL);
}

void about_listener(GtkWidget * a, gpointer user_data)
{
	static GtkWidget *about_listener = NULL;
	/* const gchar *translator_credits = _("translator_credits"); */
	const gchar *authors[] = {
		girl->selected_listener_name,
		NULL,
	};

	if (about_listener) {
		gdk_window_raise(about_listener->window);
		return;
	}

	about_listener = gnome_about_new(girl->selected_listener_name,
					girl->selected_listener_band,
					girl->selected_listener_location,
					girl->selected_listener_description,
					authors,
					NULL,
					NULL,
					girl->icon);
	g_signal_connect(G_OBJECT(about_listener), "destroy",
			 G_CALLBACK(gtk_widget_destroy), NULL);
	g_signal_connect(G_OBJECT(about_listener), "delete-event",
			 G_CALLBACK(gtk_widget_destroy), NULL);
	g_object_add_weak_pointer(G_OBJECT(about_listener), (void **) &(about_listener));
	gtk_widget_show(about_listener);
}

void about_program(GtkWidget * a, gpointer user_data)
{
	GtkWidget *about_program = NULL;
	
	/* const gchar *translator_credits = _("translator_credits"); */
	/* const gchar *authors[] = { */
	/* 	girl->selected_program_name, */
	/* 	NULL, */
	/* }; */

	/* about_program->window = gtk_window_new(GTK_WINDOW_TOPLEVEL); */
		
	if (about_program) {
		gdk_window_raise(about_program->window);
		gtk_widget_show(GTK_WIDGET(about_program->window));
		return;
	}

	if (strcmp(girl->selected_station_name, "KEXP")==0) {
		g_signal_connect(G_OBJECT(about_program), "destroy",
				 G_CALLBACK(gtk_widget_destroy), NULL);
		g_signal_connect(G_OBJECT(about_program), "delete-event",
				 G_CALLBACK(gtk_widget_destroy), NULL);
		g_object_add_weak_pointer(G_OBJECT(about_program), (void **) &(about_program));
		gtk_widget_show(about_program);
		printf("%s\n", "Test");
	}

}

void about_station(GirlData * a, gpointer user_data)
{
	static GtkWidget *about_station = NULL;
	/* const gchar *translator_credits = _("translator_credits"); */
	const gchar *authors[] = {
		girl->selected_station_name,
		NULL,
	};

	if (about_station != NULL) {
		gtk_widget_destroy(GTK_WIDGET(about_station));
	}
	
	if (about_station) {
		gdk_window_raise(about_station->window);
		return;
	}

	about_station = gnome_about_new(girl->selected_station_name,
					      girl->selected_station_band,
					      girl->selected_station_location,
					      girl->selected_station_description,
					      authors,
					      NULL,
					      NULL,
					      girl->icon);
	
	g_signal_connect(G_OBJECT(about_station), "destroy",
			 G_CALLBACK(gtk_widget_destroy), NULL);
	g_signal_connect(G_OBJECT(about_station), "delete-event",
			 G_CALLBACK(gtk_widget_destroy), NULL);
	g_object_add_weak_pointer(G_OBJECT(about_station), (void **) &(about_station));
	gtk_widget_show(about_station);
}

void about_streams(GtkWidget * a, gpointer user_data)
{
	static GtkWidget *about_streams = NULL;
	/* const gchar *translator_credits = _("translator_credits"); */
	const gchar *authors[] = {
		girl->selected_streams_uri,
		NULL,
	};

	if (about_streams) {
		gdk_window_raise(about_streams->window);
		return;
	}

	if (girl->selected_streams_uri != NULL) {
		about_streams = gnome_about_new(girl->selected_streams_uri,
						girl->selected_streams_codec,
						girl->selected_streams_mime,
						girl->selected_streams_bitrate,
						authors,
						NULL,
						NULL,
						girl->icon);
		g_signal_connect(G_OBJECT(about_streams), "destroy",
				 G_CALLBACK(gtk_widget_destroy), NULL);
		g_signal_connect(G_OBJECT(about_streams), "delete-event",
				 G_CALLBACK(gtk_widget_destroy), NULL);
		g_object_add_weak_pointer(G_OBJECT(about_streams), (void **) &(about_streams));
		gtk_widget_show(about_streams);
	}

}


void on_search_button_clicked(GtkWidget *a, gpointer user_data)
{
	GtkWidget *search;

	search_selector = create_search_selector();
	gtk_widget_show(search_selector);
	
	appbar_send_msg(_("Search radio station by location"));
}

void on_listen_button_clicked(GtkWidget *a, gpointer user_data)
{

	appbar_send_msg(_("Listening to the radio station %s in %s: %s "),
			girl->selected_station_name,
			girl->selected_station_location,
			girl->selected_station_uri,
			girl->selected_station_band);
	girl_helper_run(girl->selected_station_uri,
			girl->selected_station_name,
			GIRL_STREAM_SHOUTCAST,
			GIRL_STREAM_PLAYER);
}

void on_record_button_clicked(GtkWidget *a, gpointer user_data)
{
	GtkWidget *dialog;
	GCredentials *credentials;
	GError **err = NULL;

	dialog = gtk_message_dialog_new(GTK_WINDOW(girl_app),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_CLOSE,
					"Recording is not yet implemented.  If you want to implement it, see src/girl-record.c and send an email to ole@src.gnome.org with the patch.");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	
	credentials = g_credentials_new ();

	if (girl->selected_station_name != NULL) {
		/* girl_helper_run(girl->selected_station_uri, */
		/* 		girl->selected_station_name, */
		/* 		GIRL_STREAM_SHOUTCAST, */
		/* 		GIRL_STREAM_RECORD); */
		girl->selected_runners = girl_runners_new(g_credentials_get_unix_pid(credentials, err),
							  girl->selected_station_name,
							  "date",
							  "time",
							  "girl.wav");
	} else {
		dialog = gtk_message_dialog_new(GTK_WINDOW(girl_app),
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_CLOSE,
						"Could not record %s!",
						"girl.wav");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}
}

void on_stop_button_clicked(GtkWidget *a, gpointer user_data)
{

	if (girl->record_status == GIRL_RECORD_TRUE) {
		
		/* Close pid */

		GIRL_DEBUG_MSG("Recording stopped.\n");
		g_subprocess_force_exit(girl->record_subprocess);
		// g_spawn_close_pid( girl->record_pid);
		appbar_send_msg(_("Finished saving recording from the radio station %s in %s from %s to %s"),
				girl->selected_station_name,
				girl->selected_station_location,
				girl->selected_station_uri,
				girl->selected_archive_file);
		girl->record_status = GIRL_RECORD_FALSE;
		
	}

	girl_player_backend_pause();
	if (GTK_IS_WIDGET(girl->player_window)) {
		gtk_widget_destroy(girl->player_window);
	}

	if (girl->player_status == GIRL_PLAYER_TRUE) {
		GIRL_DEBUG_MSG("Playback stopped.\n");
		g_subprocess_force_exit(girl->player_subprocess);
		// g_spawn_close_pid( girl->player_pid);
		appbar_send_msg(_("Stopped playing the radio station %s in %s."),
				girl->selected_station_name,
				girl->selected_station_location,
				girl->selected_station_uri,
				girl->selected_station_band);
	}
	/* if (girl->player_status != GIRL_RECORD_TRUE) { */
	/* appbar_send_msg(_("Stop what?  You can \"Search\" by location, select a radio station from \"Stations\", or click on \"Listen\" and/or \"Record\".  Or go to \"Prev\" or \"Next\" radio station."), */
	/* 				girl->selected_station_name, */
	/* 				girl->selected_station_location, */
	/* 				girl->selected_station_uri, */
	/* 				girl->selected_station_band); */
	/* 	} */
	/* } */
}

GnomeVFSFileSize get_size(GnomeVFSURI * uri)
{
	GnomeVFSFileInfoOptions info_options;
	GnomeVFSFileInfo *info;
	GnomeVFSFileSize len;
	GnomeVFSResult result;

	info_options = GNOME_VFS_FILE_INFO_DEFAULT;

	info = gnome_vfs_file_info_new();
	result = gnome_vfs_get_file_info_uri(uri, info, info_options);

	if (result != GNOME_VFS_OK)
		return 0;

	len = (unsigned long long) info->size;
	gnome_vfs_file_info_unref(info);

	return len;
}

static gint
girl_archive_progress_callback(GnomeVFSXferProgressInfo * info, gpointer data)
{

	while (gtk_events_pending())
		gtk_main_iteration();

	appbar_send_msg(_("Archiving %s"), (gchar *) data);
	GIRL_DEBUG_MSG("%s", gnome_vfs_result_to_string(info->status));
	return TRUE;
}

gint girl_archive_new(gchar *title, gchar *file, gchar *codec)
	
{
	int ret;
	gchar *oldpath, *newpath;

	oldpath = g_strconcat(g_get_home_dir(), "/.girl/incomplete/ - .", strtok(codec, "/"), NULL);
	newpath = g_strconcat(file, NULL);
	
	GIRL_DEBUG_MSG("Renaming %s to %s\n", oldpath, newpath);
	
	ret = g_rename(oldpath, newpath);

	GIRL_DEBUG_MSG("Renamed %s to %s\n", oldpath, newpath);

	return ret;
}
