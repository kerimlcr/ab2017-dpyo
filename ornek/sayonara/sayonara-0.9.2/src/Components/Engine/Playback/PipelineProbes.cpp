/* PipelineProbes.cpp */

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



#include "PipelineProbes.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Helper.h"


GstPadProbeReturn
Probing::level_probed(GstPad *pad, GstPadProbeInfo *info, gpointer user_data){
	Q_UNUSED(pad)
	Q_UNUSED(info)

	bool* b = static_cast<bool*>( user_data );
	if(*b){
		return GST_PAD_PROBE_REMOVE;
	}

	else{
		return GST_PAD_PROBE_DROP;
	}
}


GstPadProbeReturn
Probing::spectrum_probed(GstPad *pad, GstPadProbeInfo *info, gpointer user_data){
	Q_UNUSED(pad)
	Q_UNUSED(info)

	bool* b = static_cast<bool*>( user_data );
	if(*b){
		return GST_PAD_PROBE_REMOVE;
	}

	else{
		return GST_PAD_PROBE_DROP;
	}
}


GstPadProbeReturn
Probing::lame_probed(GstPad *pad, GstPadProbeInfo *info, gpointer user_data){
	Q_UNUSED(pad)
	Q_UNUSED(info)

	bool* b = static_cast<bool*>( user_data );
	if(*b){
		return GST_PAD_PROBE_REMOVE;
	}

	else{
		return GST_PAD_PROBE_DROP;
	}
}


void Probing::handle_probe(bool* active, GstElement* queue, gulong* probe_id, GstPadProbeCallback callback){

	GstPad* pad =  gst_element_get_static_pad(queue, "src");

	if(*active == true){
		if(*probe_id > 0){
			gst_pad_remove_probe(pad, *probe_id);
			*probe_id = 0;
		}
	}

	else if(*probe_id == 0){

		*probe_id = gst_pad_add_probe(
					pad,
					(GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER),
					callback,
					active, // userdata
					NULL
					);
	}

	if(pad != nullptr){
		gst_object_unref(pad);
	}
}




void Probing::handle_stream_recorder_probe(StreamRecorderData* data, GstPadProbeCallback callback){

	GstPad* pad =  gst_element_get_static_pad(data->queue, "src");

	if(data->probe_id == 0){
		data->busy = true;
		data->probe_id = gst_pad_add_probe(
					pad,
					(GstPadProbeType)(GST_PAD_PROBE_TYPE_BUFFER),
					callback,
					data, // userdata
					NULL
		);

		gst_element_send_event(data->sink, gst_event_new_eos());
	}

	if(pad != nullptr){
		gst_object_unref(pad);
	}
}


GstPadProbeReturn
Probing::stream_recorder_probed(GstPad *pad, GstPadProbeInfo *info, gpointer user_data){

	Q_UNUSED(pad)
	Q_UNUSED(info)

	StreamRecorderData* data = static_cast<StreamRecorderData*>(user_data);

	if(!data){
		return GST_PAD_PROBE_DROP;
	}

	if(data->active){

		sp_log(Log::Debug) << "set new filename streamrecorder: " << data->filename;

		gst_element_set_state(data->sink, GST_STATE_NULL);
		g_object_set( data->sink,
					  "location", data->filename,
					  nullptr);

		data->has_empty_filename = false;

		if(data->probe_id > 0){
			//gst_pad_remove_probe(pad, data->probe_id);
			data->probe_id = 0;
		}

		gst_element_set_state(data->sink, GST_STATE_PLAYING);

		data->busy = false;
		return GST_PAD_PROBE_REMOVE;
	}

	else{
		if(!data->has_empty_filename){
			gst_element_set_state(data->sink, GST_STATE_NULL);
			g_object_set( data->sink,
						  "location",
						  (Helper::get_sayonara_path() + "bla.mp3").toLocal8Bit().data(),
						  nullptr);

			data->has_empty_filename = true;
		}

		data->busy = false;
		return GST_PAD_PROBE_DROP;
	}
}
