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
 * https://github.com/sreerenjb/gstplayer/blob/master/gst-backend.c
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

#include "girl-player-backend.h"
#include "girl-player-globals.h"
#include <gst/video/videooverlay.h>

struct GirlMedia *media;
extern GirlData *girl;

static gboolean handler_message (GstBus *bus, GstMessage *message , gpointer data)
{
  switch (GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_ERROR:
      {
         GError *err;
         gchar *debug;
         gst_message_parse_error(message,&err,&debug);
         g_print("\n Message:%s\n",err->message);
         g_error_free(err);
         g_free(debug);
         gst_message_unref (message);
         gst_object_unref (bus);
       }    
       break;

     case GST_MESSAGE_EOS:
       g_message("Got end of stream message..");
       gst_element_set_state (media->pipeline, GST_STATE_NULL);
       break;

     default:
      /* Unhandled message */
       break;
   }
   return TRUE;
}

void girl_player_backend_pause()
{
  gst_element_set_state(media->pipeline, GST_STATE_PAUSED);  
  GIRL_DEBUG_MSG("Paused....");

}

void girl_player_backend_play()
{
  gst_element_set_state(media->pipeline, GST_STATE_PLAYING);  
  GIRL_DEBUG_MSG("Playing....");
}

void girl_player_backend_seek()
{
}

GstBusSyncReply CreateWindow (GstBus *bus,GstMessage *message,gpointer data)
{
	if (GST_MESSAGE_TYPE (message) != GST_MESSAGE_ELEMENT)
		return GST_BUS_PASS;
	if ( !gst_structure_has_name (gst_message_get_structure (message), "prepare-xwindow-id")) 
		return GST_BUS_PASS;
	if (Window_Xid != 0) { 
		GstVideoOverlay *video_overlay;
		video_overlay = GST_VIDEO_OVERLAY (GST_MESSAGE_SRC (message));
		gst_video_overlay_set_window_handle (video_overlay, Window_Xid);
	} else {
		g_warning ("Should have obtained Window_Xid by now!");
	}
	gst_message_unref (message);
	return GST_BUS_DROP;   
}


gboolean girl_player_backend_start (gchar *uri, gchar *name)
{
	media = (struct GirlMedia*)malloc(sizeof(GirlMedia));
	media->uri = uri;
	media->pipeline = gst_element_factory_make ("playbin", "playbin");
	if (!media->pipeline)
	{
		g_message("Failed to create the pipeline element,playbin...!");
		return FALSE;
	}

  g_object_set(G_OBJECT(media->pipeline),"uri",media->uri,NULL);

  media->bus = gst_pipeline_get_bus(GST_PIPELINE(media->pipeline));
  gst_bus_set_sync_handler (media->bus, (GstBusSyncHandler)CreateWindow, NULL, NULL);
  gst_bus_add_watch(media->bus, handler_message, NULL);
  gst_object_unref (media->bus);

  girl_player_backend_play();
  return TRUE;
}

gboolean girl_player_backend_init (int *argc,char **argv[])
{
  media = (struct GirlMedia*)malloc(sizeof(GirlMedia));
  if (media != NULL)
    return TRUE;
  else 
    return FALSE;	
}

void girl_player_backend_stop (GMainLoop *loop)
{

	girl->player_status = GIRL_PLAYER_FALSE;
	gst_element_set_state(media->pipeline, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT (media->pipeline));
	gst_deinit();
	g_main_loop_quit (loop);
}
