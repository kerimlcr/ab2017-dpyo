/* ConvertEngine.cpp */

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


#include "ConvertEngine.h"

#include "Components/Engine/AbstractEngine.h"
#include "Components/Engine/Callbacks/PipelineCallbacks.h"
#include "ConvertPipeline.h"
#include "Helper/Tagging/Tagging.h"
#include "Helper/FileHelper.h"

#include <QUrl>

ConvertEngine::ConvertEngine(QObject *parent) :
	Engine(parent)
{
	_pipeline = new ConvertPipeline(this);
	_name = EngineName::ConvertEngine;

	connect(_pipeline, &ConvertPipeline::sig_pos_changed_ms, this, &ConvertEngine::set_cur_position_ms);
}

bool ConvertEngine::init() {
	return _pipeline->init();
}


// methods
bool ConvertEngine::set_uri(const MetaData& md) {

	QUrl url;
	gchar* uri;
	gchar* target_uri;
	QString cvt_target_path;
	bool success = false;

	if(_uri){
		g_free(_uri);
		_uri = nullptr;
	}

	if(_target_uri){
		g_free(_target_uri);
		_target_uri = nullptr;
	}

	_playing_stream = Helper::File::is_www( md.filepath() );

	if (_playing_stream) {
		url = QUrl(md.filepath());
		uri = url.toString().toUtf8().data();
	}

	// no stream (not quite right because of mms, rtsp or other streams
	// normal filepath -> no uri
	else if (!md.filepath().contains("://")) {

		url = QUrl::fromLocalFile(md.filepath());
		uri = url.toString().toUtf8().data();
	}

	else {
		uri = md.filepath().toUtf8().data();
	}

	QString filename = Helper::File::get_filename_of_path(md.filepath());
	int idx = filename.lastIndexOf(".");
	if(idx > 0) {
		filename = filename.left(idx);
	}

	cvt_target_path = _settings->get(Set::Engine_CovertTargetPath);
	filename = cvt_target_path + "/" + filename + ".mp3";

	target_uri = filename.toUtf8().data();

	_uri = g_strdup(uri);
	_target_uri = g_strdup(target_uri);

	success = _pipeline->set_uri(g_strdup(uri));
	_pipeline->set_target_uri(target_uri);

	_md_target = md;
	_md_target.set_filepath(filename);

	return success;
}

void ConvertEngine::change_track(const MetaData& md) {

	stop();
	_md = md;

	emit sig_md_changed(_md);
	emit sig_pos_changed_s(0);

	set_uri(md);
}

void ConvertEngine::change_track(const QString& str) {
	Q_UNUSED(str);
}


void ConvertEngine::play() {

	_pipeline->play();

	g_timeout_add(200, (GSourceFunc) PipelineCallbacks::position_changed, this);
}

void ConvertEngine::pause() {
	return;
}

void ConvertEngine::stop() {

	_pipeline->stop();

    Tagging::setMetaDataOfFile(_md_target);
}

// public from Gstreamer Callbacks
void ConvertEngine::set_track_finished(GstElement* src) {
	Q_UNUSED(src)
	emit sig_track_finished();
}

void ConvertEngine::set_cur_position_ms(qint64 v) {
	emit sig_pos_changed_s((quint32) v / 1000);
}


void ConvertEngine::set_volume(int vol) {
	Q_UNUSED(vol);
}


void ConvertEngine::jump_abs_ms(quint64 pos_ms) {
	Q_UNUSED(pos_ms);
}

void ConvertEngine::jump_rel_ms(quint64 ms){
	Q_UNUSED(ms);
}

void ConvertEngine::jump_rel(double percent) {
	Q_UNUSED(percent);
}
