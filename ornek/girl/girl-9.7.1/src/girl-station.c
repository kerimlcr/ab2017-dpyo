/* $id$
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

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libgnome/gnome-exec.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-application-registry.h>

#include "girl.h"
#include "girl-station.h"
#include "girl-player.h"
#include "girl-record.h"

extern GirlData *girl;
extern GList *girl_stations;
extern GList *girl_listeners;
extern GList *girl_streams;

extern GtkWidget *girl_app;

void show_error(gchar * msg)
{
#ifdef GIRL_CLI

	printf("Error!!!\n");
#else
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(GTK_WINDOW(girl_app),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_CLOSE, msg, NULL);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
#endif				/* GIRL_CLI */
}

void
cb_subprocess_player (GSubprocess *subprocess)
{
	g_subprocess_force_exit(subprocess);
}

void
cb_subprocess_record (GSubprocess *subprocess)
{
	g_subprocess_force_exit(subprocess);
}

static void
cb_child_watch_player( GPid  pid,
		       gint  status)
{
	/* Remove timeout callback */

	/* FIXME? g_source_remove(girl->timeout_id ); */
	GIRL_DEBUG_MSG("Closing PID %i in %s", pid, __FUNCTION__);
	/* Close pid */
	g_spawn_close_pid( pid );
	g_spawn_check_exit_status(status, NULL);
}

static void
cb_child_watch_record( GPid  pid,
		       gint  status)
{
	/* Remove timeout callback */
	/* FIXME? g_source_remove(girl->timeout_id ); */
	GIRL_DEBUG_MSG("Closing PID %i in %s", pid, __FUNCTION__);
	/* Close pid */
	g_spawn_close_pid( pid );
	g_spawn_check_exit_status(status, NULL);
}

static gboolean
cb_out_watch( GIOChannel   *channel,
	      GIOCondition  cond)
{
	gchar *string;
	gsize  size;

	if( cond == G_IO_HUP )
	{
		g_io_channel_unref( channel );
		return( FALSE );
	}

	g_io_channel_read_line( channel, &string, &size, NULL, NULL );

	/* gnome_appbar_pop(girl->appbar); */
	/* gnome_appbar_push(girl->appbar, string); */

	g_free( string );

	return( TRUE );
}

static gboolean
cb_err_watch( GIOChannel   *channel,
	      GIOCondition  cond)
{
	gchar *string;
	gsize  size;

	if( cond == G_IO_HUP )
	{
		g_io_channel_unref( channel );
		return( FALSE );
	}

	g_io_channel_read_line( channel, &string, &size, NULL, NULL );
	/* gnome_appbar_pop(girl->appbar); */
	/* gnome_appbar_push(girl->appbar, string); */
	g_free( string );

	return( TRUE );
}

static gboolean
cb_timeout( )
{
	/* Bounce progress bar */
	gtk_progress_bar_pulse( girl->progress );

	return( TRUE );
}

void girl_helper_run(gchar *url, gchar *name, GirlStreamType type, GirlHelperType helper)
{
	GError *err = NULL;
	GTimeVal mtime;
	/* const char *mime_info = NULL; */
	/* GnomeVFSMimeApplication *app; */
	char *app = NULL, *command = NULL, *msg = NULL;
	char **argv = NULL;
	gint argc;
	gboolean    ret;
	GError *error = NULL;
	GCancellable *player_cancellable;
	GCancellable *record_cancellable;
	
	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		perror(0);
		exit(1);
	}
	
	g_return_if_fail(url != NULL);
	GIRL_DEBUG_MSG("%s", url);

	/* mime_info = gnome_vfs_get_mime_type(url); */

	g_get_current_time(&mtime);

	/* app = (gchar *)gnome_vfs_mime_get_default_application (mime_info); */

	// girl_player_main(url, girl->selected_station_name);

	if (helper == GIRL_STREAM_PLAYER) {
		girl_player_main(url, girl->selected_station_name);
	}

	if (helper == GIRL_STREAM_RECORD) {
		girl_record_main(url, girl->selected_station_name);
	}

	return;

#if 0
	if (g_strcmp0(app,"no")!=0) {
		command = g_strconcat(app, " ", url, NULL);
		GIRL_DEBUG_MSG("Helper application is %s\n", command);
		if (type == GIRL_STREAM_SHOUTCAST) {
			if (helper == GIRL_STREAM_PLAYER) {
				command = g_strconcat(app, " ", url, NULL);
				/* argv[0] = g_strdup(app); */
				/* argv[1] = g_strdup(url); */
			}
			if (helper == GIRL_STREAM_RECORD) {

				GtkWidget *dialog;
				char *filename, *default_filename;
				GDate gdate;
				time_t t;
				struct tm *tmp;
				char outstr[200];
				t = time(NULL);
				tmp = localtime(&t);
				strftime(outstr, sizeof(outstr), "%a, %d %b %Y %T %z", tmp);
				dialog = gtk_file_chooser_dialog_new ("Save Recorded Broadcast File",
								      GTK_WINDOW(girl_app),
								      GTK_FILE_CHOOSER_ACTION_SAVE,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
								      NULL);
				default_filename = g_strconcat(girl->selected_station_name, " - Broadcast Recording - ", outstr, ".mp3", NULL);  
				gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), default_filename);
				if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
				{
					filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
					girl->selected_archive_file = g_strconcat(filename, NULL);
				}
				gtk_widget_destroy (dialog);
				
				/* printf("Archiving program at %s\n", archive); */
				
				appbar_send_msg(_("Recording from %s in %s to %s"),
						girl->selected_station_name,
						girl->selected_station_location,
						girl->selected_archive_file);		
				
				command = g_strconcat(app, " ", url, " -d ", g_get_home_dir(), "/.girl/ -D %D", NULL);
				GIRL_DEBUG_MSG("%s\n", command);

				// girl_archive_new("Archive", girl->selected_archive_file);
				
				/* " -d ", g_get_home_dir(), "/.girl -D \"", name, "\" -s -a -u girl/", VERSION, NULL); */
				/* command = g_strconcat(command, " -d ", g_get_home_dir(), "/.girl/", name, " -D %S%A%T -t 10 -u girl/", VERSION, NULL); */
			}
		        GIRL_DEBUG_MSG("Helper command is %s\n", command);
		}
		/* gnome_vfs_mime_application_free (app); */
	} else {
		if (helper == GIRL_STREAM_PLAYER) {
			msg = g_strdup_printf(_("An error happened trying to play "
						"%s\nEither the file doesn't exist, or you "
						"don't have a player for it."),
					      url);
		}
		if (helper == GIRL_STREAM_RECORD) {
			msg = g_strdup_printf(_("An error happened trying to record "
						"%s\nEither the file doesn't exist, or you "
						"don't have a recorder for it."),
					      url);
		}
		if (msg != NULL) {
			show_error(msg);
			g_free(msg);
		}
		return;
	}

	if (helper == GIRL_STREAM_PLAYER) {
#if 0
		g_shell_parse_argv(command,
				   &argc,
				   &argv,
				   NULL);
		ret = g_spawn_async_with_pipes (".",
						argv,
						NULL,
						G_SPAWN_SEARCH_PATH|G_SPAWN_STDOUT_TO_DEV_NULL|G_SPAWN_STDERR_TO_DEV_NULL|G_SPAWN_DO_NOT_REAP_CHILD,
						NULL,
						NULL,
						&girl->player_pid,
						NULL,
						NULL,
						NULL,
						&err);
		if( ! ret )
		{
			msg = g_strdup_printf(_("Failed to run %s (%i)\n"), command, girl->player_pid);
			show_error(msg);
			g_free(msg);
			return;
		}
		/* Add watch function to catch termination of the process. This function
		 * will clean any remnants of process. */
		g_child_watch_add( girl->player_pid, (GChildWatchFunc)cb_child_watch_player, girl);
		
		/* girl->player_pid = pid; */
		girl->player_status = GIRL_PLAYER_TRUE;
		/* Install timeout fnction that will move the progress bar */
		girl->timeout_id = g_timeout_add(100,(GSourceFunc)cb_timeout,girl);
/* #endif */
/* #if 0 */
		ret = g_spawn_async_with_pipes( NULL, /* command */ argv, NULL,
						G_SPAWN_DO_NOT_REAP_CHILD|G_SPAWN_DEFAULT, NULL,
						NULL, &girl->player_pid, NULL, &out, &error, NULL );
		if( ! ret )
		{
			msg = g_strdup_printf(_("Failed to run %s (%i)\n"), command, girl->player_pid);
			show_error(msg);
			g_free(msg);
			return;
		}
		/* Add watch function to catch termination of the process. This function
		 * will clean any remnants of process. */
		g_child_watch_add( girl->player_pid, (GChildWatchFunc)cb_child_watch_player, girl );
		/* Create channels that will be used to read girl from pipes. *
			
#ifdef G_OS_WIN32
		out_ch = g_io_channel_win32_new_fd( out );
		err_ch = g_io_channel_win32_new_fd( error );
#else
		out_ch = g_io_channel_unix_new( out );
		err_ch = g_io_channel_unix_new( error );
#endif
		*/

                /* Add watches to channels */
		g_io_add_watch( out_ch, G_IO_IN | G_IO_HUP, (GIOFunc)cb_out_watch, girl );
		g_io_add_watch( err_ch, G_IO_IN | G_IO_HUP, (GIOFunc)cb_err_watch, girl );
		/* Install timeout fnction that will move the progress bar */
		girl->timeout_id = g_timeout_add( 100, (GSourceFunc)cb_timeout, girl );
/* #endif */
#if 0
		if (!g_spawn_command_line_sync(command, stdout, stderr, status, &err)) {
			msg = g_strdup_printf(_("Failed to open URL: '%s'\n"
						"Status code: %i\n"
						"Details: %s"), url, status, err->message);
			show_error(msg);
			g_error_free(err);
			g_free(msg);
		} else {
			GIRL_DEBUG_MSG("Launching %s\n", command);
		}
		/* Add watch function to catch termination of the process. This function
		 * will clean any remnants of process. */
		g_child_watch_add( girl->player_pid, (GChildWatchFunc)cb_child_watch_player, girl );
#endif
		/* Original async player code */

		girl->player_launcher = g_subprocess_launcher_new (G_SUBPROCESS_FLAGS_NONE);
		girl->player_subprocess = g_subprocess_launcher_spawn (girl->player_launcher, &error, app, url, NULL);
		if (girl->player_subprocess == NULL) {
			msg = g_strdup_printf(_("Failed to open URL: '%s'\n"
						"Details: %s"), url, error->message);
			show_error(msg);
			g_error_free(error);
			g_free(msg);
			goto quit_player;
		} else {
			girl->player_status = GIRL_PLAYER_TRUE;
			GIRL_DEBUG_MSG("Launching %s player\n", command);
		}

#endif
#if 0
		if (!g_spawn_command_line_async(command, &err)) {
			msg = g_strdup_printf(_("Failed to open URL: '%s'\n"
						"Details: %s"), url, err->message);
			show_error(msg);
			g_error_free(err);
			g_free(msg);
		} else {
			girl->player_status = GIRL_PLAYER_TRUE;
			GIRL_DEBUG_MSG("Launching %s player\n", command);
			g_subprocess_wait_async (girl->player_subprocess,
						 player_cancellable,
						 (GAsyncReadyCallback)cb_subprocess_player,
						 NULL);
		}
#endif

	}
	
	if (helper == GIRL_STREAM_RECORD) {
		
/* #if 0 */
		/* gchar *argv[] = { command, NULL }; */
		/* Spawn child process */
		gchar *recording_path_name = g_strconcat(g_get_home_dir(), "/.girl/", NULL);
		gchar *formatting_argument = g_strdup("-D %D");
		girl->record_launcher = g_subprocess_launcher_new (G_SUBPROCESS_FLAGS_NONE);
		/* girl->record_subprocess = g_subprocess_launcher_spawn (girl->record_launcher, &error, GIRL_HELPER_RECORD, url, "-d", recording_path_name, "-D", "%D", NULL); */
		if (girl->record_subprocess == NULL) {
			/* msg = g_strdup_printf(_("Failed to run '%s'\n" */
			/* 			"Details: %s"), GIRL_HELPER_RECORD, error->message); */
			show_error(msg);
			g_error_free(error);
			g_free(msg);
			goto quit_record;
		} else {
			GIRL_DEBUG_MSG("Launching %s\n", command);
			g_subprocess_wait_async (girl->record_subprocess,
						 record_cancellable,
						 (GAsyncReadyCallback)cb_subprocess_record, NULL);
		}

#if 0
		g_shell_parse_argv(command,
				   &argc,
				   &argv,
				   NULL);
		ret = g_spawn_async_with_pipes (".",
						argv,
						NULL,
						G_SPAWN_SEARCH_PATH|G_SPAWN_STDOUT_TO_DEV_NULL|G_SPAWN_STDERR_TO_DEV_NULL|G_SPAWN_DO_NOT_REAP_CHILD,
						NULL,
						NULL,
						&girl->record_pid,
						NULL,
						NULL,
						NULL,
						&err);
		if( ! ret )
		{
			msg = g_strdup_printf(_("Failed to run %s (%i)\n"), command, girl->record_pid);
			show_error(msg);
			g_free(msg);
			return;
		}
		/* Add watch function to catch termination of the process. This function
		 * will clean any remnants of process. */
		g_child_watch_add( girl->record_pid, (GChildWatchFunc)cb_child_watch_record, girl);

		/* girl->record_pid = pid; */
		girl->record_status = GIRL_RECORD_TRUE;

		girl_archive_new("Archive", girl->selected_archive_file, girl->selected_streams_codec);
				
		/* Install timeout fnction that will move the progress bar */
		girl->timeout_id = g_timeout_add(100,(GSourceFunc)cb_timeout,girl);
/* #endif */
#if 0 
		ret = g_spawn_async_with_pipes( NULL, /* command */ argv, NULL,
						G_SPAWN_DO_NOT_REAP_CHILD|G_SPAWN_DEFAULT, NULL,
						NULL, &girl->record_pid, NULL, &out, &error, NULL );
		if( ! ret )
		{
			msg = g_strdup_printf(_("Failed to run %s (%i)\n"), command, girl->record_pid);
			show_error(msg);
			g_free(msg);
			return;
		}
		/* Add watch function to catch termination of the process. This function
		 * will clean any remnants of process. */
		g_child_watch_add( girl->record_pid, (GChildWatchFunc)cb_child_watch_record, girl );
		/* Create channels that will be used to read girl from pipes. */
#ifdef G_OS_WIN32
		out_ch = g_io_channel_win32_new_fd( out );
		err_ch = g_io_channel_win32_new_fd( error );
#else
		out_ch = g_io_channel_unix_new( out );
		err_ch = g_io_channel_unix_new( error );
#endif
		/* Add watches to channels */
		g_io_add_watch( out_ch, G_IO_IN | G_IO_HUP, (GIOFunc)cb_out_watch, girl );
		g_io_add_watch( err_ch, G_IO_IN | G_IO_HUP, (GIOFunc)cb_err_watch, girl );
		/* Install timeout fnction that will move the progress bar */
		girl->timeout_id = g_timeout_add( 100, (GSourceFunc)cb_timeout, girl );
#endif
#if 0
		if (!g_spawn_command_line_sync(command, stdout, stderr, status, &err)) {
			msg = g_strdup_printf(_("Failed to open URL: '%s'\n"
						"Status code: %i\n"
						"Details: %s"), url, status, err->message);
			show_error(msg);
			g_error_free(err);
			g_free(msg);
		} else {
			GIRL_DEBUG_MSG("Launching %s\n", command);
		}
		/* Add watch function to catch termination of the process. This function
		 * will clean any remnants of process. */
		g_child_watch_add( girl->record_pid, (GChildWatchFunc)cb_child_watch_record, girl );
#endif
#endif
	}

quit_player:

        if (girl->player_subprocess)
		g_object_unref(girl->player_subprocess);
        if (girl->player_launcher)
		g_object_unref(girl->player_launcher);
quit_record:
        if (girl->record_subprocess)
		g_object_unref(girl->record_subprocess);
        if (girl->record_launcher)
		g_object_unref(girl->record_launcher);
#endif
}

void girl_stream_player(GtkWidget * widget, gpointer data)
{
	girl_helper_run(girl->selected_station_uri,
			girl->selected_station_name,
			GIRL_STREAM_SHOUTCAST,
		        GIRL_STREAM_PLAYER);
}

void girl_stream_record(GtkWidget * widget, gpointer data)
{
	girl_helper_run(girl->selected_station_uri,
			girl->selected_station_name,
			GIRL_STREAM_SHOUTCAST,
		        GIRL_STREAM_RECORD);
}

void girl_stream_select(GirlStreamsInfo * info)
{
}

static void
girl_station_parser(GirlStationInfo * station, xmlDocPtr doc,
		    xmlNodePtr cur)
{
	xmlNodePtr sub;
	char *chans;

	g_return_if_fail(station != NULL);
	g_return_if_fail(doc != NULL);
	g_return_if_fail(cur != NULL);

	station->id = (gchar *)xmlGetProp(cur, (const xmlChar *)"id");
	GIRL_DEBUG_MSG("station->id = %s\n", station->id);
	station->name = (gchar *)xmlGetProp(cur, (const xmlChar *)"name");
	GIRL_DEBUG_MSG("station->name = %s\n", station->name);
	station->rank = (gchar *)xmlGetProp(cur, (const xmlChar *)"rank");
	GIRL_DEBUG_MSG("station->rank = %s\n", station->rank);
	station->type = (gchar *)xmlGetProp(cur, (const xmlChar *)"type");
	GIRL_DEBUG_MSG("station->type = %s\n", station->type);
	station->band = (gchar *)xmlGetProp(cur, (const xmlChar *)"band");
	GIRL_DEBUG_MSG("station->band = %s\n", station->band);

	sub = cur->xmlChildrenNode;

	while (sub != NULL) {

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "frequency"))) {
			station->frequency = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("station->frequency = %s\n",
			    station->frequency);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "location"))) {
			station->location = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("station->location = %s\n", station->location);

		}

		if ((!xmlStrcmp
		     (sub->name, (const xmlChar *) "description"))) {
			station->description = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("station->description = %s\n", station->description);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "uri"))) {
			station->uri = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("station->uri = %s\n", station->uri);
			/* fprintf(stdout, "%s (%s)\n%s\n\n", station->name, station->location, station->uri); */
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "stream"))) {

			GirlStreamInfo *stream = g_new0(GirlStreamInfo, 1);

			girl->stream_count++;

			station->stream = stream;

			station->stream->mimetype = (gchar *)
				xmlGetProp(sub, (const xmlChar *)"mime");
			GIRL_DEBUG_MSG("station->stream->mimetype = %s\n",
			    station->stream->mimetype);
			if (xmlGetProp(sub, (const xmlChar *)"bitrate") != NULL) {
				station->stream->bitrate =
					(glong)atol((char *)xmlGetProp(sub, (const xmlChar *)"bitrate"));
				GIRL_DEBUG_MSG("station->stream->bitrate = %li\n",
				    station->stream->bitrate);
			}

			if (xmlGetProp(sub, (const xmlChar *)"samplerate") != NULL) {
				station->stream->samplerate = (glong) 
					atol((char *)xmlGetProp(sub, (const xmlChar *)"samplerate"));
			}

			GIRL_DEBUG_MSG("station->stream->samplerate = %li\n",
			    station->stream->samplerate);
			station->stream->uri = (gchar *)xmlGetProp(sub, (const xmlChar *)"uri");
			GIRL_DEBUG_MSG("station->stream->uri = %s\n",
			    station->stream->uri);

			/* fprintf(stdout, "%s (%s)\n%s\n\n", station->name, station->location, station->stream->uri); */
			
			chans = (gchar *)xmlGetProp(sub, (const xmlChar *)"stations");

			if (chans != NULL) {
				if (strcmp(chans, "stereo") == 0) {
					station->stream->channels =
					    GIRL_CHANNELS_STEREO;
					GIRL_DEBUG_MSG("station->stream->channels = %d\n", station->stream->channels);
				} else if (strcmp(chans, "mono") == 0) {
					station->stream->channels =
					    GIRL_CHANNELS_MONO;
					GIRL_DEBUG_MSG("station->stream->channels = %d\n", station->stream->channels);
				} else if (strcmp(chans, "5:1") == 0) {
					station->stream->channels =
					    GIRL_CHANNELS_5_1;
					GIRL_DEBUG_MSG("station->stream->channels = %d\n", station->stream->channels);
				}
				g_free(chans);
			}

		}
		/* if... "stream" */
		sub = sub->next;
	}

	girl->station_count++;
	return;
}

GirlStationInfo *girl_station_load_from_http(GirlStationInfo * head,
					     gpointer data)
{
	GirlStationInfo *gstation;
	gstation = girl_station_load_from_file (head, "http://girl.src.oka.no/girl.xml");
	return gstation;
}

GirlStationInfo *girl_station_load_from_file(GirlStationInfo * head,
					     char *filename)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	GirlStationInfo *curr = NULL;
	char *version;
	GirlStationInfo *mem_station;

	g_return_val_if_fail(filename != NULL, NULL);

	doc = xmlReadFile(filename, NULL, 0);

	if (doc == NULL) {
		perror("xmlParseFile");
		xmlFreeDoc(doc);
		return NULL;
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		fprintf(stderr, "Empty document\n");
		xmlFreeDoc(doc);
		return NULL;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "girl")) {
		fprintf(stderr,
			"Document of wrong type, root node != girl\n");
		xmlFreeDoc(doc);
		return NULL;
	}

	version = (gchar *)xmlGetProp(cur, (const xmlChar *)"version");

	GIRL_DEBUG_MSG("Valid Girl %s XML document... Parsing stations...\n",
	    version);

	free(version);

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {

		if ((!xmlStrcmp(cur->name, (const xmlChar *) "station"))) {

			GIRL_DEBUG_MSG("Found a new station.\n");

			curr = g_new0(GirlStationInfo, 1);
			mem_station = g_new0(GirlStationInfo, 1);

			girl_station_parser(curr, doc, cur);

			curr->next = head;

			head = curr;

			mem_station = head;

			girl_stations = g_list_append(girl_stations, (GirlStationInfo *)mem_station);

			GIRL_DEBUG_MSG("Done with parsing the station.\n");

		}
		cur = cur->next;
	}

	GIRL_DEBUG_MSG("Finished parsing XML document.\n");

	xmlFreeDoc(doc);

	return curr;
}

gint girl_station_update (GirlStationInfo *head, gchar *station_band, gchar *station_description, gchar *station_name, gchar *station_location, gchar *station_uri, gchar *station_website) {
	gchar msg[256];
	gchar *msg1, *msg2;
	/* Open ~/.girl/girl.xml.  Parse structure.  Insert new item.  Save structure. */
	GirlStationInfo *new_station;
	GirlStationInfo *stationinfo;
	/* GList *girl_local_stations = NULL; */
	gchar *local_station_uri, *local_station_name, *local_station_location, *local_station_band, *local_station_description, *local_station_website;
	gchar *stations = g_strconcat(g_get_home_dir(), "/.girl/girl.xml", NULL);

	gboolean local_girl_file = g_file_test (".girl/girl.xml", G_FILE_TEST_EXISTS);

	GIRL_DEBUG_MSG("local_girl_file = %i\n", local_girl_file);

	if (local_girl_file == 0) {

		gchar *local_girl_directory = g_strconcat(g_get_home_dir(), "/.girl", NULL);
		g_mkdir_with_parents (local_girl_directory, 0700);
		
	}
	
	FILE *fp;
	
	stationinfo = girl_station_load_from_file(NULL, stations);

	new_station = g_new0(GirlStationInfo, 1);
	new_station->name = g_strdup(station_name);
	new_station->band = g_strdup(station_band);
	new_station->description = g_strdup(station_description);
	new_station->location = g_strdup(station_location);
	new_station->stream = g_new0(GirlStreamInfo, 1);
	new_station->stream->uri = g_strdup(station_uri);
	new_station->uri = g_strdup(station_website);
	fp = g_fopen(stations, "w+");
	g_fprintf(fp, "<?xml version='1.0' encoding='UTF-8'?>\n<!DOCTYPE girl SYSTEM 'girl-9.1.dtd'>\n<girl version='9.1'>\n");
	// stationinfo-> = l->data;
	while (stationinfo != NULL) {
		local_station_uri = g_strdup(stationinfo->stream->uri);
		local_station_name = g_strdup(stationinfo->name);
		local_station_location = g_strdup(stationinfo->location);
		local_station_band = g_strdup(stationinfo->band);
		local_station_description = g_strdup(stationinfo->description);
		local_station_website = g_strdup(stationinfo->uri);
		/* FIXME: Save mime='audio/mp3' uri='%s' codec='MPEG 1 Audio, Layer 3 (MP3)' samplerate='24000 Hz' channels='Mono' bitrate='32 kbps' */
		g_fprintf(fp, "  <station band=\"%s\" id=\"%s\" lang=\"en\" name=\"%s\" rank=\"1.0\" type=\"org\">\n    <frequency uri=\"%s\">%s in %s</frequency>\n    <location>%s</location>\n    <description lang=\"en\">%s</description>\n    <stream uri=\"%s\" />\n    <uri>%s</uri>\n  </station>\n", local_station_band, local_station_name, local_station_name, local_station_website, local_station_band, local_station_location, local_station_location, local_station_description, local_station_uri, local_station_website);
		stationinfo = stationinfo->next;

	}
	g_fprintf(fp, "  <station band=\"%s\" id=\"%s\" lang=\"en\" name=\"%s\" rank=\"1.0\" type=\"org\">\n    <frequency uri=\"%s\">%s in %s</frequency>\n    <location>%s</location>\n    <description lang=\"en\">%s</description>\n    <stream uri=\"%s\" />\n    <uri>%s</uri>\n  </station>\n", new_station->band, new_station->name, new_station->name, new_station->uri, new_station->band, new_station->location, new_station->location, new_station->description, new_station->stream->uri, new_station->uri);
	g_fprintf(fp, "</girl>\n");
	fclose(fp);
	girl_stations = g_list_append(girl_stations, (GirlStationInfo *)new_station);
	g_free(stations);
	g_free(new_station);
	g_free(stationinfo);

	return (0);
}
