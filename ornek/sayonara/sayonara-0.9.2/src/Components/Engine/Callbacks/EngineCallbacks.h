/* EngineCallbacks.h */

/* Copyright (C) 2011-2016  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#ifndef ENGINECALLBACKS_H
#define ENGINECALLBACKS_H

#include "Helper/Logger/Logger.h"
#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsink.h>
#include <gst/gstbuffer.h>
#include <gst/gstelement.h>

namespace EngineCallbacks {

#ifdef Q_OS_WIN
	void destroy_notify(gpointer data);

	GstBusSyncReply
	bus_message_received(GstBus* bus, GstMessage* msg, gpointer data);
#endif
	gboolean
	bus_state_changed(GstBus* bus, GstMessage* msg, gpointer data);

	gboolean
	level_handler(GstBus* bus, GstMessage* message, gpointer data);

	gboolean
	spectrum_handler(GstBus* bus, GstMessage* message, gpointer data);

	gboolean
	show_position(GstElement* pipeline);
}

#endif // ENGINECALLBACKS_H

