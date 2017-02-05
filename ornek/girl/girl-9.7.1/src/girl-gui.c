/* $Id$
 *
 * GNOME Internet Radio Locator
 *
 * Copyright (C) 2014, 2015, 2016, 2017  Ole Aamot Software
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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <sys/stat.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#if 0
#define G_DISABLE_DEPRECATED
#define GDK_DISABLE_DEPRECATED
#define GDK_PIXBUF_DISABLE_DEPRECATED
#define GTK_DISABLE_DEPRECATED
#define GNOME_DISABLE_DEPRECATED
#endif

#include <libgnome/libgnome.h>
/* #include <libgnome/gnome-desktop-item.h> */
#include <gtk/gtk.h>
#include <gtk/gtkcombobox.h>
#include <glib/gstdio.h>
#include "girl.h"
#include "girl-player-frontend.h"
#include "girl-gui.h"
#include "girl-program.h"
#include "girl-station.h"
#include "girl-streams.h"
#include "girl-tz.h"

extern GtkWidget *girl_app;
extern GtkWidget *search_selector;

GnomeUIInfo toolbar[] = {
	GNOMEUIINFO_ITEM_STOCK(N_("New"), N_("Add a new radio station"),
			       on_new_station_clicked, GTK_STOCK_NEW),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Search"), N_("Search by location for radio stations"),
			       on_search_button_clicked,
			       GTK_STOCK_FIND),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Listen"), N_("Listen to selected radio station"),
			       on_listen_button_clicked,
			       GTK_STOCK_MEDIA_PLAY),
#if HAVE_GIRL_RECORD == 1
	GNOMEUIINFO_ITEM_STOCK(N_("Record"), N_("Record selected radio station"),
			       on_record_button_clicked,
			       GTK_STOCK_MEDIA_RECORD),
#endif /* HAVE_GIRL_RECORD */	
	GNOMEUIINFO_ITEM_STOCK(N_("Stop"), N_("Stop recording selected radio station"),
			       on_stop_button_clicked,
			       GTK_STOCK_MEDIA_STOP),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Previous"),
			       N_("Go back to the previous radio station"),
			       on_previous_station_click, GTK_STOCK_GO_BACK),
	GNOMEUIINFO_ITEM_STOCK(N_("Stations"), N_("Internet Radio Stations"),
	 		       on_stations_selector_button_clicked,
			       GTK_STOCK_NETWORK),
	GNOMEUIINFO_ITEM_STOCK(N_("Next"), N_("Proceed to the next radio station"),
			       on_next_station_click, GTK_STOCK_GO_FORWARD),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Station"),
			       N_("About the current Station"),
			       about_station, GNOME_STOCK_ABOUT),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Program"),
			       N_("About the GNOME Internet Radio Locator"),
			       about_app, GNOME_STOCK_ABOUT),

	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(N_("Exit"),
			       N_("Quit the GNOME Internet Radio Locator"),
			       quit_app, GTK_STOCK_QUIT),
	GNOMEUIINFO_END,

};

/* 
   Reading list of stations from

   $PREFIX/share/girl/girl.xml 
   $HOME/.girl/girl.xml

*/

/* This is the signal handler that got connected to button
 * press/release events of the List
 */
void sigh_button_event( GtkWidget      *gtklist,
			GdkEventButton *event,
			GtkWidget      *frame )
{
	/* We only do something if the third (rightmost mouse button
	 * was released
	 */
	if (event->type==GDK_BUTTON_RELEASE &&
	    event->button==3) {
		GList           *dlist, *free_list;
		GtkWidget       *new_station;

		/* Fetch the currently selected list item which
		 * will be our next station ;)
		 */
		dlist=GTK_LIST(gtklist)->selection;
		if (dlist)
			new_station=GTK_WIDGET(dlist->data);
		else
			new_station=NULL;

		/* Look for already imprisoned list items, we
		 * will put them back into the list.
		 * Remember to free the doubly linked list that
		 * gtk_container_children() returns
		 */
		dlist=gtk_container_children(GTK_CONTAINER(frame));
		free_list=dlist;
		while (dlist) {
			GtkWidget       *list_item;

			list_item=dlist->data;

			gtk_widget_reparent(list_item, gtklist);

			dlist=dlist->next;
		}
		g_list_free(free_list);

		/* If we have a new station, remove station from the
		 * List and put station into the frame "Station History".
		 * We need to unselect the item first.
		 */
		if (new_station) {
			GList   static_dlist;

			static_dlist.data=new_station;
			// static_dlist.next=NULL;
			// static_dlist.prev=NULL;

			gtk_list_unselect_child(GTK_LIST(gtklist),
						new_station);
			gtk_widget_reparent(new_station, frame);
		}
	}
}

/* This is the signal handler that gets called if List
 * emits the "selection_changed" signal
 */
void sigh_print_selection( GtkWidget *gtklist,
			   gpointer   func_data )
{
	GList   *dlist;

	/* Fetch the doubly linked list of selected items
	 * of the List, remember to treat this as read-only!
	 */
	dlist=GTK_LIST(gtklist)->selection;

	/* If there are no selected items there is nothing more
	 * to do than just telling the user so
	 */
	if (!dlist) {
		g_print("Selection cleared\n");
		return;
	}
	/* Ok, we got a selection and so we print it
	 */
	g_print("The selection is a ");


	/* Get the list item from the doubly linked list
	 * and then query the data associated with list_item_data_key.
	 * We then just print it */
	while (dlist) {
		GtkObject       *list_item;
		gchar           *item_data_string;

		list_item=GTK_OBJECT(dlist->data);
		item_data_string=gtk_object_get_data(list_item,
						     list_item_data_key);
		appbar_send_msg(_("Playing %s"), item_data_string);

		girl_helper_run(item_data_string,
				"Historic station",
				GIRL_STREAM_SHOUTCAST,
				GIRL_STREAM_PLAYER);
		
		dlist=dlist->next;
	}
}

GtkWidget *create_listeners_selector(char *selected_listener_uri,
				     char *filename)
{
	GirlListenerInfo *listenerinfo, *locallistener;

	GtkWidget *listeners_selector;
	GtkWidget *align, *menu, *drop_down, *item;

	gchar *listener_uri, *listener_name, *listener_location, *listener_band, *listener_description;
	gchar *label, *world_listener_xml_uri,
	    *local_listener_xml_file;

	int i = 0, selection = -1;

	/* The Listeners dialog */
	listeners_selector = gtk_dialog_new_with_buttons(_("Select a listener"), GTK_WINDOW(girl_app), 0,	/* flags */
							 GTK_STOCK_CLOSE,
							 GTK_RESPONSE_ACCEPT,
							 NULL);
	gtk_container_set_border_width
	    (GTK_CONTAINER(GTK_DIALOG(listeners_selector)->vbox), 6);

	align = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER
			  (GTK_DIALOG(listeners_selector)->vbox), align);
	gtk_container_set_border_width(GTK_CONTAINER(align), 6);
	gtk_widget_show(align);

	menu = gtk_menu_new();
	gtk_widget_show(menu);

	/* creating the menu items */

	world_listener_xml_uri = gnome_program_locate_file(NULL,
							   GNOME_FILE_DOMAIN_APP_DATADIR,
							   "girl/listeners.xml",
							   FALSE,
							   NULL);
	GIRL_DEBUG_MSG("world_listener_xml_uri = %s\n",
	    world_listener_xml_uri);

	if (world_listener_xml_uri == NULL) {
		g_warning(_("Failed to open %s.  Please install it.\n"),
			  world_listener_xml_uri);
	}

	local_listener_xml_file =
	    g_strconcat(g_get_home_dir(), "/.girl/listeners.xml", NULL);
	locallistener =
	    girl_listener_load_from_http(NULL, local_listener_xml_file);

	if (locallistener == NULL) {
		g_warning(_("Failed to open %s\n"),
			  local_listener_xml_file);
	}

/*   g_free (local_listener_xml_file); */

	listenerinfo =
	    girl_listener_load_from_file(locallistener,
					 world_listener_xml_uri);

	/* girl_listeners = NULL; */

	while (listenerinfo != NULL) {

		label =
		    g_strconcat(listenerinfo->name, " (",
				listenerinfo->location, ")", NULL);
		listener_uri = g_strdup(listenerinfo->uri);
		listener_name = g_strdup(listenerinfo->name);
		listener_location = g_strdup(listenerinfo->location);
		listener_description = g_strdup(listenerinfo->description);
		listener_band = g_strdup(listenerinfo->band);

		/* girl_listeners = g_list_append(girl_listeners,(GirlListenerInfo *)listenerinfo); */

		if (label != NULL) {
			item = gtk_menu_item_new_with_label(label);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
			g_signal_connect(G_OBJECT(item), "activate",
					 G_CALLBACK
					 (on_listeners_selector_changed),
					 NULL);
			g_object_set_data(G_OBJECT(item), "listener_uri",
					  (gpointer) listener_uri);
			g_object_set_data(G_OBJECT(item), "listener_name",
					  (gpointer) listener_name);
			g_object_set_data(G_OBJECT(item),
					  "listener_location",
					  (gpointer) listener_location);
			g_object_set_data(G_OBJECT(item),
					  "listener_band",
					  (gpointer) listener_band);
			g_object_set_data(G_OBJECT(item),
					  "listener_description",
					  (gpointer) listener_description);
			gtk_widget_show(item);
			g_free(label);

			/* selection */

			GIRL_DEBUG_MSG("selected_listener_uri: %s\n",
			       selected_listener_uri);

			GIRL_DEBUG_MSG("listener_uri: %s\n", listener_uri);

			if (selected_listener_uri != NULL
			    && listener_uri != NULL
			    && !strcmp(selected_listener_uri,
				       listener_uri))
				selection = i;
		} else {
			g_free(listener_uri);
			g_free(listener_name);
			g_free(listener_location);
			g_free(listener_description);
		}
		i++;
		listenerinfo = listenerinfo->next;
	}

	drop_down = gtk_option_menu_new();
	gtk_widget_show(drop_down);
	gtk_option_menu_set_menu(GTK_OPTION_MENU(drop_down), menu);
	gtk_container_add(GTK_CONTAINER(align), drop_down);

	if (selection != -1)
		gtk_option_menu_set_history(GTK_OPTION_MENU(drop_down), selection);

	g_signal_connect(G_OBJECT(listeners_selector), "response",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) listeners_selector);
	g_signal_connect(G_OBJECT(listeners_selector), "delete-event",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) listeners_selector);

	return listeners_selector;
}

GtkWidget *create_programs_selector(char *selected_program_uri,
				    char *filename)
{
	GirlProgramInfo *programinfo, *localprogram;
	GtkWidget *programs_selector;
	GtkWidget *align, *menu, *drop_down, *item;

	gchar *program_uri, *program_name, *program_location, *program_band, *program_description;
	gchar *label, *world_program_xml_filename, *local_program_xml_file;

	int i = 0, selection = -1;

	/* The Programs dialog */
	programs_selector = gtk_dialog_new_with_buttons(_("Select a program"), GTK_WINDOW(girl_app), 0,	/* flags */
							GTK_STOCK_CLOSE,
							GTK_RESPONSE_ACCEPT,
							NULL);
	gtk_container_set_border_width
	    (GTK_CONTAINER(GTK_DIALOG(programs_selector)->vbox), 6);

	align = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER
			  (GTK_DIALOG(programs_selector)->vbox), align);
	gtk_container_set_border_width(GTK_CONTAINER(align), 6);
	gtk_widget_show(align);

	menu = gtk_menu_new();
	gtk_widget_show(menu);

	/* creating the menu items */

	/* world_program_xml_filename = gnome_program_locate_file(NULL, */
	/* 						   GNOME_FILE_DOMAIN_APP_DATADIR, */
	/* 						   "girl/programs.xml", */
	/* 						   FALSE, */
	/* 						   NULL); */
	world_program_xml_filename = g_strconcat(GIRL_DATADIR, "/programs.xml", NULL);

	/* world_program_xml_filename = g_strdup("http://girl.src.oka.no/programs.xml"); */

	GIRL_DEBUG_MSG("world_program_xml_filename = %s\n",
	    world_program_xml_filename);

	if (world_program_xml_filename == NULL) {
		g_warning(_("Failed to open %s.\n"),
			  world_program_xml_filename);
	}

	local_program_xml_file =
	    g_strconcat(g_get_home_dir(), "/.girl/programs.xml", NULL);
	localprogram =
	    girl_program_load_from_file(NULL, local_program_xml_file);

	if (localprogram == NULL) {
		g_warning(_("Failed to open %s.\n"), local_program_xml_file);
	}

/*   g_free (local_program_xml_file); */

	programinfo =
	    girl_program_load_from_file(localprogram,
					world_program_xml_filename);

	girl_programs = NULL;

	while (programinfo != NULL) {

		label =
		    g_strconcat(programinfo->name, " (",
				programinfo->location, ")", NULL);
		program_uri = g_strdup(programinfo->archive->uri);
		program_name = g_strdup(programinfo->name);
		program_location = g_strdup(programinfo->location);
		program_band = g_strdup(programinfo->band);
		program_description = g_strdup(programinfo->description);

		girl_programs = g_list_append(girl_programs,(GirlProgramInfo *)programinfo);

		if (label != NULL) {
			item = gtk_menu_item_new_with_label(label);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
			g_signal_connect(G_OBJECT(item), "activate",
					 G_CALLBACK
					 (on_programs_selector_changed),
					 NULL);
			g_object_set_data(G_OBJECT(item), "program_uri",
					  (gpointer) program_uri);
			g_object_set_data(G_OBJECT(item), "program_name",
					  (gpointer) program_name);
			g_object_set_data(G_OBJECT(item),
					  "program_location",
					  (gpointer) program_location);
			g_object_set_data(G_OBJECT(item),
					  "program_band",
					  (gpointer) program_band);
			g_object_set_data(G_OBJECT(item),
					  "program_description",
					  (gpointer) program_description);
			gtk_widget_show(item);
			g_free(label);

			/* selection */
			if (selected_program_uri != NULL &&
			    !strcmp(selected_program_uri, program_uri))
				selection = i;
		} else {
			g_free(program_uri);
			g_free(program_name);
			g_free(program_location);
			g_free(program_band);
			g_free(program_description);
		}
		i++;
		programinfo = programinfo->next;
	}

	drop_down = gtk_option_menu_new();
	gtk_widget_show(drop_down);
	gtk_option_menu_set_menu(GTK_OPTION_MENU(drop_down), menu);
	gtk_container_add(GTK_CONTAINER(align), drop_down);

	if (selection != -1)
		gtk_option_menu_set_history(GTK_OPTION_MENU(drop_down), selection);

	g_signal_connect(G_OBJECT(programs_selector), "response",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) programs_selector);
	g_signal_connect(G_OBJECT(programs_selector), "delete-event",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) programs_selector);

	return programs_selector;
}

static gboolean
on_search_matches(GtkEntryCompletion *widget,
		  GtkTreeModel *model,
		  GtkTreeIter *iter,
		  gpointer user_data)
{
	GValue value = {0, };

	gtk_tree_model_get_value(model, iter, STATION_URI, &value);
	girl->selected_station_uri = g_strdup(g_value_get_string(&value));
	g_value_unset(&value);

	gtk_tree_model_get_value(model, iter, STATION_NAME, &value);
	girl->selected_station_name = g_strdup(g_value_get_string(&value));
	g_value_unset(&value);	

	gtk_tree_model_get_value(model, iter, STATION_LOCATION, &value);
	girl->selected_station_location = g_strdup(g_value_get_string(&value));
	g_value_unset(&value);

	gtk_tree_model_get_value(model, iter, STATION_DESCRIPTION, &value);
	girl->selected_station_description = g_strdup(g_value_get_string(&value));
	g_value_unset(&value);

	appbar_send_msg(_("Your search by location resulted in the radio station %s in %s: %s"),
			girl->selected_station_name,
			girl->selected_station_location,
			girl->selected_station_uri);

	girl_player_frontend_stop (girl->player_window, NULL);

	if (GTK_IS_WIDGET(search_selector)) {
		gtk_widget_destroy(search_selector);
	}
	
	girl_helper_run(girl->selected_station_uri,
			girl->selected_station_name,
			GIRL_STREAM_SHOUTCAST,
			GIRL_STREAM_PLAYER);
	
	return FALSE;
}

GtkWidget *create_search_selector(void) {

	GirlStationInfo *stationinfo, *localstation;
	GtkWidget *search_selector, *content_area;
	GtkWidget *align, *textentry;
	GtkEntryCompletion *completion;
	GtkListStore *model;
	GtkTreeIter iter;
	
	gchar *world_station_xml_filename, *local_station_xml_file;

	/* int i = 0, search_selection = -1; */

	GStatBuf stats;

	memset(&stats, 0, sizeof(stats));

	/* The Stations dialog */
	search_selector = gtk_dialog_new_with_buttons(_("Search by location"), GTK_WINDOW(girl_app), 0,	/* flags */
							GTK_STOCK_CLOSE,
							GTK_RESPONSE_ACCEPT,
							NULL);
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (search_selector));
	gtk_container_set_border_width
	    (GTK_CONTAINER(GTK_DIALOG(search_selector)->vbox), 6);

	align = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER
			  (GTK_DIALOG(search_selector)->vbox), align);
	gtk_container_set_border_width(GTK_CONTAINER(align), 6);
	gtk_widget_show(align);

	textentry = gtk_entry_new();
	completion = gtk_entry_completion_new();
	gtk_entry_completion_set_text_column(completion, STATION_NAME);
	gtk_entry_completion_set_text_column(completion, STATION_LOCATION);
	gtk_entry_set_completion(GTK_ENTRY(textentry), completion);
	g_signal_connect(G_OBJECT(completion), "match-selected",
			 G_CALLBACK(on_search_matches), NULL);
	model = gtk_list_store_new(11, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

	world_station_xml_filename = g_strconcat(GIRL_DATADIR, "/girl.xml", NULL);
	GIRL_DEBUG_MSG("world_station_xml_filename = %s\n",
	    world_station_xml_filename);

	if (world_station_xml_filename == NULL) {
		g_warning(("Failed to open %s.  Please install it.\n"),
			  world_station_xml_filename);
	}

	local_station_xml_file =
	    g_strconcat(g_get_home_dir(), "/.girl/girl.xml", NULL);

	if (!g_stat(local_station_xml_file, &stats)) {
		localstation = girl_station_load_from_file(NULL, local_station_xml_file);
	} else {
		localstation = NULL;
	}

	if (localstation == NULL) {
		g_warning(_("Failed to open %s.\n"), local_station_xml_file);
	}

/*   g_free (local_station_xml_file); */

	stationinfo =
	    girl_station_load_from_file(localstation,
					world_station_xml_filename);

	girl_stations = NULL;

	while (stationinfo != NULL) {

		gtk_list_store_append(model, &iter);
		gtk_list_store_set(model,
				   &iter,
				   STATION_NAME,
				   stationinfo->name,
				   STATION_LOCATION,
				   stationinfo->location,
				   STATION_URI,
				   stationinfo->stream->uri,
				   STATION_DESCRIPTION,
				   stationinfo->description,
				   STATION_FREQUENCY,
				   stationinfo->frequency,
				   STATION_BAND,
				   stationinfo->band,
				   STATION_TYPE,
				   stationinfo->type,
				   STATION_RANK,
				   stationinfo->rank,
				   STATION_BITRATE,
				   stationinfo->bitrate,
				   STATION_SAMPLERATE,
				   stationinfo->samplerate,
				   STATION_ID,
				   stationinfo->id,
				   -1);

		stationinfo = stationinfo->next;
	}

	gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(model));

	gtk_widget_show(textentry);	
	gtk_container_add(GTK_CONTAINER(content_area), textentry);
	
#if 0
	/* label = */
		/*     g_strconcat(stationinfo->name, " (", */
		/* 		stationinfo->location, ")", NULL); */
		/* station_uri = g_strdup(stationinfo->stream->uri); */
		/* station_name = g_strdup(stationinfo->name); */
		/* station_location = g_strdup(stationinfo->location); */
		/* station_band = g_strdup(stationinfo->band); */
		/* station_description = g_strdup(stationinfo->description); */
		/* station_website = g_strdup(stationinfo->uri); */
		
		/* girl_stations = g_list_append(girl_stations,(GirlStationInfo *)stationinfo); */

		if (label != NULL) {
			/* item = gtk_menu_item_new_with_label(label); */
			/* gtk_menu_shell_append(GTK_MENU_SHELL(menu), item); */
			g_signal_connect(G_OBJECT(item), "activate",
					 G_CALLBACK
					 (on_stations_selector_changed),
					 NULL);
			g_object_set_data(G_OBJECT(item), "station_uri",
					  (gpointer) station_uri);
			g_object_set_data(G_OBJECT(item), "station_name",
					  (gpointer) station_name);
			g_object_set_data(G_OBJECT(item),
					  "station_location",
					  (gpointer) station_location);
			g_object_set_data(G_OBJECT(item),
					  "station_band",
					  (gpointer) station_band);
			g_object_set_data(G_OBJECT(item),
					  "station_description",
					  (gpointer) station_description);
			g_object_set_data(G_OBJECT(item),
					  "station_website",
					  (gpointer) station_website);
			gtk_widget_show(item);
			g_free(label);

			/* selection */
#if 0 /* FIXME */
			if (selected_station_uri != NULL && 
			    !strcmp(selected_station_uri, station_uri))
				search_selection = i;
#endif
		} else {
			g_free(station_uri);
			g_free(station_name);
			g_free(station_location);
			g_free(station_band);
			g_free(station_description);
		}
		i++;
	}
	
	/* drop_down = gtk_option_menu_new(); */
	/* gtk_widget_show(drop_down); */
	/* gtk_option_menu_set_menu(GTK_OPTION_MENU(drop_down), menu); */
	/* gtk_container_add(GTK_CONTAINER(align), drop_down); */

	/* if (selection != -1) */
	/* 	gtk_option_menu_set_history(GTK_OPTION_MENU(drop_down), selection); */
#endif	
	g_signal_connect(G_OBJECT(search_selector), "response",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) search_selector);
	g_signal_connect(G_OBJECT(search_selector), "delete-event",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) search_selector);

	return search_selector;
}

GtkWidget *create_stations_selector(char *selected_station_uri,
				    char *filename)
{
	GirlStationInfo *stationinfo, *localstation;
	GtkWidget *stations_selector;
	GtkWidget *align, *menu, *drop_down, *item;

	gchar *station_uri, *station_name, *station_location, *station_band, *station_description, *station_website;
	gchar *label, *world_station_xml_filename, *local_station_xml_file;

	int i = 0, selection = -1;

	GStatBuf stats;

	memset(&stats, 0, sizeof(stats));

	/* The Stations dialog */
	stations_selector = gtk_dialog_new_with_buttons(_("Select a station"), GTK_WINDOW(girl_app), 0,	/* flags */
							GTK_STOCK_CLOSE,
							GTK_RESPONSE_ACCEPT,
							NULL);
	gtk_container_set_border_width
	    (GTK_CONTAINER(GTK_DIALOG(stations_selector)->vbox), 6);

	align = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER
			  (GTK_DIALOG(stations_selector)->vbox), align);
	gtk_container_set_border_width(GTK_CONTAINER(align), 6);
	gtk_widget_show(align);

	menu = gtk_menu_new();
	gtk_widget_show(menu);

	/* creating the menu items */

	/* world_station_xml_filename = gnome_program_locate_file(NULL, */
	/* 						       GNOME_FILE_DOMAIN_APP_DATADIR, */
	/* 						       "girl/girl.xml", */
	/* 						       FALSE, */
	/* 						       NULL); */

	/* world_station_xml_filename = g_strdup("http://girl.src.oka.no/girl.xml"); */

	world_station_xml_filename = g_strconcat(GIRL_DATADIR, "/girl.xml", NULL);
	GIRL_DEBUG_MSG("world_station_xml_filename = %s\n",
	    world_station_xml_filename);

	if (world_station_xml_filename == NULL) {
		g_warning(_("Failed to open %s.\n"),
			  world_station_xml_filename);
	}

	local_station_xml_file =
	    g_strconcat(g_get_home_dir(), "/.girl/girl.xml", NULL);

	if (!g_stat(local_station_xml_file, &stats)) {
		localstation = girl_station_load_from_file(NULL, local_station_xml_file);
	} else {
		localstation = NULL;
	}

	if (localstation == NULL) {
		g_warning(_("Failed to open %s\n"), local_station_xml_file);
	}

/*   g_free (local_station_xml_file); */

	stationinfo =
	    girl_station_load_from_file(localstation,
					world_station_xml_filename);

	girl_stations = NULL;

	while (stationinfo != NULL) {

		label =
		    g_strconcat(stationinfo->name, " (",
				stationinfo->location, ")", NULL);
		station_uri = g_strdup(stationinfo->stream->uri);
		station_name = g_strdup(stationinfo->name);
		station_location = g_strdup(stationinfo->location);
		station_band = g_strdup(stationinfo->band);
		station_description = g_strdup(stationinfo->description);
		station_website = g_strdup(stationinfo->uri);
		
		girl_stations = g_list_append(girl_stations,(GirlStationInfo *)stationinfo);

		if (label != NULL) {
			item = gtk_menu_item_new_with_label(label);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
			g_signal_connect(G_OBJECT(item), "activate",
					 G_CALLBACK
					 (on_stations_selector_changed),
					 NULL);
			g_object_set_data(G_OBJECT(item), "station_uri",
					  (gpointer) station_uri);
			g_object_set_data(G_OBJECT(item), "station_name",
					  (gpointer) station_name);
			g_object_set_data(G_OBJECT(item),
					  "station_location",
					  (gpointer) station_location);
			g_object_set_data(G_OBJECT(item),
					  "station_band",
					  (gpointer) station_band);
			g_object_set_data(G_OBJECT(item),
					  "station_description",
					  (gpointer) station_description);
			g_object_set_data(G_OBJECT(item),
					  "station_website",
					  (gpointer) station_website);
			gtk_widget_show(item);
			g_free(label);

			/* selection */
			if (selected_station_uri != NULL &&
			    !strcmp(selected_station_uri, station_uri))
				selection = i;
		} else {
			g_free(station_uri);
			g_free(station_name);
			g_free(station_location);
			g_free(station_band);
			g_free(station_description);
		}
		i++;
		stationinfo = stationinfo->next;
	}

	drop_down = gtk_option_menu_new();
	gtk_widget_show(drop_down);
	gtk_option_menu_set_menu(GTK_OPTION_MENU(drop_down), menu);
	gtk_container_add(GTK_CONTAINER(align), drop_down);

	if (selection != -1)
		gtk_option_menu_set_history(GTK_OPTION_MENU(drop_down), selection);

	g_signal_connect(G_OBJECT(stations_selector), "response",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) stations_selector);
	g_signal_connect(G_OBJECT(stations_selector), "delete-event",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) stations_selector);

	return stations_selector;
}

static gboolean
on_location_matches(GtkEntryCompletion *widget,
		    GtkTreeModel *model,
		    GtkTreeIter *iter,
		    gpointer user_data)
{
	GValue value = {0, };

	gtk_tree_model_get_value(model, iter, STATION_LOCATION, &value);
	girl->selected_station_location = g_strdup(g_value_get_string(&value));
	g_value_unset(&value);

	appbar_send_msg(_("Found location %s"),
			girl->selected_station_location);
	/* girl_helper_run(girl->selected_station_uri, */
	/* 		girl->selected_station_name, */
	/* 		GIRL_STREAM_SHOUTCAST, */
	/* 		GIRL_STREAM_PLAYER); */
	return FALSE;
}

GtkWidget *create_new_station_selector(void) {

	GirlStationInfo *localstation, *stationinfo;
	GtkWidget *station_selector, *content_area;
	GtkWidget *align;
	GtkWidget *bandentry, *descriptionentry, *nameentry, *locationentry, *urientry, *websiteentry;
	GtkEntryCompletion *completion;
	GtkListStore *location_model;
	GtkTreeIter iter;
	TzDB *db;
	GPtrArray *locs;
	guint i;
	char *pixmap_dir = NULL;
	gchar *path = NULL;
	setlocale (LC_ALL, "C");

	gchar *world_station_xml_filename, *local_station_xml_file;
	gint retval;
	
	/* int i = 0, search_selection = -1; */

	GStatBuf stats;

	memset(&stats, 0, sizeof(stats));

	/* The Stations dialog */
	station_selector = gtk_dialog_new_with_buttons(_("New radio station"), GTK_WINDOW(girl_app), 0,	/* flags */
						       GTK_STOCK_SAVE,
						       GTK_RESPONSE_ACCEPT,
						       NULL);
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (station_selector));
	gtk_container_set_border_width
	    (GTK_CONTAINER(GTK_DIALOG(station_selector)->vbox), 6);

	align = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER
			  (GTK_DIALOG(station_selector)->vbox), align);
	gtk_container_set_border_width(GTK_CONTAINER(align), 6);
	gtk_widget_show(align);

	nameentry = gtk_entry_new();
	locationentry = gtk_entry_new();
	urientry = gtk_entry_new();
	bandentry = gtk_entry_new();
	websiteentry = gtk_entry_new();
	descriptionentry = gtk_entry_new();

	gtk_entry_set_text(GTK_ENTRY(nameentry), "Station name");
	gtk_entry_set_text(GTK_ENTRY(locationentry), "City name");
	gtk_entry_set_text(GTK_ENTRY(urientry), "http://uri-to-stream/");
	gtk_entry_set_text(GTK_ENTRY(bandentry), "FM/AM bandwidth");
	gtk_entry_set_text(GTK_ENTRY(websiteentry), "http://uri-to-website/");
	gtk_entry_set_text(GTK_ENTRY(descriptionentry), "Description");
	completion = gtk_entry_completion_new();
	gtk_entry_completion_set_text_column(completion, STATION_LOCATION);
	gtk_entry_set_completion(GTK_ENTRY(locationentry), completion);
	g_signal_connect(G_OBJECT(completion), "match-selected",
			 G_CALLBACK(on_location_matches), NULL);
	location_model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

	world_station_xml_filename = g_strconcat(GIRL_DATADIR, "/girl.xml", NULL);
	GIRL_DEBUG_MSG("world_station_xml_filename = %s\n",
	    world_station_xml_filename);

	if (world_station_xml_filename == NULL) {
		g_warning(("Failed to open %s.  Please install it.\n"),
			  world_station_xml_filename);
	}

	local_station_xml_file =
	    g_strconcat(g_get_home_dir(), "/.girl/girl.xml", NULL);

	if (!g_stat(local_station_xml_file, &stats)) {
		localstation = girl_station_load_from_file(NULL, local_station_xml_file);
	} else {
		localstation = NULL;
	}

	if (localstation == NULL) {
		g_warning(_("Failed to open %s.\n"), local_station_xml_file);
	}

/*   g_free (local_station_xml_file); */

	stationinfo =
	    girl_station_load_from_file(localstation,
					world_station_xml_filename);

	// girl_stations = NULL;

	/* while (stationinfo != NULL) { */
	/* Timezone map */
	db = tz_load_db ();
	locs = tz_get_locations (db);
	for (i = 0; i < locs->len ; i++) {
		TzLocation *loc = locs->pdata[i];
		TzInfo *info;
		char *filename;
		gdouble selected_offset;
		char buf[16];
		info = tz_info_from_location (loc);
		selected_offset = tz_location_get_utc_offset (loc)
			/ (60.0*60.0) + ((info->daylight) ? -1.0 : 0.0);
		filename = g_strdup_printf ("timezone_%s.png",
					    g_ascii_formatd (buf, sizeof (buf),
							     "%g", selected_offset));
		path = g_build_filename (pixmap_dir, filename, NULL);
		/* g_printf("Name is %s\n", tz_info_get_clean_name(db, loc->zone)); */
		/* GIRL_DEBUG_MSG("%s\n", loc->zone); */
		/* if (g_file_test (path, G_FILE_TEST_IS_REGULAR) == FALSE) { */
		/* 	g_message ("File '%s' missing for zone '%s'", filename, loc->zone); */
		gtk_list_store_append(location_model, &iter);
		/* g_print("%s %s", stationinfo->location, loc->zone); */
		/* if (g_strcmp0(stationinfo->location, loc->zone)==0) { */
		gtk_list_store_set(location_model,
				   &iter,
				   STATION_LOCATION,
				   loc->zone,
				   -1);
		/* } */
		retval = 1;
	}
	/* 	stationinfo = stationinfo->next; */
	/* }  */
	gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(location_model));
	gtk_widget_show(nameentry);
	gtk_widget_show(locationentry);
	gtk_widget_show(urientry);
	gtk_widget_show(bandentry);
	gtk_widget_show(descriptionentry);
	gtk_widget_show(websiteentry);
	gtk_container_add(GTK_CONTAINER(content_area), nameentry);
	gtk_container_add(GTK_CONTAINER(content_area), locationentry);
	gtk_container_add(GTK_CONTAINER(content_area), urientry);
	gtk_container_add(GTK_CONTAINER(content_area), bandentry);
	gtk_container_add(GTK_CONTAINER(content_area), descriptionentry);
	gtk_container_add(GTK_CONTAINER(content_area), websiteentry);
	/* g_signal_connect(G_OBJECT(station_selector), GTK_RESPONSE_ACCEPT, */
	/* 		 G_CALLBACK(on_new_station_selector_changed), */
	/* 		 NULL); */
	g_object_set_data(G_OBJECT(station_selector), "station_location",
			  (gpointer) gtk_entry_get_text(GTK_ENTRY(locationentry)));
	g_object_set_data(G_OBJECT(station_selector), "station_name",
			  (gpointer) gtk_entry_get_text(GTK_ENTRY(nameentry)));
	g_object_set_data(G_OBJECT(station_selector), "station_uri",
			  (gpointer) gtk_entry_get_text(GTK_ENTRY(urientry)));
	g_object_set_data(G_OBJECT(station_selector), "station_band",
			  (gpointer) gtk_entry_get_text(GTK_ENTRY(bandentry)));
	g_object_set_data(G_OBJECT(station_selector), "station_description",
			  (gpointer) gtk_entry_get_text(GTK_ENTRY(descriptionentry)));
	g_object_set_data(G_OBJECT(station_selector), "station_website",
			  (gpointer) gtk_entry_get_text(GTK_ENTRY(websiteentry)));

#if 0 /* FIXME: Add input fields */
	g_object_set_data(G_OBJECT(station_selector), "station_band",
			  (gpointer) station_band);
	g_object_set_data(G_OBJECT(station_selector), "station_description",
			  (gpointer) station_description);
	g_object_set_data(G_OBJECT(station_selector), "station_website",
			  (gpointer) station_website);
#endif
	// gtk_widget_show(station_selector);
	// g_free(label);
	g_signal_connect(G_OBJECT(station_selector), "response",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) station_selector);
	g_signal_connect(G_OBJECT(station_selector), "delete-event",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) station_selector);
	tz_db_free (db);
	g_free (pixmap_dir);
	/* g_free (filename); */
	/* g_free (path); */
	return station_selector;
}

GtkWidget *create_streams_selector(char *selected_streams_uri,
				    char *filename)
{
	GirlStreamsInfo *streamsinfo, *localstreams;
	GtkWidget *streams_selector;
	GtkWidget *align, *menu, *drop_down, *item;

	gchar *streams_mime, *streams_uri, *streams_codec, *streams_bitrate, *streams_samplerate;
	GirlChannels streams_channels;
	gchar *label, *world_streams_xml_filename, *local_streams_xml_file;

	int i = 0, selection = -1;

	GStatBuf stats;

	/* The Streams dialog */
	streams_selector = gtk_dialog_new_with_buttons(_("Select a stream"), GTK_WINDOW(girl_app), 0,	/* flags */
							GTK_STOCK_CLOSE,
							GTK_RESPONSE_ACCEPT,
							NULL);
	gtk_container_set_border_width
	    (GTK_CONTAINER(GTK_DIALOG(streams_selector)->vbox), 6);

	align = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER
			  (GTK_DIALOG(streams_selector)->vbox), align);
	gtk_container_set_border_width(GTK_CONTAINER(align), 6);
	gtk_widget_show(align);

	menu = gtk_menu_new();
	gtk_widget_show(menu);

	/* creating the menu items */

	/* world_streams_xml_filename = gnome_program_locate_file(NULL, */
	/* 						       GNOME_FILE_DOMAIN_APP_DATADIR, */
	/* 						       "girl/streams.xml", */
	/* 						       FALSE, */
	/* 						       NULL); */
	world_streams_xml_filename = g_strconcat(GIRL_DATADIR, "/streams.xml", NULL);
	/* world_streams_xml_filename = g_strdup("http://girl.src.oka.no/streams.xml"); */
	GIRL_DEBUG_MSG("world_streams_xml_filename = %s\n",
	    world_streams_xml_filename);

	if (world_streams_xml_filename == NULL) {
		g_warning(_("Failed to open %s.\n"),
			  world_streams_xml_filename);
	}

	local_streams_xml_file =
	    g_strconcat(g_get_home_dir(), "/.girl/streams.xml", NULL);

	if (!g_stat(local_streams_xml_file, &stats)) {
		localstreams = girl_streams_load_from_file(NULL, local_streams_xml_file);
	} else {
		localstreams = NULL;
	}

	if (!g_stat("~/.gnome2/girl", &stats)) {
		if (localstreams == NULL) {
			g_warning(_("Failed to open %s\n"), local_streams_xml_file);
		}
	}

	streamsinfo = girl_streams_load_from_file(localstreams, world_streams_xml_filename);

	girl_streams = NULL;

	while (streamsinfo != NULL) {

		label =
		    g_strconcat(streamsinfo->uri, " (",
				streamsinfo->mime, ")", NULL);
		streams_uri = g_strdup(streamsinfo->uri);
		streams_mime = g_strdup(streamsinfo->mime);
		streams_codec = g_strdup(streamsinfo->codec);
		streams_bitrate = g_strdup(streamsinfo->bitrate);
		streams_samplerate = g_strdup(streamsinfo->samplerate);
		streams_channels = streamsinfo->channels;
		girl_streams = g_list_append(girl_streams,(GirlStreamsInfo *)streamsinfo);

		if (label != NULL) {
			item = gtk_menu_item_new_with_label(label);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
			g_signal_connect(G_OBJECT(item), "activate",
					 G_CALLBACK
					 (on_streams_selector_changed),
					 NULL);
			g_object_set_data(G_OBJECT(item), "streams_uri",
					  (gpointer) streams_uri);
			g_object_set_data(G_OBJECT(item), "streams_mime",
					  (gpointer) streams_mime);
			g_object_set_data(G_OBJECT(item),
					  "streams_codec",
					  (gpointer) streams_codec);
			g_object_set_data(G_OBJECT(item),
					  "streams_bitrate",
					  (gpointer) streams_bitrate);
			g_object_set_data(G_OBJECT(item),
					  "streams_samplerate",
					  (gpointer) streams_samplerate);
			g_object_set_data(G_OBJECT(item),
					  "streams_channels",
					  (gpointer) streams_channels);
			gtk_widget_show(item);
			g_free(label);

			/* selection */
			if (selected_streams_uri != NULL &&
			    !strcmp(selected_streams_uri, streams_uri))
				selection = i;
		} else {
			g_free(streams_uri);
			g_free(streams_mime);
			g_free(streams_codec);
			g_free(streams_bitrate);
			g_free(streams_samplerate);
		}
		i++;
		streamsinfo = streamsinfo->next;
	}

	drop_down = gtk_option_menu_new();
	gtk_widget_show(drop_down);
	gtk_option_menu_set_menu(GTK_OPTION_MENU(drop_down), menu);
	gtk_container_add(GTK_CONTAINER(align), drop_down);

	if (selection != -1)
		gtk_option_menu_set_history(GTK_OPTION_MENU(drop_down), selection);

	g_signal_connect(G_OBJECT(streams_selector), "response",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) streams_selector);
	g_signal_connect(G_OBJECT(streams_selector), "delete-event",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) streams_selector);

	return streams_selector;
}

#if 0
static void station_print_selection(GtkWidget * list, gpointer func_data)
{
	GList *girl_history;
	girl_history = GTK_LIST(list)->selection;

	if (!girl_history) {
		g_print(_("Selection cleared!\n"));
		return;
	}

	g_print("The selection is a ");

	while (girl_history) {

		GtkObject *list_item;
		gchar *item_data_string;

		list_item = girl_history->data;
		item_data_string =
		    g_object_get_data(G_OBJECT(list_item),
				      "station_list_item_data");
		g_print("%s ", item_data_string);

		girl_history = girl_history->next;
	}
	g_print("\n");
}
#endif

GtkWidget *create_girl_app()
{
	GtkWidget *girl_app;
	GtkWidget *vbox1;

	GtkWidget *girl_pixmap;

	GtkWidget *appbar;
	GtkWidget *progress;
	
	GirlData *girl_data = g_new0(GirlData, 1);
	char *pmf;

	girl_app = gnome_app_new("girl", _("GIRL"));
	gtk_window_set_title(GTK_WINDOW(girl_app),
			     _("GNOME Internet Radio Locator"));

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox1);
	gnome_app_set_contents(GNOME_APP(girl_app), vbox1);

	pmf = g_strconcat(GIRL_DATADIR, "/pixmaps/girl-map.png", NULL);

	girl_pixmap = gtk_image_new_from_file(pmf);
	g_free(pmf);

	if (girl_pixmap == NULL)
		g_error(_("Couldn't create pixmap"));

	gtk_widget_show(girl_pixmap);
	gtk_box_pack_start(GTK_BOX(vbox1), girl_pixmap, TRUE, TRUE, 0);

	progress = gtk_progress_bar_new();
	/* girl->progress = GTK_PROGRESS_BAR( progress ); */

	appbar = gnome_appbar_new(FALSE, TRUE, FALSE);
	gnome_app_set_statusbar(GNOME_APP(girl_app), GTK_WIDGET(appbar));
	gnome_app_create_toolbar(GNOME_APP(girl_app), toolbar);

	girl_data->pixmap = GTK_IMAGE(girl_pixmap);
	girl_data->appbar = GNOME_APPBAR(appbar);
	girl_data->progress = GTK_PROGRESS_BAR(progress);

#if HAVE_GIRL_RECORD == 1
	gnome_appbar_push(girl_data->appbar,_("Search by location from \"Search\" or select a radio station from \"Stations\".  Click \"Listen\" to listen to, or \"Record\" to record from the station."));
#else
	gnome_appbar_push(girl_data->appbar,_("Select a radio station from \"Stations\" and click \"Listen\" to listen to the station."));
#endif
	/*    g_signal_connect(G_OBJECT(calendar), */
	/*                     "day_selected_double_click", */
	/*                     G_CALLBACK (on_listen_button_clicked), girl_data); */
	g_signal_connect(G_OBJECT(girl_app), "destroy",
			 G_CALLBACK(quit_app), girl_data);

	girl = girl_data;

#if GIRL_CFG
	gnome_config_push_prefix("/girl/General/");

	girl->selected_listener_uri =
	    gnome_config_get_string("selected_listener_uri=");
	girl->selected_listener_name =
	    gnome_config_get_string("selected_listener_name=");
	girl->selected_listener_location =
	    gnome_config_get_string("selected_listener_location=");
	girl->selected_listener_band =
	    gnome_config_get_string("selected_listener_band=");
	girl->selected_listener_description =
	    gnome_config_get_string("selected_listener_description=");

	GIRL_DEBUG_MSG("girl->selected_listener_uri: %s\n",
	       girl->selected_listener_uri);
	GIRL_DEBUG_MSG("girl->selected_listener_name: %s\n",
	       girl->selected_listener_name);
	GIRL_DEBUG_MSG("girl->selected_listener_location: %s\n",
	       girl->selected_listener_location);
	GIRL_DEBUG_MSG("girl->selected_listener_band: %s\n",
	       girl->selected_listener_band);
	GIRL_DEBUG_MSG("girl->selected_listener_description: %s\n",
	       girl->selected_listener_description);

	girl->selected_station_uri =
	    gnome_config_get_string("selected_station_uri=");
	girl->selected_station_name =
	    gnome_config_get_string("selected_station_name=");
	girl->selected_station_location =
	    gnome_config_get_string("selected_station_location=");
	girl->selected_station_band =
	    gnome_config_get_string("selected_station_band=");
	girl->selected_station_description =
	    gnome_config_get_string("selected_station_description=");

	girl->selected_station_name =
	    gnome_config_get_string("selected_station_name=");
	girl->selected_station_location =
	    gnome_config_get_string("selected_station_location=");
	girl->selected_station_band =
	    gnome_config_get_string("selected_station_band=");
	girl->selected_station_description =
	    gnome_config_get_string("selected_station_description=");

	GIRL_DEBUG_MSG("girl->selected_station_uri: %s\n",
	       girl->selected_station_uri);
	GIRL_DEBUG_MSG("girl->selected_station_name: %s\n",
	       girl->selected_station_name);
	GIRL_DEBUG_MSG("girl->selected_station_location: %s\n",
	       girl->selected_station_location);
	GIRL_DEBUG_MSG("girl->selected_station_band: %s\n",
	       girl->selected_station_band);
	GIRL_DEBUG_MSG("girl->selected_station_description: %s\n",
	       girl->selected_station_description);

	if (strcmp(girl->selected_station_uri,"")==0) {
		girl->selected_station_uri = g_strdup("http://fm939.wnyc.org/wnycfm");
	}
	if (strcmp(girl->selected_station_name,"")==0) {
		girl->selected_station_name = g_strdup("WNYC");
	}
	if (strcmp(girl->selected_station_location,"")==0) {
		girl->selected_station_location = g_strdup("New York City, NY");
	}
	if (strcmp(girl->selected_station_band,"")==0) {
		girl->selected_station_band = g_strdup("ONLINE");
	}
	if (strcmp(girl->selected_station_description,"")==0) {
		girl->selected_station_description = g_strdup("WNYC 93.9 FM and AM 820 are New York's flagship public radio stations, broadcasting the finest programs from NPR, American Public Media, Public Radio International and the BBC World Service, as well as a wide range of award-winning local programming.");
	}

	girl->selected_streams_uri =
		gnome_config_get_string("selected_streams_uri=");
	girl->selected_streams_mime =
	    gnome_config_get_string("selected_streams_mime=");
	girl->selected_streams_codec =
	    gnome_config_get_string("selected_streams_codec=");
	girl->selected_streams_bitrate =
	    gnome_config_get_string("selected_streams_bitrate=");
	girl->selected_streams_samplerate =
	    gnome_config_get_string("selected_streams_samplerate=");
	girl->selected_streams_channels =
		(GirlChannels)gnome_config_get_string("selected_streams_channels=");

	GIRL_DEBUG_MSG("girl->selected_streams_uri: %s\n",
	       girl->selected_streams_uri);
	GIRL_DEBUG_MSG("girl->selected_streams_mime: %s\n",
	       girl->selected_streams_mime);
	GIRL_DEBUG_MSG("girl->selected_streams_codec: %s\n",
	       girl->selected_streams_codec);
	GIRL_DEBUG_MSG("girl->selected_streams_bitrate: %s\n",
	       girl->selected_streams_bitrate);
	GIRL_DEBUG_MSG("girl->selected_streams_samplerate: %s\n",
	       girl->selected_streams_samplerate);
	GIRL_DEBUG_MSG("girl->selected_channels: %0x\n",
	       girl->selected_streams_channels);

	gnome_config_pop_prefix();
#endif

	return girl_app;
}
