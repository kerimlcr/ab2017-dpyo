/* $Id$
 *
 * GNOME Internet Radio Locator
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

#include "girl.h"
#include "girl-runners.h"

GirlRunnersInfo *girl_runners_new (pid_t pid,
				   gchar *name,
				   gchar *date,
				   gchar *time,
				   gchar *file) {
	GirlRunnersInfo *runner = g_new0(GirlRunnersInfo, 1);
	runner->pid = pid;
	runner->name = name;
	runner->date = date;
	runner->time = time;
	runner->file = file;
	return (GirlRunnersInfo *)runner;
}

void girl_runners_free(GirlRunnersInfo * info) {
}

void girl_runners_mutex_get(GirlRunnersInfo *info) {
}

void girl_runners_mutex_release(GirlRunnersInfo *info) {
}

void girl_runners_mutex_lock(GirlRunnersInfo *info);
void girl_runners_mutex_unlock(GirlRunnersInfo *info);

void girl_runners_run(GirlRunnersInfo *info)
{
	/* GThread *thread; */
	/* gboolean thread_ready; */
	/* gint recording_lock; */
	/* g_assert (g_mutex_trylock (girl_mutex)); */
	/* g_assert (G_TRYLOCK (mutex)); */
	/* thread_ready = GIRL_RECORDING_FALSE; */
	/* thread = g_thread_create (thread, girl_helper_run, command, NULL); */
	/* while (!thread_ready) { */
	/* 	g_usleep(G_USEC_PER_SEC / 5); */
	/* } */
	/* G_UNLOCK (info->mutex); */
	/* g_mutex_unlock (mutex); */
	/* g_mutex_free (mutex); */
	girl_runners_mutex_get(info);
	/* recording_lock = GIRL_RECORDING_TRUE; */
	girl_runners_mutex_release(info);
	return;
}
