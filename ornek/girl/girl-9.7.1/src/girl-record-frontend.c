/* $Id$
 *
 * GNOME Internet Radio Locator
 *
 * Copyright (C) 2016, 2017  Ole Aamot Software
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
 * https://github.com/sreerenjb/gstplayer/blob/master/gst-frontend.c
 *
 * Copyright (C) 2010 Sreerenj Balachandran.
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

#include <config.h>
#include "girl.h"
#include "girl-record-backend.h"
#include "girl-record-frontend.h"
#include "girl-record-globals.h"

extern GtkWidget *girl_app;
extern GirlData *girl;

GirlRecord *record;

static void video_widget_realize_cb (GtkWidget * widget, gpointer data)
{
	Window_Xid = GDK_WINDOW_XID (widget->window);
}

static gboolean 
delete_event (GtkWidget *window,
              GdkEvent *event,
              gpointer data)
{
	g_message ("delete event occurred");
	girl_record_backend_pause();
	return FALSE;
}

static void
girl_record_frontend_destroy (GtkWidget *widget, gpointer data)
{
	gtk_widget_destroy (widget);
}

static void
girl_record_frontend_play (GtkWidget *widget, gpointer data)
{
	girl_record_backend_pause();
	girl_record_backend_play();
}

static void
girl_record_frontend_pause (GtkWidget *widget, gpointer data)
{
	girl_record_backend_pause();
}

gboolean girl_record_frontend_start (gchar *name)
{
	record->table = gtk_table_new (300,60,FALSE);
	
	girl->record_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_window_set_position (GTK_WINDOW (girl->record_window),GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_title (GTK_WINDOW (girl->record_window),name);
	gtk_window_set_default_size (GTK_WINDOW (girl->record_window),300,60);
	gtk_container_set_border_width (GTK_CONTAINER (girl->record_window), 10);
	gtk_window_set_transient_for (GTK_WINDOW(girl->record_window),
				      GTK_WINDOW(girl_app));
	
	gtk_signal_connect (GTK_OBJECT (girl->record_window), "delete_event",
			    GTK_SIGNAL_FUNC (delete_event), NULL);
	
	g_signal_connect (G_OBJECT (girl->record_window), "destroy",
			  G_CALLBACK (girl_record_frontend_destroy), NULL);

	record->video_window = gtk_drawing_area_new ();
	gtk_widget_set_size_request (record->video_window, 100, 100);
	
	g_signal_connect (record->video_window, "realize",
			  G_CALLBACK (video_widget_realize_cb), NULL);
	
	gtk_widget_set_double_buffered (record->video_window, FALSE);
	
	record->play_button = gtk_button_new_with_label ("Play");
	record->pause_button = gtk_button_new_with_label ("Pause");
	
	
	gtk_container_add (GTK_CONTAINER (girl->record_window), record->table);
	gtk_table_attach_defaults (GTK_TABLE(record->table), record->video_window, 0, 700, 0, 500);
	gtk_table_attach_defaults (GTK_TABLE(record->table), record->play_button, 0, 15, 590, 600);
	gtk_table_attach_defaults (GTK_TABLE(record->table), record->pause_button, 15, 25, 590, 600);
	
	g_signal_connect (G_OBJECT (record->play_button), "clicked",
                          G_CALLBACK (girl_record_frontend_play), NULL);
	
	g_signal_connect (G_OBJECT (record->pause_button), "clicked",
                          G_CALLBACK (girl_record_frontend_pause), NULL);
	
	gtk_widget_show_all (girl->record_window);
	gtk_widget_realize (girl->record_window);
	
	g_assert (Window_Xid != 0);
	
	return TRUE;
}
gboolean girl_record_frontend_init (int *argc, gchar **argv[])
{
	record = (GirlRecord *)malloc(sizeof(GirlRecord));
	if (record != NULL)
		return TRUE;
	else
		return FALSE;
}

