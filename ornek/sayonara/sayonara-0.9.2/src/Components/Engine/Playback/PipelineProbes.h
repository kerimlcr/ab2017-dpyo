/* PipelineProbes.h */

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



#ifndef PIPELINEPROBES_H
#define PIPELINEPROBES_H

#include <gst/gst.h>
#include <QString>


struct StreamRecorderData
{
	GstElement* queue=nullptr;
	GstElement* sink=nullptr;
	gchar* filename=nullptr;

	bool active;
	int probe_id;
	bool busy;
	bool has_empty_filename;

	StreamRecorderData(){
		probe_id=0;
		busy = false;
		active = false;
		has_empty_filename = true;
	}

	~StreamRecorderData(){
		if(filename){
			free(filename);
			filename = nullptr;
		}
	}
};


namespace Probing {


	GstPadProbeReturn
	level_probed(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);

	GstPadProbeReturn
	spectrum_probed(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);

	GstPadProbeReturn
	lame_probed(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);

	void handle_probe(bool* active, GstElement* queue, gulong* probe_id, GstPadProbeCallback callback);


	GstPadProbeReturn
	stream_recorder_probed(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);
	void handle_stream_recorder_probe(StreamRecorderData* data, GstPadProbeCallback callback);

}

#endif // PIPELINEPROBES_H
