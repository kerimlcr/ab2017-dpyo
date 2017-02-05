/* $Id$
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

#ifndef GIRL_H
#define GIRL_H

typedef enum {
	GIRL_CHANNELS_MONO = 0x0001,
	GIRL_CHANNELS_STEREO = 0x0002,
	GIRL_CHANNELS_5_1 = 0x0005,
	GIRL_CHANNELS_NONE = 0xffff
} GirlChannels;

typedef enum {
	GIRL_STREAM_SHOUTCAST = 0x0001,
	GIRL_STREAM_OGG = 0x0002,
	GIRL_STREAM_AAC = 0x0003
} GirlStreamType;

typedef enum {
	GIRL_STREAM_PLAYER = 0x0001,
	GIRL_STREAM_RECORD = 0x0002
} GirlHelperType;

#include <gnome.h>
#include <libgnomevfs/gnome-vfs.h>
#include "girl-listener.h"
#include "girl-program.h"
#include "girl-runners.h"
#include "girl-streams.h"
#include "girl-station.h"
#include "girl-player.h"

#if GIRL_DEBUG == 0
#define GIRL_DEBUG_MSG(x...) g_message(x)
#else
#define GIRL_DEBUG_MSG(x...)
#endif

#define GIRL_RECORD_TRUE 1
#define GIRL_RECORD_FALSE 0
#define GIRL_PLAYER_TRUE 1
#define GIRL_PLAYER_FALSE 0

void show_error(gchar * msg);
void statusbar_send_msg(const char *a, ...);

/* GnomeVFSFileSize get_size(GnomeVFSURI *uri); */

/* GnomeVFSURI *xfer_channels(gchar *src, gchar *dest); */

gchar *copy_to_mem(GnomeVFSURI * uri, GnomeVFSFileSize len);

void appbar_send_msg(const char *a, ...);
gint girl_archive_new(gchar *title, gchar *file, gchar *codec);

/* Fetcher for the channels */

void girl_helper_run(char *url, char *name, GirlStreamType type, GirlHelperType girl);
void on_search_button_clicked(GtkWidget * button, gpointer user_data);
void on_listen_button_clicked(GtkWidget * button, gpointer user_data);
void on_record_button_clicked(GtkWidget * button, gpointer user_data);
void on_stop_button_clicked(GtkWidget *a, gpointer user_data);
void on_next_station_click(GtkWidget *, gpointer user_data);
void on_new_station_clicked(GtkWidget *a, gpointer user_data);
void on_new_station_selector_changed(GtkWidget *a, gpointer user_data);
void on_previous_station_click(GtkWidget *, gpointer user_data);
void on_listeners_selector_button_clicked(GtkWidget *, gpointer user_data);
void on_listeners_selector_changed(GtkWidget * a, gpointer user_data);
void on_programs_selector_button_clicked(GtkWidget *, gpointer user_data);
void on_programs_selector_changed(GtkWidget * a, gpointer user_data);
void on_stations_selector_button_clicked(GtkWidget *, gpointer user_data);
void on_stations_selector_changed(GtkWidget * a, gpointer user_data);
void on_streams_selector_button_clicked(GtkWidget *, gpointer user_data);
void on_streams_selector_changed(GtkWidget * a, gpointer user_data);
void quit_app(GtkWidget *, gpointer user_data);
void about_app(GtkWidget *, gpointer user_data);
void about_listener(GtkWidget *, gpointer user_data);
void about_program(GtkWidget *, gpointer user_data);

struct _GirlData {
	GtkImage *pixmap;
	GnomeAppBar *appbar;
	GtkProgressBar *progress;
	GtkAboutDialog *window;
	GtkStatusbar *statusbar;
	GtkWidget *about_station;
	GtkWidget *player_window;
	GtkWidget *record_window;
	gchar *selected_archive_file;
	GirlListenerInfo *selected_listener;
	gchar *selected_listener_uri;
	gchar *selected_listener_name;
	gchar *selected_listener_location;
	gchar *selected_listener_band;
	gchar *selected_listener_description;
	GirlProgramInfo *selected_program;
	gchar *selected_program_uri;
	gchar *selected_program_name;
	gchar *selected_program_location;
	gchar *selected_program_band;
	gchar *selected_program_description;
	GirlRunnersInfo *selected_runners;
	gint timeout_id;
	gint current_station_number;
	GirlStationInfo *previous_station;
	GirlStationInfo *selected_station;
	gchar *selected_station_uri;
	gchar *selected_station_name;
	gchar *selected_station_location;
	gchar *selected_station_band;
	gchar *selected_station_description;
	gchar *selected_station_website;
	gint selected_bitrate;
	GirlStreamsInfo *selected_streams;
	gchar *selected_streams_mime;
	gchar *selected_streams_uri;
	gchar *selected_streams_codec;
	gchar *selected_streams_samplerate;
	gchar *selected_streams_stations;
	gchar *selected_streams_bitrate;
	GirlChannels selected_streams_channels;
	gint selected_samplerate;
	GdkPixbuf *icon;
	gint player_status;
	GPid player_pid;
	GSubprocess *player_subprocess;
	GSubprocess *record_subprocess;
	GSubprocessLauncher *player_launcher;
	GSubprocessLauncher *record_launcher;
	gint record_status;
	GPid record_pid;
	gint station_count;
	gint stream_count;
	GMainLoop *player_loop;
	GMainLoop *record_loop;
};

typedef struct _GirlData GirlData;

void about_station(GirlData *, gpointer user_data);
void about_streams(GtkWidget *, gpointer user_data);

extern GirlData *girl;
extern GList *girl_listeners;
extern GList *girl_programs;
extern GList *girl_stations;
extern GList *girl_streams;

extern gchar *list_item_data_key;

static void  sigh_print_selection( GtkWidget *gtklist,
				   gpointer   func_data);

static void  sigh_button_event( GtkWidget      *gtklist,
				GdkEventButton *event,
				GtkWidget      *frame );

#endif /* GIRL_H */
