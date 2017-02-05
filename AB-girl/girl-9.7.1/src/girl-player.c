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
 * https://github.com/sreerenjb/gstplayer/blob/master/gstplayer.c
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

#include "girl-player-frontend.h"
#include "girl-player-backend.h"
#include "girl-player-globals.h"

extern GirlData *girl;

void
girl_player_main(gchar *streamuri, gchar *name)
{
 
	gchar *uri = g_strdup(streamuri);
		
	/* if (argc > 1) { */
	/*   if (g_strrstr (argv[1],"rtsp://") || g_strrstr (argv[1],"http://") || g_strrstr (argv[1],"file://")) */
	/*     uri = g_strdup (argv[1]); */
	/*   else */
	/*     uri = g_strdup_printf ("file://%s", argv[1]); */
	/* } */
	/* else { */
	/*   g_message ("Specify the media file name..<girl-playerplayer filename>"); */
	/*   return 0; */
	/* } */
	
	if (!girl_player_frontend_start (name)) {  
		g_message ("Player UI creation failure...."); 
		exit (0);
	}
	if (!girl_player_backend_start (uri, name)) {  
		g_message ("Girl-Player backend creation failure");
		exit (0);
	}
	girl->player_loop = g_main_loop_new (NULL, FALSE);
	g_main_loop_run (girl->player_loop);
	girl_player_backend_stop (girl->player_loop);
	girl->player_status = GIRL_PLAYER_TRUE;
	exit (0);
}
