/* PlaybackPipeline.cpp */

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

#include "PlaybackPipeline.h"

#include "Components/Engine/Callbacks/PipelineCallbacks.h"
#include "PipelineProbes.h"
#include "Helper/Helper.h"
#include "Helper/EqualizerPresets.h"

#include <gst/base/gstdataqueue.h>
#include <algorithm>

//http://gstreamer.freedesktop.org/data/doc/gstreamer/head/manual/html/chapter-dataaccess.html

PlaybackPipeline::PlaybackPipeline(Engine* engine, QObject *parent) :
	AbstractPipeline("Playback Pipeline", engine, parent),
	CrossFader(),
	ChangeablePipeline()
{
	_speed_val = 1.0f;
	_speed_active = false;

	_vol = 0;

	_level_probe = 0;
	_spectrum_probe = 0;
	_lame_probe = 0;
	_file_probe = 0;

	_run_broadcast = false;
	_show_level = false;
	_show_spectrum = false;

	_run_sr = false;
	_sr_data = new StreamRecorderData();
}


PlaybackPipeline::~PlaybackPipeline() {
}

bool PlaybackPipeline::init(GstState state){

	if(!AbstractPipeline::init(state)){
		return false;
	}

	_settings->set(SetNoDB::MP3enc_found, _lame != nullptr);
	_settings->set(SetNoDB::Pitch_found, _pitch != nullptr);

	REGISTER_LISTENER(Set::Engine_Vol, _sl_vol_changed);
	REGISTER_LISTENER(Set::Engine_Mute, _sl_mute_changed);

	// set by gui, initialized directly in pipeline
	REGISTER_LISTENER(Set::Engine_ShowLevel, _sl_show_level_changed);
	REGISTER_LISTENER(Set::Engine_ShowSpectrum, _sl_show_spectrum_changed);
	REGISTER_LISTENER(Set::Engine_Pitch, _sl_speed_changed);
	REGISTER_LISTENER(Set::Engine_Speed, _sl_speed_changed);
	REGISTER_LISTENER(Set::Engine_PreservePitch, _sl_speed_changed);
	REGISTER_LISTENER(Set::Engine_SpeedActive, _sl_speed_active_changed);
	set_n_sound_receiver(false);

	set_streamrecorder_path("");

	return true;
}

bool PlaybackPipeline::create_elements(){

	// input
	if(!create_element(&_audio_src, "uridecodebin", "src")) return false;
	if(!create_element(&_audio_convert, "audioconvert")) return false;
	if(!create_element(&_equalizer, "equalizer-10bands")) return false;

	if(!create_element(&_pitch, "pitch")){
		_pitch = nullptr;
	}

	if(!create_element(&_tee, "tee")) return false;

	// standard output branch
	if(!create_element(&_eq_queue, "queue", "eq_queue")) return false;
	//if(!create_element(&_speed, "scaletempo")) return false;
	if(!create_element(&_volume, "volume")) return false;
	if(!create_element(&_audio_sink, "autoaudiosink")) return false;

	// level branch
	if(!create_element(&_level_queue, "queue", "level_queue")) return false;
	if(!create_element(&_level, "level")) return false;
	if(!create_element(&_level_sink, "fakesink", "level_sink")) return false;

	// spectrum branch
	if(!create_element(&_spectrum_queue, "queue", "spectrum_queue")) return false;
	if(!create_element(&_spectrum, "spectrum")) return false;
	if(!create_element(&_spectrum_sink,"fakesink", "spectrum_sink")) return false;

	// lame branch
	if( !create_element(&_lame_queue, "queue", "lame_queue") ||
			!create_element(&_lame_converter, "audioconvert", "lame_converter") ||
			!create_element(&_lame_resampler, "audioresample", "lame_resampler") ||
			!create_element(&_lame, "lamemp3enc") ||
			!create_element(&_lame_app_sink, "appsink", "lame_appsink"))
	{
		_lame = nullptr;
	}

	// stream recorder branch
	if(	!create_element(&_file_queue, "queue", "sr_queue") ||
			!create_element(&_file_converter, "audioconvert", "sr_converter") ||
			!create_element(&_file_resampler, "audioresample", "sr_resample") ||
			!create_element(&_file_lame, "lamemp3enc", "sr_lame")  ||
			!create_element(&_file_sink, "filesink", "sr_filesink"))
	{
		_file_sink = nullptr;
	}

	else{
		_sr_data->queue = _file_queue;
		_sr_data->sink = _file_sink;
	}


	return true;
}

bool PlaybackPipeline::add_and_link_elements(){

	bool success;
	GstPadTemplate* tee_src_pad_template;

	gst_bin_add_many(GST_BIN(_pipeline),
					 _audio_src, _audio_convert, _equalizer, _tee,

					 _eq_queue, _volume, _audio_sink,
					 _level_queue, _level, _level_sink,
					 _spectrum_queue, _spectrum, _spectrum_sink,

					 nullptr);

	/* before tee */
	success = gst_element_link_many(_audio_convert, _equalizer, _tee,  nullptr);
	if(!_test_and_error_bool(success, "Engine: Cannot link audio convert with tee")){
		return false;
	}

	/* standard output branch */
	success = gst_element_link_many(_eq_queue, _volume, /*_speed,*/ _audio_sink, nullptr);
	if(!_test_and_error_bool(success, "Engine: Cannot link eq with audio sink")) {
		return false;
	}


	/* level branch */
	success = gst_element_link_many(_level_queue, _level, _level_sink, nullptr);
	if(!_test_and_error_bool(success, "Engine: Cannot link Level pipeline")){
		return false;
	}


	/* spectrum branch */
	success = gst_element_link_many(_spectrum_queue, _spectrum, _spectrum_sink, nullptr);

	if(!_test_and_error_bool(success, "Engine: Cannot link Spectrum pipeline")){
		return false;
	}


	/* lame branch (optional) */
	if(_lame){
		gst_bin_add_many(GST_BIN(_pipeline), _lame_queue,  _lame_converter, _lame_resampler, _lame, _lame_app_sink, nullptr);
		success = gst_element_link_many( _lame_queue, _lame_converter, _lame_resampler, _lame, _lame_app_sink, nullptr);
		_test_and_error_bool(success, "Engine: Cannot link lame stuff");
	}

	/* stream rippper branch (optional) */
	if(_file_sink){
		gst_bin_add_many(GST_BIN(_pipeline), _file_queue, _file_converter, _file_resampler, _file_lame, _file_sink, nullptr);
		success = gst_element_link_many( _file_queue, _file_converter, _file_resampler, _file_lame, _file_sink, nullptr);
		_test_and_error_bool(success, "Engine: Cannot link streamripper stuff");
	}

	/* create tee pads */
	tee_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (_tee), "src_%u");
	if(!_test_and_error(tee_src_pad_template, "Engine: _tee_src_pad_template is nullptr")) {
		return false;
	}

	/* connect branches with tee */
	success = tee_connect(_tee, tee_src_pad_template, _level_queue, "Level");
	if(!_test_and_error_bool(success, "Engine: Cannot link level queue with tee")){
		return false;
	}

	tee_connect(_tee, tee_src_pad_template, _spectrum_queue, "Spectrum");
	if(!_test_and_error_bool(success, "Engine: Cannot link spectrum queue with tee")){
		return false;
	}

	tee_connect(_tee, tee_src_pad_template, _eq_queue, "Equalizer");
	if(!_test_and_error_bool(success, "Engine: Cannot link eq queue with tee")){
		return false;
	}

	if(_lame){
		success = tee_connect(_tee, tee_src_pad_template, _lame_queue, "Lame");
		if(!_test_and_error_bool(success, "Engine: Cannot link lame queue with tee")){
			_settings->set(SetNoDB::MP3enc_found, false);
		}
	}

	if(_file_sink){
		success = tee_connect(_tee, tee_src_pad_template, _file_queue, "Streamripper");
		if(!_test_and_error_bool(success, "Engine: Cannot link streamripper stuff")){
			_settings->set(Set::Engine_SR_Active, false);
		}
	}

	return true;
}

bool PlaybackPipeline::configure_elements(){

	guint64 interval = 25000000;
	gint threshold = -75;
	QList<GstElement*> sinks;

	g_object_set (G_OBJECT (_audio_src),
				  "use-buffering", true,
				  nullptr);

	g_object_set (G_OBJECT (_level),
				  "post-messages", true,
				  "interval", interval,
				  nullptr);


	g_object_set (G_OBJECT (_spectrum),
				  "post-messages", true,
				  "interval", interval,
				  "bands", N_BINS,
				  "threshold", threshold,
				  "message-phase", false,
				  "message-magnitude", true,
				  "multi-channel", false,
				  nullptr);

	init_equalizer();

	/* run synced and not as fast as we can */
	g_object_set(G_OBJECT (_audio_sink),
				 "sync",
				 true,
				 nullptr);

	if(_lame){

		g_object_set(G_OBJECT (_lame),
					 "perfect-timestamp", true,
					 "target", 1,
					 "cbr", true,
					 "bitrate", 128,
					 "encoding-engine-quality", 2,
					 nullptr);

		gst_object_ref(_lame_app_sink);
		g_object_set(G_OBJECT(_lame_app_sink),
					 "emit-signals",
					 true, nullptr );

		sinks << _lame_app_sink;
	}

	if(_file_sink){
		g_object_set(G_OBJECT (_file_lame),

					 "perfect-timestamp", true,
					 "target", 1,
					 "cbr", true,
					 "bitrate", 128,
					 "encoding-engine-quality", 2,
					 nullptr);

		g_object_set(G_OBJECT(_file_sink),
//					 "buffer-mode", 1,
					 "buffer-size", 8192,
					 "location", (Helper::get_sayonara_path() + "bla.mp3").toLocal8Bit().data(),
					 nullptr);

		sinks << _file_sink;

		gst_element_set_state(_file_sink, GST_STATE_NULL);
	}

	sinks << _level_sink << _spectrum_sink;

	for(GstElement* sink : sinks){

		//gst_object_ref(sink);
		/* run synced and not as fast as we can */
		g_object_set(G_OBJECT (sink), "sync", true, nullptr);

		/* change the state, when it's said to change the state */
		g_object_set(G_OBJECT (sink), "async", false, nullptr);
	}

	g_signal_connect (_audio_src, "pad-added", G_CALLBACK (PipelineCallbacks::pad_added_handler), _audio_convert);
	if(_lame){
		g_signal_connect (_lame_app_sink, "new-sample", G_CALLBACK(PipelineCallbacks::new_buffer), this);
	}

	return true;
}

quint64 PlaybackPipeline::get_about_to_finish_time() const
{
	return std::max<quint64>(this->get_fading_time(),
							 AbstractPipeline::get_about_to_finish_time());
}


void PlaybackPipeline::init_equalizer()
{
	int last_idx = _settings->get(Set::Eq_Last);
	QList<EQ_Setting> presets = _settings->get(Set::Eq_List);
	presets.push_front(EQ_Setting());

	if( !between(last_idx, presets)){
		last_idx = 0;
	}

	EQ_Setting last_preset = presets[last_idx];
	QList<int> values = last_preset.values();

	int i=0;


	for(auto it=values.begin(); it != values.end(); it++){

		double new_val;
		if (*it > 0) {
			new_val = *it * 0.25;
		}

		else{
			new_val = *it * 0.75;
		}

		set_eq_band(QString("band") + QString::number(i), new_val);
		i++;
	}
}



void PlaybackPipeline::play()
{
	gst_element_set_state(_pipeline, GST_STATE_PLAYING);
	_sl_vol_changed();
}


void PlaybackPipeline::pause()
{
	gst_element_set_state(_pipeline, GST_STATE_PAUSED);
}


void PlaybackPipeline::stop()
{
	_position_ms = 0;
	_duration_ms = 0;
	_uri = nullptr;

	gst_element_set_state(_pipeline	, GST_STATE_NULL);
	abort_fader();
}

void PlaybackPipeline::_sl_vol_changed()
{
	_vol = _settings->get(Set::Engine_Vol);

	float vol_val = (float) ((_vol * 1.0f) / 100.0f);

	g_object_set(G_OBJECT(_volume), "volume", vol_val, nullptr);
}


void PlaybackPipeline::_sl_mute_changed()
{
	bool muted = _settings->get(Set::Engine_Mute);
	g_object_set(G_OBJECT(_volume), "mute", muted, nullptr);
}


bool PlaybackPipeline::_seek(gint64 ns)
{
	return gst_element_seek_simple(_audio_src,
							GST_FORMAT_TIME,
							(GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SNAP_NEAREST),
							ns);
}

gint64 PlaybackPipeline::seek_rel(double percent, gint64 ref_ns)
{
	gint64 new_time_ns;

	if (percent > 1.0){
		new_time_ns = ref_ns;
	}

	else if(percent < 0){
		new_time_ns = 0;
	}

	else{
		new_time_ns = (percent * ref_ns); // nsecs
	}


	if( _seek(new_time_ns) ) {
		return new_time_ns;
	}

	return 0;
}


gint64 PlaybackPipeline::seek_abs(gint64 ns)
{
	ns = std::max((gint64) 0, ns);

	if( _seek(ns) ) {
		return ns;
	}

	return 0;
}


void PlaybackPipeline::set_speed(float speed, double pitch, bool preserve_pitch)
{
	if(!_settings->get(Set::Engine_SpeedActive)) {
		return;
	}

	if(!_pitch){
		return;
	}

	if(preserve_pitch){
		g_object_set(_pitch,
					 "tempo", speed,
					 "rate", 1.0,
					 "pitch", pitch,
					 nullptr);
	}

	else{
		g_object_set(_pitch,
					 "tempo", 1.0,
					 "rate", speed,
					 "pitch", pitch,
					 nullptr);
	}
}


void PlaybackPipeline::change_pitch(int a_frequency)
{
	if(!_pitch){
		return;
	}

	if(a_frequency == 440){
		remove_element(_pitch, _audio_convert, _equalizer);
	}
	else{
		add_element(_pitch, _audio_convert, _equalizer);
	}

	double freq = a_frequency * 1.0;
	double scale = freq / 440.0;
	if(a_frequency == 440)
	{
		scale = 1.0;
	}

	g_object_set(G_OBJECT(_pitch),
		"pitch", scale,
		nullptr);
}


void PlaybackPipeline::_sl_show_level_changed()
{
	_show_level = _settings->get(Set::Engine_ShowLevel);

	Probing::handle_probe(&_show_level, _level_queue, &_level_probe, Probing::level_probed);
}


void PlaybackPipeline::_sl_show_spectrum_changed()
{
	_show_spectrum = _settings->get(Set::Engine_ShowSpectrum);

	Probing::handle_probe(&_show_spectrum, _spectrum_queue, &_spectrum_probe, Probing::spectrum_probed);
}


void PlaybackPipeline::set_n_sound_receiver(int num_sound_receiver)
{
	if(!_lame){
		return;
	}

	_run_broadcast = (num_sound_receiver > 0);

	Probing::handle_probe(&_run_broadcast, _lame_queue, &_lame_probe, Probing::lame_probed);
}

GstElement* PlaybackPipeline::get_source() const
{
	return _audio_src;
}

GstElement* PlaybackPipeline::get_pipeline() const
{
	return _pipeline;
}

bool PlaybackPipeline::set_uri(gchar* uri)
{
	if(!uri) return false;

	stop();

	g_object_set(G_OBJECT(_audio_src), "uri", uri, nullptr);

	GstElement* soup_source;
	g_object_get(G_OBJECT(_audio_src), "source", &soup_source, nullptr);
	if(soup_source != nullptr)
	{
		g_object_set(G_OBJECT(soup_source), 
				"ssl_strict", false, 
				NULL);		
	}

	gst_element_set_state(_pipeline, GST_STATE_PAUSED);

	return true;
}


void PlaybackPipeline::set_eq_band(const QString& band_name, double val)
{
	g_object_set( G_OBJECT(_equalizer),
				  band_name.toUtf8().data(),
				  val,
				  nullptr
	);
}



void PlaybackPipeline::set_streamrecorder_path(const QString& path)
{
	if(!_file_sink) {
		return;
	}

	if(path == _sr_path && !_sr_path.isEmpty()) {
		return;
	}

	if(_sr_data->busy){
		return;
	}

	_sr_path = path;
	_run_sr = !(path.isEmpty());

	gchar* old_filename = _sr_data->filename;

	_sr_data->filename = strdup(_sr_path.toUtf8().data());
	_sr_data->active = _run_sr;

	Probing::handle_stream_recorder_probe(_sr_data, Probing::stream_recorder_probed);

	if(old_filename){
		free(old_filename);
	}
}


void PlaybackPipeline::set_current_volume(double volume)
{
	g_object_set(_volume, "volume", volume, nullptr);
}


double PlaybackPipeline::get_current_volume() const
{
	double volume;
	g_object_get(_volume, "volume", &volume, nullptr);
	return volume;
}



void PlaybackPipeline::_sl_speed_active_changed()
{
	if(!_pitch){
		return;
	}

	bool active = _settings->get(Set::Engine_SpeedActive);
	GstElement* element = get_source();

	gint64 pos;
	gst_element_query_position(element, GST_FORMAT_TIME, &pos);

	if(active){
		add_element(_pitch, _audio_convert, _equalizer);
		_sl_speed_changed();
	}

	else{
		remove_element(_pitch, _audio_convert, _equalizer);
	}

	if(this->get_state() == GST_STATE_PLAYING){
		gst_element_seek_simple(element,
							GST_FORMAT_TIME,
							(GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SNAP_NEAREST),
							pos);

	}
}


void PlaybackPipeline::_sl_speed_changed()
{
	this->set_speed(
		_settings->get(Set::Engine_Speed),
		_settings->get(Set::Engine_Pitch) / 440.0,
		_settings->get(Set::Engine_PreservePitch)

	);
}
