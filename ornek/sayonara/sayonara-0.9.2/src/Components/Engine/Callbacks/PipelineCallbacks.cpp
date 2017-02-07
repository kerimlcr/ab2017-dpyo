/* PipelineCallbacks.cpp */

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



#include "PipelineCallbacks.h"
#include "Components/Engine/AbstractPipeline.h"

#include <gst/app/gstappsink.h>


gboolean PipelineCallbacks::position_changed(gpointer data) {

	GstState state;
	AbstractPipeline* pipeline;

	pipeline = static_cast<AbstractPipeline*>(data);
	if(!pipeline){
		return false;
	}

	state = pipeline->get_state();

	if( state != GST_STATE_PLAYING &&
		state != GST_STATE_PAUSED &&
		state != GST_STATE_READY)
	{
		return true;
	}

	pipeline->refresh_position();
	pipeline->check_about_to_finish();

	return true;
}

// dynamic linking, important for decodebin
void PipelineCallbacks::pad_added_handler(GstElement* src, GstPad* new_src_pad, gpointer data) {
	Q_UNUSED(src)

	GstElement*			element;
	GstPad*				sink_pad;
	GstPadLinkReturn	pad_link_return;

	element = static_cast<GstElement*>(data);
	if(!element){
		return;
	}

	sink_pad = gst_element_get_static_pad(element, "sink");
	if(!sink_pad){
		return;
	}

	if(gst_pad_is_linked(sink_pad)) {
		return;
	}

	pad_link_return = gst_pad_link(new_src_pad, sink_pad);

	if(pad_link_return != GST_PAD_LINK_OK) {

		sp_log(Log::Error) << "Dynamic pad linking: Cannot link pads";

		switch(pad_link_return){
			case GST_PAD_LINK_WRONG_HIERARCHY:
				sp_log(Log::Error) << "Cause: Wrong hierarchy";
				break;
			case GST_PAD_LINK_WAS_LINKED:
				sp_log(Log::Error) << "Cause: Pad was already linked";
				break;
			case GST_PAD_LINK_WRONG_DIRECTION:
				sp_log(Log::Error) << "Cause: Pads have wrong direction";
				break;
			case GST_PAD_LINK_NOFORMAT:
				sp_log(Log::Error) << "Cause: Pads have incompatible format";
				break;
			case GST_PAD_LINK_NOSCHED:
				sp_log(Log::Error) << "Cause: Pads cannot cooperate scheduling";
				break;
			case GST_PAD_LINK_REFUSED:
			default:
				sp_log(Log::Error) << "Cause: Refused because of different reason";
				break;
		}
	}
}


#define TCP_BUFFER_SIZE 16384
GstFlowReturn PipelineCallbacks::new_buffer(GstElement *sink, gpointer p){

	static uchar data[TCP_BUFFER_SIZE];

	AbstractPipeline* pipeline;
	GstSample* sample;
	GstBuffer* buffer;
	gsize size = 0;
	gsize size_new = 0;

	pipeline = static_cast<AbstractPipeline*>(p);
	if(!pipeline){
		return GST_FLOW_OK;
	}

	sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
	if(!sample) {
		return GST_FLOW_OK;
	}

	buffer = gst_sample_get_buffer(sample);
	if(!buffer) {
		return GST_FLOW_OK;
	}

	size = gst_buffer_get_size(buffer);
	size_new = gst_buffer_extract(buffer, 0, data, size);
	pipeline->set_data(data, size_new);

	//gst_buffer_unref(buffer);
	gst_sample_unref(sample);

	return GST_FLOW_OK;
}
