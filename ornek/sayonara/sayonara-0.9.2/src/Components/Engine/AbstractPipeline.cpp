/* GSTPipeline.cpp */

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

#include "AbstractPipeline.h"
#include "Components/Engine/AbstractEngine.h"

#include "Callbacks/EngineCallbacks.h"
#include "Callbacks/PipelineCallbacks.h"

#include <gst/app/gstappsink.h>

AbstractPipeline::AbstractPipeline(QString name, Engine* engine, QObject* parent) :
	QObject(parent),
	SayonaraClass()
{
	_initialized = false;
	_engine = engine;
	_name = name;
    _position_ms = 0;
    _duration_ms = 0;
	_about_to_finish = false;
}

AbstractPipeline::~AbstractPipeline(){
	if (_bus){
		gst_object_unref (_bus);
	}

	if (_pipeline) {
		gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
		gst_object_unref (GST_OBJECT(_pipeline));
	}
}


bool AbstractPipeline::init(GstState state){

	bool success = false;
	if(_initialized) {
		return true;
	}

	// create equalizer element
	_pipeline = gst_pipeline_new(_name.toStdString().c_str());
	if(!_test_and_error(_pipeline, "Engine: Pipeline sucks")){
		return false;
	}

	_bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));

	success = create_elements();
	if(!success) {
		return false;
	}

	success = add_and_link_elements();
	if(!success) {
		return false;
	}

	configure_elements();

	gst_element_set_state(_pipeline, state);

#ifdef Q_OS_WIN
	gst_bus_set_sync_handler(_bus, EngineCallbacks::bus_message_received, _engine, EngineCallbacks::destroy_notify);
#else
	gst_bus_add_watch(_bus, EngineCallbacks::bus_state_changed, _engine);
#endif

	_progress_timer = new QTimer(this);
	_progress_timer->setInterval(200);
	connect(_progress_timer, &QTimer::timeout, this, [=](){
		if(this->get_state() != GST_STATE_NULL){
					PipelineCallbacks::position_changed(this);
		}
	});

	_progress_timer->start();

	_initialized = true;
	return true;
}

void AbstractPipeline::refresh_position() {

	gint64 pos_pipeline, pos_source;
	bool success;
	GstElement* element;

	element = get_source();

	if(!element){
		element = GST_ELEMENT(_pipeline);
	}

	success = gst_element_query_position(element, GST_FORMAT_TIME, &pos_source);

	if(success && (pos_source >> 10) > 0){
		_position_ms = GST_TIME_AS_MSECONDS(pos_source);
	}

	else if(success){
		success = gst_element_query_position(_pipeline, GST_FORMAT_TIME, &pos_pipeline);
		_position_ms = GST_TIME_AS_MSECONDS(pos_pipeline);
	}

	else{
		_position_ms = 0;
	}

	if(_duration_ms >= 0){
		emit sig_pos_changed_ms( _position_ms);
	}
}


void AbstractPipeline::refresh_duration(){

	gint64 dur;
	bool success;
	GstElement* element;

	element = get_source();
	if(!element){
		element = GST_ELEMENT(_pipeline);
	}

	success = gst_element_query_duration(element, GST_FORMAT_TIME, &dur);

	if(success){
		_duration_ms = GST_TIME_AS_MSECONDS(dur);
	}

	refresh_position();
}

void AbstractPipeline::set_data(uchar* data, quint64 size){
	emit sig_data(data, size);
}

void show_time_info(gint64 pos, gint64 dur){

	sp_log(Log::Debug) << "Difference: "
					   << dur - pos << ": "
					   << pos << " - "
					   << dur;

}

void AbstractPipeline::check_about_to_finish(){


	gint64 difference = _duration_ms - _position_ms;


	if(difference <= 0 && !_about_to_finish){
		refresh_duration();


		if(_duration_ms <= 0){
			return;
		}

		difference = _duration_ms - _position_ms;
	}

	//show_time_info(_position_ms, _duration_ms);

	qint64 about_to_finish_time = (qint64) get_about_to_finish_time();

	if(difference < about_to_finish_time && !_about_to_finish) {

		_about_to_finish = true;
		emit sig_about_to_finish(difference);
	}

	else if(difference > about_to_finish_time){
		_about_to_finish = false;
	}
}

qint64 AbstractPipeline::get_time_to_go() const
{
	gint64 position, duration;
	GstElement* element;

	element = get_source();
	if(!element){
		element = GST_ELEMENT(_pipeline);
	}

	element = GST_ELEMENT(_pipeline);

	gst_element_query_duration(element, GST_FORMAT_TIME, &duration);
	gst_element_query_position(element, GST_FORMAT_TIME, &position);

	return GST_TIME_AS_MSECONDS(duration - position) - 100;
}



qint64 AbstractPipeline::get_duration_ms(){
    return _duration_ms;
}

qint64 AbstractPipeline::get_position_ms(){
    return _position_ms;
}


void AbstractPipeline::finished() {

	emit sig_finished();
}


GstBus* AbstractPipeline::get_bus() {
	return _bus;
}


GstState AbstractPipeline::get_state() {
	GstState state;
	gst_element_get_state(_pipeline, &state, nullptr, GST_MSECOND * 10);
	return state;
}



GstElement* AbstractPipeline::get_pipeline() const {
	return _pipeline;
}

bool AbstractPipeline::set_uri(gchar* uri) {
	_uri = uri;
	return (_uri != nullptr);
}

gchar* AbstractPipeline::get_uri() {
	return _uri;
}


bool AbstractPipeline::create_element(GstElement** elem, const gchar* elem_name, const gchar* name){

	QString error_msg;
	if(strlen(name) > 0){
		*elem = gst_element_factory_make(elem_name, name);
		error_msg = QString("Engine: ") + name + " creation failed";
	}

	else{
		*elem = gst_element_factory_make(elem_name, elem_name);
		error_msg = QString("Engine: ") + elem_name + " creation failed";
	}

	bool success = _test_and_error(*elem, error_msg);

	return success;
}

bool AbstractPipeline::tee_connect(GstElement* tee, GstPadTemplate* tee_src_pad_template, GstElement* queue, const QString& queue_name){

	GstPadLinkReturn s;
	GstPad* tee_queue_pad;
	GstPad* queue_pad;

	QString error_1 = QString("Engine: Tee-") + queue_name + " pad is nullptr";
	QString error_2 = QString("Engine: ") + queue_name + " pad is nullptr";
	QString error_3 = QString("Engine: Cannot link tee with ") + queue_name;

	tee_queue_pad = gst_element_request_pad(tee, tee_src_pad_template, nullptr, nullptr);
	if(!_test_and_error(tee_queue_pad, error_1)){
		return false;
	}

	queue_pad = gst_element_get_static_pad(queue, "sink");
	if(!_test_and_error(queue_pad, error_2)) {
		return false;
	}

	s = gst_pad_link (tee_queue_pad, queue_pad);
	if(!_test_and_error_bool((s == GST_PAD_LINK_OK), error_3)) {
		return false;
	}

	g_object_set (queue, "silent", TRUE, nullptr);

	gst_object_unref(tee_queue_pad);
	gst_object_unref(queue_pad);
	return true;
}



bool
AbstractPipeline::has_element(GstElement* e) const
{
	if(!e){
		return true;
	}

	GstObject* o = (GstObject*) e;
	GstObject* parent = nullptr;

	while(o){
		if( o == (GstObject*) _pipeline ){
			if( (GstObject*) e != o ){
				gst_object_unref(o);
			}

			return true;
		}

		parent = gst_object_get_parent(o);
		if( (GstObject*) e != o ){
			gst_object_unref(o);
		}

		o = parent;
	}

	return false;

}


quint64 AbstractPipeline::get_about_to_finish_time() const
{
	return 300;
}

bool
_test_and_error(void* element, QString errorstr) {

	if(!element) {
		sp_log(Log::Error) << errorstr;
		return false;
	}

	return true;
}

bool
_test_and_error_bool(bool b, QString errorstr) {
	if(!b) {
		sp_log(Log::Error) << errorstr;
		return false;
	}

	return true;
}
