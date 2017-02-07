/* PlaybackEngine.cpp */

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

#include "PlaybackEngine.h"
#include "PlaybackPipeline.h"
#include "StreamRecorder.h"
#include "SoundOutReceiver.h"

#include "Helper/Tagging/Tagging.h"
#include "Helper/FileHelper.h"
#include "Helper/Playlist/PlaylistMode.h"

#include <QUrl>
#include <algorithm>

PlaybackEngine::PlaybackEngine(QObject* parent) :
	Engine(parent)
{
	_name = EngineName::PlaybackEngine;
	_playing_stream = false;
	_sr_active = false;
	_stream_recorder = new StreamRecorder(this);
	_gapless_state = GaplessState::NoGapless;

	_gapless_timer = new QTimer();
	_gapless_timer->setTimerType(Qt::PreciseTimer);
	_gapless_timer->setSingleShot(true);

	connect(_gapless_timer, &QTimer::timeout, this, &PlaybackEngine::gapless_timed_out);

	REGISTER_LISTENER(Set::Engine_SR_Active, _streamrecorder_active_changed);
}


PlaybackEngine::~PlaybackEngine() {

	if(_stream_recorder->is_recording()){
		set_streamrecorder_recording(false);
	}

	if(_gapless_timer){
		delete _gapless_timer; _gapless_timer = nullptr;
	}


	delete _pipeline; _pipeline = nullptr;
	if(_other_pipeline)
	{
		delete _other_pipeline; _other_pipeline = nullptr;
	}

	delete _stream_recorder; _stream_recorder = nullptr;
}


bool PlaybackEngine::init() {

	gst_init(0, 0);

	_pipeline = new PlaybackPipeline(this);
	if(!_pipeline->init()){
		return false;
	}

	_other_pipeline = nullptr;

	connect(_pipeline, &PlaybackPipeline::sig_about_to_finish, this, &PlaybackEngine::set_about_to_finish);
	connect(_pipeline, &PlaybackPipeline::sig_pos_changed_ms, this, &PlaybackEngine::set_cur_position_ms);
	connect(_pipeline, &PlaybackPipeline::sig_data, this, &PlaybackEngine::sig_data);

	REGISTER_LISTENER(Set::PL_Mode, _playlist_mode_changed);
	return true;
}


void PlaybackEngine::change_track_gapless(const MetaData& md) {

	set_uri(md.filepath());
	_md = md;

	bool crossfade = _settings->get(Set::Engine_CrossFaderActive);

	if(crossfade){
		_pipeline->play();
		_pipeline->fade_in();
	}

	else {
		qint64 time_to_go = _other_pipeline->get_time_to_go();

		 if(time_to_go <= 0){
			_pipeline->play();
		 }

		 else{
			 _gapless_timer->setInterval(time_to_go);
			 _gapless_timer->start();

			 sp_log(Log::Debug) << "Will start playing in " << time_to_go << "msec";
		 }
	}

	change_gapless_state(GaplessState::TrackFetched);
}

void PlaybackEngine::change_track(const QString& filepath) {

	MetaData md;
	md.set_filepath(filepath);

	bool got_md = Tagging::getMetaDataOfFile(md);
	if( !got_md ) {
		stop();
	}

	else{
		change_track(md);
	}
}


void PlaybackEngine::change_track(const MetaData& md) {

	bool success;
	emit sig_pos_changed_s(0);

	if(_gapless_timer){
		_gapless_timer->stop();
	}

	if( _gapless_state == GaplessState::AboutToFinish ) {
		change_track_gapless(md);
		return;
	}

	if(_other_pipeline){
		_other_pipeline->stop();
	}

	success = set_uri(md.filepath());
	if (!success){
		return;
	}

	_md = md;
	_cur_pos_ms = 0;

	change_gapless_state(GaplessState::Playing);
}


bool PlaybackEngine::set_uri(const QString& filepath) {

	bool success = false;
	QUrl url;

	if(_uri){
		g_free(_uri);
		_uri = nullptr;
	}

	_playing_stream = Helper::File::is_www(filepath);

	// stream, but don't want to record
	// stream is already uri
	if (_playing_stream) {

		url = QUrl(filepath);
		_uri = g_strdup(url.toString().toUtf8().data());
	}

	// no stream (not quite right because of mms, rtsp or other streams
	// normal filepath -> no uri
	else if (!filepath.contains("://")) {

		url = QUrl::fromLocalFile(filepath);
		_uri = g_strdup(url.url().toUtf8().data());
	}

	else {
		_uri = g_strdup(filepath.toUtf8().data());
	}

	if(g_utf8_strlen(_uri, 1024) == 0){
		sp_log(Log::Warning) << "uri = 0";
		return false;
	}

	success = _pipeline->set_uri(_uri);

	return success;
}


void PlaybackEngine::play() {

	if( _gapless_state == GaplessState::AboutToFinish ||
		_gapless_state == GaplessState::TrackFetched )
	{
		return;
	}

	_pipeline->play();

	if(_sr_active && _stream_recorder->is_recording()){
		set_streamrecorder_recording(true);
	}
}


void PlaybackEngine::stop() {

	change_gapless_state(GaplessState::Playing);

	if(_gapless_timer){
		_gapless_timer->stop();
	}

	sp_log(Log::Info) << "Playback Engine: stop";
	_pipeline->stop();

	if(_other_pipeline){
		_other_pipeline->stop();
	}

	if(_sr_active && _stream_recorder->is_recording()){
		set_streamrecorder_recording(false);
	}

	emit sig_pos_changed_s(0);
}


void PlaybackEngine::pause() {
	_pipeline->pause();
}


void PlaybackEngine::jump_abs_ms(quint64 pos_ms) {
	_pipeline->seek_abs(pos_ms * GST_MSECOND);
}

void PlaybackEngine::jump_rel_ms(quint64 ms) {

	quint64 new_time_ms = _pipeline->get_position_ms() + ms;
	_pipeline->seek_abs(new_time_ms * GST_MSECOND);
}


void PlaybackEngine::jump_rel(double percent) {
	_pipeline->seek_rel(percent, _md.length_ms * GST_MSECOND);
}


void PlaybackEngine::set_equalizer(int band, int val) {

	double new_val;
	if (val > 0) {
		new_val = val * 0.25;
	}

	else{
		new_val = val * 0.75;
	}

	QString band_name = QString("band") + QString::number(band);
	_pipeline->set_eq_band(band_name, new_val);

	if(_other_pipeline){
		_other_pipeline->set_eq_band(band_name, new_val);
	}
}


void PlaybackEngine::set_buffer_state(int progress, GstElement* src)
{
	if(!Helper::File::is_www(_md.filepath())){
		return;
	}

	if(!_pipeline->has_element(src)){
		return;
	}

	emit sig_buffering(progress);
}


void PlaybackEngine::set_cur_position_ms(qint64 pos_ms) {

	if(sender() != _pipeline){
		return;
	}

	if(pos_ms < 0 && Helper::File::is_www(_md.filepath())){
		return;
	}

	qint32 pos_sec = pos_ms / 1000;
	qint32 cur_pos_sec = _cur_pos_ms / 1000;

	if ( cur_pos_sec == pos_sec ){
		return;
	}

	_cur_pos_ms = pos_ms;

	emit sig_pos_changed_s( pos_sec );
}


void PlaybackEngine::set_track_ready(GstElement* src){
	update_duration(src);

	if(_pipeline->has_element(src)){
		emit sig_track_ready();
	}
}


void PlaybackEngine::set_about_to_finish(qint64 time2go) {

	Q_UNUSED(time2go)

	if(sender() != _pipeline){
		return;
	}

	if( _gapless_state == GaplessState::NoGapless ||
		_gapless_state == GaplessState::AboutToFinish )
	{
		return;
	}

	sp_log(Log::Debug) << "About to finish: " << (int) _gapless_state << " (" << time2go << "ms)";
	change_gapless_state(GaplessState::AboutToFinish);

	bool crossfade = _settings->get(Set::Engine_CrossFaderActive);
	if(crossfade){
		_pipeline->fade_out();
	}

	// switch pipelines
	std::swap(_pipeline, _other_pipeline);

	emit sig_track_finished();
	emit sig_pos_changed_ms(0);
}


void PlaybackEngine::set_track_finished(GstElement* src)
{
	if(_pipeline->has_element(src)){
		emit sig_track_finished();
		emit sig_pos_changed_ms(0);
	}

	if(_other_pipeline && _other_pipeline->has_element(src)){
		sp_log(Log::Debug) << "Old track finished";

		_other_pipeline->stop();
		_cur_pos_ms = 0;
		change_gapless_state(GaplessState::Playing);
	}
}

void PlaybackEngine::gapless_timed_out()
{
	_pipeline->play();
}


void PlaybackEngine::_playlist_mode_changed() {

	PlaylistMode plm = _settings->get(Set::PL_Mode);
	bool gapless =	PlaylistMode::isActiveAndEnabled(plm.gapless()) ||
					_settings->get(Set::Engine_CrossFaderActive);

	if(gapless) {

		if(!_other_pipeline) {

			_other_pipeline = new PlaybackPipeline(this);
			if(!_other_pipeline->init()){
				change_gapless_state(GaplessState::NoGapless);
				return;
			}

			connect(_other_pipeline, &PlaybackPipeline::sig_about_to_finish, this, &PlaybackEngine::set_about_to_finish);
			connect(_other_pipeline, &PlaybackPipeline::sig_pos_changed_ms, this, &PlaybackEngine::set_cur_position_ms);
			connect(_other_pipeline, &PlaybackPipeline::sig_data, this, &PlaybackEngine::sig_data);
		}

		change_gapless_state(GaplessState::Playing);
	}

	else {
		change_gapless_state(GaplessState::NoGapless);
	}
}


void PlaybackEngine::change_gapless_state(GaplessState state)
{
	PlaylistMode plm = _settings->get(Set::PL_Mode);

	bool gapless = PlaylistMode::isActiveAndEnabled(plm.gapless());
	bool crossfader = _settings->get(Set::Engine_CrossFaderActive);

	_gapless_state = state;

	if(!gapless && !crossfader) {
		_gapless_state = GaplessState::NoGapless;
	}
}


void PlaybackEngine::_streamrecorder_active_changed(){

	_sr_active = _settings->get(Set::Engine_SR_Active);

	if(!_sr_active){
		set_streamrecorder_recording(false);
	}
}


void PlaybackEngine::set_streamrecorder_recording(bool b){

	QString dst_file;

	if(_stream_recorder->is_recording() != b){
		_stream_recorder->record(b);
	}

	if(b){
		dst_file = _stream_recorder->change_track(_md);
		if(dst_file.isEmpty()){
			return;
		}
	}

	if(_pipeline) {
		_pipeline->set_streamrecorder_path(dst_file);
	}
}

void PlaybackEngine::set_n_sound_receiver(int num_sound_receiver)
{
	_pipeline->set_n_sound_receiver(num_sound_receiver);

	if(_other_pipeline){
		_other_pipeline->set_n_sound_receiver(num_sound_receiver);
	}
}

void PlaybackEngine::update_cover(const QImage& img, GstElement* src)
{
	if( _pipeline->has_element(src) )
	{
		emit sig_cover_changed(img);
	}
}


void PlaybackEngine::update_md(const MetaData& md, GstElement* src){

	if(!_pipeline->has_element(src)){
		return;
	}

	if(!Helper::File::is_www( _md.filepath() )) {
		return;
	}

	if(md.title.isEmpty()) {
		return;
	}

	if(	md.title == _md.title)
	{
		return;
	}

	_cur_pos_ms = 0;
	_md.title = md.title;

	emit sig_md_changed(_md);

	if(_sr_active && _stream_recorder->is_recording()){
		set_streamrecorder_recording(true);
	}
}


void PlaybackEngine::update_duration(GstElement* src) {

	if(! _pipeline->has_element(src)){
		return;
	}

	_pipeline->refresh_duration();

	qint64 duration_ms = _pipeline->get_duration_ms();
	quint32 duration_s = (duration_ms >> 10);
	quint32 md_duration_s = (_md.length_ms >> 10);

	if(duration_s == 0 || duration_s > 1500000){
		return;
	}

	if(duration_s == md_duration_s) {
		return;
	}

	_md.length_ms = duration_ms;

	emit sig_dur_changed(_md);
}


void PlaybackEngine::update_bitrate(quint32 br, GstElement* src){

	if(!_pipeline->has_element(src)){
		return;
	}

	if( br / 1000 <= 0) {
		return;
	}

	if( br / 1000 == _md.bitrate / 1000) {
		return;
	}

	_md.bitrate = br;
	emit sig_br_changed(_md);
}


void PlaybackEngine::add_spectrum_receiver(SpectrumReceiver* receiver){
	_spectrum_receiver << receiver;
}

void PlaybackEngine::set_spectrum(const QList<float>& vals){
	for(SpectrumReceiver* rcv : _spectrum_receiver){
		if(rcv){
			rcv->set_spectrum(vals);
		}
	}
}


void PlaybackEngine::add_level_receiver(LevelReceiver* receiver){
	_level_receiver << receiver;
}

void PlaybackEngine::set_level(float left, float right){
	for(LevelReceiver* rcv : _level_receiver){
		if(rcv){
			rcv->set_level(left, right);
		}
	}
}
