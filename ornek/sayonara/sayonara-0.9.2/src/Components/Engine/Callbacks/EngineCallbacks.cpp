/* EngineCallbacks.cpp */

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


#include "EngineCallbacks.h"
#include "Components/Engine/Playback/PlaybackEngine.h"

#include "Helper/globals.h"
#include "Helper/FileHelper.h"

#include <algorithm>
#include <QList>
#include <QImage>
#include <QRegExp>

#ifdef Q_OS_WIN
	void EngineCallbacks::destroy_notify(gpointer data){}

	GstBusSyncReply
	EngineCallbacks::bus_message_received(GstBus* bus, GstMessage* msg, gpointer data) {

		if(bus_state_changed(bus, msg, data)){
			gst_message_unref(msg);
			return GST_BUS_DROP;
		}

		gst_message_unref(msg);
		return GST_BUS_DROP;
	}
#endif

bool parse_image(GstTagList* tags, QImage& img)
{
	GstSample* sample;

	bool success = gst_tag_list_get_sample(tags, GST_TAG_IMAGE, &sample);
	if(!success){
		success = gst_tag_list_get_sample(tags, GST_TAG_PREVIEW_IMAGE, &sample);
		if(!success){
			return false;
		}
	}

	GstCaps* caps = gst_sample_get_caps(sample);
	if(!caps){
		return false;
	}

	gchar* mimetype = gst_caps_to_string(caps);
	if(mimetype == nullptr){
		return false;
	}

	QString mime(mimetype);
	g_free(mimetype); mimetype = nullptr;

	QRegExp re(".*(image/[a-z|A-Z]+).*");
	if(re.indexIn(mime) >= 0){
		mime = re.cap(1);
	}

	GstBuffer* buffer = gst_sample_get_buffer( sample );
	if(!buffer){
		gst_sample_unref(sample);
		return false;
	}

	gsize size = gst_buffer_get_size(buffer);
	if(size == 0){
		gst_sample_unref(sample);
		return false;
	}

	gchar* data = new gchar[size];
	size = gst_buffer_extract(buffer, 0, data, size);

	if(size == 0){
		delete[] data;

		gst_sample_unref(sample);
	
		return false;
	}

	img = QImage::fromData((const uchar*) data, size, mime.toLocal8Bit().data());
	
	delete[] data;
	gst_sample_unref(sample);

	return (!img.isNull());
}


// check messages from bus
gboolean EngineCallbacks::bus_state_changed(GstBus* bus, GstMessage* msg, gpointer data) {

	Q_UNUSED(bus);

	Engine*			engine;
	GError*			err;
	GstElement*		src;

	GstMessageType	msg_type;
	quint32			bitrate;

	QString			msg_src_name;
	QImage 			img;

	engine = static_cast<Engine*>(data);
	if(!engine){
		return true;
	}

	msg_type = GST_MESSAGE_TYPE(msg);
	msg_src_name = QString(GST_MESSAGE_SRC_NAME(msg)).toLower();
	src = reinterpret_cast<GstElement*>(msg->src);

	switch (msg_type) {

		case GST_MESSAGE_EOS:

			if (!engine) {
				break;
			}

			if(  !msg_src_name.contains("sr_filesink") &&
				 !msg_src_name.contains("level_sink") &&
				 !msg_src_name.contains("spectrum_sink") &&
				 !msg_src_name.contains("pipeline"))
			{
				sp_log(Log::Debug) << "EOF reached: " << msg_src_name;
				break;
			}

			engine->set_track_finished(src);

			break;

		case GST_MESSAGE_ELEMENT:

			if(!engine) {
				break;
			}

			if(msg_src_name.compare("spectrum") == 0){
				return spectrum_handler(bus, msg, engine);
			}

			if(msg_src_name.compare("level") == 0){
				return level_handler(bus, msg, engine);
			}

			break;

		case GST_MESSAGE_SEGMENT_DONE:
			sp_log(Log::Debug) << "Segment done: " << msg_src_name;
			break;

		case GST_MESSAGE_TAG:

			GstTagList*		tags;
			gchar*			title;
			bool			success;

			if( msg_src_name.compare("sr_filesink") == 0 ||
				msg_src_name.compare("level_sink") == 0 ||
				msg_src_name.compare("spectrum_sink") == 0)
			{
				break;
			}

			tags = nullptr;
			gst_message_parse_tag(msg, &tags);

			if(!tags){
				break;
			}

			success = parse_image(tags, img);
			if(success){
				engine->update_cover(img, src);
			}
			
			success = gst_tag_list_get_uint(tags, GST_TAG_BITRATE, &bitrate);
			if(success){
				engine->update_bitrate((bitrate / 1000) * 1000, src);
			}

			success = gst_tag_list_get_string(tags, GST_TAG_TITLE, (gchar**) &title);
			if(success){
				MetaData md;
				md.title = title;
				g_free(title);
				engine->update_md(md, src);
			}

			gst_tag_list_unref(tags);

			break;

		case GST_MESSAGE_STATE_CHANGED:
			GstState old_state, new_state, pending_state;

			gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
			/*sp_log(Log::Debug) << GST_MESSAGE_SRC_NAME(msg) << ": "
							   << "State changed from "
							   << gst_element_state_get_name(old_state)
							   << " to "
							   << gst_element_state_get_name(new_state)
							   << " pending: "
							   << gst_element_state_get_name(pending_state);*/

			if(!msg_src_name.contains("pipeline", Qt::CaseInsensitive)){
				break;
			}


			if( new_state == GST_STATE_PLAYING ||
				new_state == GST_STATE_PAUSED)
			{
				engine->set_track_ready(src);
			}

			break;

		case GST_MESSAGE_BUFFERING:

			gint percent;
			
			gint avg_in, avg_out;
			gint64 buffering_left;

			GstBufferingMode mode;
			gst_message_parse_buffering(msg, &percent);
			gst_message_parse_buffering_stats(msg, &mode, &avg_in, &avg_out, &buffering_left );

/*			sp_log(Log::Debug) << "Buffering " << percent;
			sp_log(Log::Debug) << "Buffering State: " << (int) mode << " avg in: " << avg_in << " avg out: " << avg_out << " todo: " << buffering_left;*/

			engine->set_buffer_state(percent, src);
			break;

		case GST_MESSAGE_DURATION_CHANGED:
			engine->update_duration(src);
			break;

		case GST_MESSAGE_INFO:
			/*gst_message_parse_info(msg, &err, nullptr);*/
			break;

		case GST_MESSAGE_WARNING:

			gst_message_parse_warning(msg, &err, nullptr);
			sp_log(Log::Warning) << "Engine " << (int) engine->get_name() << ": GST_MESSAGE_WARNING: " << err->message << ": "
					 << GST_MESSAGE_SRC_NAME(msg);
			break;

		case GST_MESSAGE_ERROR:

			gst_message_parse_error(msg, &err, nullptr);

			sp_log(Log::Error) << "Engine " << (int) engine->get_name() << ": GST_MESSAGE_ERROR: " << err->message << ": "
					 << GST_MESSAGE_SRC_NAME(msg);

			engine->set_track_finished(src);
			engine->stop();
			g_error_free(err);

			break;

		case GST_MESSAGE_STREAM_STATUS:
			/*GstStreamStatusType type;
			gst_message_parse_stream_status(msg, &type, NULL);
			sp_log(Log::Debug) << "Get stream status " << type;*/
			break;

		default:
			break;
	}

	return true;
}


// level changed
gboolean
EngineCallbacks::level_handler(GstBus * bus, GstMessage * message, gpointer data) {

	Q_UNUSED(bus);

	static double			channel_values[2];

	PlaybackEngine*			engine;
	GValueArray*			rms_arr;
	const GstStructure*		structure;
	const gchar*			name;
	const GValue*			peak_value;
	guint					n_peak_elements;

	engine = static_cast<PlaybackEngine*>(data);
	if(!engine) {
		return true;
	}

	structure = gst_message_get_structure(message);
	if(!structure) {
		sp_log(Log::Warning) << "structure is null";
		return true;
	}

	name = gst_structure_get_name(structure);
    if ( strcmp(name, "level") != 0 ) {
		return true;
    }

	peak_value = gst_structure_get_value(structure, "peak");
	if(!peak_value) {
		return true;
	}

	rms_arr = static_cast<GValueArray*>(g_value_get_boxed(peak_value));

	n_peak_elements = rms_arr->n_values;
	if(n_peak_elements == 0) {
		return true;
    }

	n_peak_elements = std::min((guint) 2, n_peak_elements);
	for(guint i=0; i<n_peak_elements; i++) {

		double d;
		const GValue* val;

		val = rms_arr->values + i;

		if(!G_VALUE_HOLDS_DOUBLE(val)) {
			sp_log(Log::Debug) << "Could not find a double";
			break;
		}

		d = g_value_get_double(val);
		if(d < 0){
			channel_values[i] = d;
		}
	}

	if(n_peak_elements >= 2) {
		engine->set_level(channel_values[0], channel_values[1]);
	}

	else if(n_peak_elements == 1) {
		engine->set_level(channel_values[0], channel_values[0]);
	}

	return true;
}


// spectrum changed
gboolean
EngineCallbacks::spectrum_handler(GstBus* bus, GstMessage* message, gpointer data) {

	Q_UNUSED(bus);

	PlaybackEngine*			engine;
	const GstStructure*		structure;
	const gchar*			structure_name;
	const GValue*			magnitudes;
	static QList<float>		spectrum_vals;

	engine = static_cast<PlaybackEngine*>(data);
	if(!engine) {
		return true;
	}

	structure = gst_message_get_structure(message);
	if(!structure) {
		return true;
    }

	structure_name = gst_structure_get_name(structure);
	if( strcmp(structure_name, "spectrum") != 0 ) {
		return true;
    }

	magnitudes = gst_structure_get_value (structure, "magnitude");

	if(spectrum_vals.isEmpty()){
		for (guint i = 0; i < N_BINS; ++i) {
			spectrum_vals << 0;
		}
	}

	for (guint i = 0; i < N_BINS; ++i) {

        float f;
		const GValue* mag;

		mag = gst_value_list_get_value(magnitudes, i);
		if(!mag) {
			continue;
		}

		f = (g_value_get_float(mag) + 75) / (75.0f);

		spectrum_vals[i] = f;
    }

	engine->set_spectrum(spectrum_vals);

	return true;
}
