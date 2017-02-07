/* EngineHandler.cpp */

/* Copyright (C) 2014  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY{} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "EngineHandler.h"

#include "Components/PlayManager/PlayManager.h"
#include "Interfaces/RawSoundReceiver/RawSoundReceiverInterface.h"

#include "Playback/PlaybackEngine.h"
#include "Convert/ConvertEngine.h"


EngineHandler::EngineHandler(QObject* parent) : Engine(parent) {

	_cur_engine = nullptr;

	_play_manager = PlayManager::getInstance();

	connect(_play_manager, &PlayManager::sig_playstate_changed,
			this, &EngineHandler::playstate_changed);

	connect(_play_manager, &PlayManager::sig_track_changed,
			this, EngineHandler_change_track_md);

	connect(_play_manager, &PlayManager::sig_seeked_abs_ms,
			this, &EngineHandler::jump_abs_ms);

	connect(_play_manager, &PlayManager::sig_seeked_rel,
			this, &EngineHandler::jump_rel);

	connect(_play_manager, &PlayManager::sig_seeked_rel_ms,
			this, &EngineHandler::jump_rel_ms);

	connect(_play_manager, &PlayManager::sig_record,
			this, &EngineHandler::sr_record_button_pressed);

	MetaData md = _play_manager->get_cur_track();
	if(!md.filepath().isEmpty()){
		change_track(md);
	}

	switch_engine(EngineName::PlaybackEngine);
}


EngineHandler::~EngineHandler() {
	for(Engine* e : _engines)
	{
		delete e;
	}

	_engines.clear();
}

bool EngineHandler::init() {

	return true;
}


void EngineHandler::start_convert() {

	stop();

	if( _cur_engine->get_name() != EngineName::ConvertEngine ) {
		switch_engine(EngineName::ConvertEngine);
    }

	_cur_engine->stop();
}

void EngineHandler::end_convert() {

	stop();

	sp_log(Log::Debug) << "Engine end convert";

	if( _cur_engine->get_name() != EngineName::PlaybackEngine ) {
		sp_log(Log::Debug) << "Change to playback engine";
		switch_engine(EngineName::PlaybackEngine);
	}

	_cur_engine->stop();
}



void EngineHandler::fill_engines(const QList<Engine*>& engines) {
	_engines = engines;

	switch_engine(EngineName::PlaybackEngine);
}


void EngineHandler::playstate_changed(PlayManager::PlayState state){

	if(!_cur_engine) return;

	switch(state){
		case PlayManager::PlayState::Playing:
			play();
			break;

		case PlayManager::PlayState::Paused:
			pause();
			break;

		case PlayManager::PlayState::Stopped:
			stop();
			break;

		default:
			return;
	}
}


void EngineHandler::play() {
	if(!_cur_engine) return;

	_cur_engine->play();
}

void EngineHandler::stop() {

	if(!_cur_engine) return;
	_cur_engine->stop();
}

void EngineHandler::pause() {
	if(!_cur_engine) return;
	_cur_engine->pause();
}



void EngineHandler::jump_abs_ms(quint64 ms) {
	if(!_cur_engine) return;
	_cur_engine->jump_abs_ms(ms);
}

void EngineHandler::jump_rel_ms(quint64 ms){
	if(!_cur_engine) return;
	_cur_engine->jump_rel_ms(ms);
}

void EngineHandler::jump_rel(double where) {
	if(!_cur_engine) return;
	_cur_engine->jump_rel(where);
}


void EngineHandler::change_track(const MetaData& md) {
	if(!_cur_engine) return;
	_cur_engine->change_track(md);
}

void EngineHandler::change_track(const QString& str) {
	if(!_cur_engine) return;
	_cur_engine->change_track(str);
}


void EngineHandler::sl_md_changed(const MetaData& md) {
	_play_manager->change_metadata(md);
	emit sig_md_changed(md);
}

void EngineHandler::sl_dur_changed(const MetaData& md){
	_play_manager->change_duration(md.length_ms);
	emit sig_dur_changed(md);
}

void EngineHandler::sl_pos_changed_ms(quint64 ms) {
	_play_manager->set_position_ms(ms);
}

void EngineHandler::sl_pos_changed_s(quint32 sec) {
	_play_manager->set_position_ms( (quint64) (sec * 1000) );
}

void EngineHandler::sl_track_ready_changed(){
	_play_manager->set_track_ready();
}

void EngineHandler::sl_track_finished() {
	_play_manager->next();
}


void EngineHandler::sl_buffer_state_changed(int progress){
	_play_manager->buffering(progress);
}

void EngineHandler::sr_record_button_pressed(bool b){

	PlaybackEngine* p = get_playback_engine();
	if(p){
		p->set_streamrecorder_recording(b);
	}
}

bool EngineHandler::configure_connections(Engine* old_engine, Engine* new_engine) {

	if(!old_engine && !new_engine) return false;
	if(old_engine == new_engine) return false;

	if(old_engine) {
		disconnect(old_engine, &Engine::sig_track_ready, this, &EngineHandler::sl_track_ready_changed);
		disconnect(old_engine, &Engine::sig_md_changed, this, &EngineHandler::sl_md_changed);
		disconnect(old_engine, &Engine::sig_pos_changed_ms, this, &EngineHandler::sl_pos_changed_ms);
		disconnect(old_engine, &Engine::sig_pos_changed_s, this, &EngineHandler::sl_pos_changed_s);
		disconnect(old_engine, &Engine::sig_dur_changed, this, &EngineHandler::sl_dur_changed);
		disconnect(old_engine, &Engine::sig_br_changed, this, &Engine::sig_br_changed);
		disconnect(old_engine, &Engine::sig_track_finished, this, &EngineHandler::sl_track_finished);
		disconnect(old_engine, &Engine::sig_buffering, this, &EngineHandler::sl_buffer_state_changed);
		disconnect(old_engine, &Engine::sig_cover_changed, this, &EngineHandler::sig_cover_changed);
	}

	if(new_engine) {
		connect(new_engine, &Engine::sig_track_ready, this, &EngineHandler::sl_track_ready_changed);
		connect(new_engine, &Engine::sig_md_changed, this, &EngineHandler::sl_md_changed);
		connect(new_engine, &Engine::sig_pos_changed_ms, this, &EngineHandler::sl_pos_changed_ms);
		connect(new_engine, &Engine::sig_pos_changed_s, this, &EngineHandler::sl_pos_changed_s);
		connect(new_engine, &Engine::sig_dur_changed, this, &EngineHandler::sl_dur_changed);
		connect(new_engine, &Engine::sig_br_changed, this, &Engine::sig_br_changed);
		connect(new_engine, &Engine::sig_track_finished, this, &EngineHandler::sl_track_finished);
		connect(new_engine, &Engine::sig_buffering, this, &EngineHandler::sl_buffer_state_changed);
		connect(new_engine, &Engine::sig_cover_changed, this, &EngineHandler::sig_cover_changed);
	}

	return true;
}

Engine* EngineHandler::get_engine(EngineName name)
{
	for(Engine* e : _engines){
		if(e && e->get_name() == name){
			return e;
		}
	}

	if(name == EngineName::PlaybackEngine){
		PlaybackEngine* pb_engine = new PlaybackEngine();
		if(pb_engine->init()){
			_engines << static_cast<Engine*>(pb_engine);
			connect(pb_engine, &PlaybackEngine::sig_data, this, &EngineHandler::new_data);
			return pb_engine;
		}
	}

	else if(name == EngineName::ConvertEngine){
		ConvertEngine* cvt_engine = new ConvertEngine();
		if(cvt_engine->init()){
			_engines << static_cast<Engine*>(cvt_engine);
			return cvt_engine;

		}
	}

	return nullptr;
}


void EngineHandler::switch_engine(EngineName name) {

	Engine* new_engine=get_engine(name);

	if(!new_engine){
		return;
	}

	configure_connections(_cur_engine, new_engine);
	_cur_engine = new_engine;

}

PlaybackEngine* EngineHandler::get_playback_engine(){
	return dynamic_cast<PlaybackEngine*>(get_engine(EngineName::PlaybackEngine));
}

ConvertEngine* EngineHandler::get_convert_engine(){
	return dynamic_cast<ConvertEngine*>(get_engine(EngineName::ConvertEngine));
}

void EngineHandler::new_data(const uchar* data, quint64 n_bytes){
	for(RawSoundReceiverInterface* receiver : _raw_sound_receiver){
		receiver->new_audio_data(data, n_bytes);
	}
}

void EngineHandler::register_raw_sound_receiver(RawSoundReceiverInterface* receiver)
{
	PlaybackEngine* engine;

	if(_raw_sound_receiver.contains(receiver)){
		return;
	}

	_raw_sound_receiver << receiver;

	engine = get_playback_engine();
	if(engine){
		get_playback_engine()->set_n_sound_receiver(_raw_sound_receiver.size());
	}
}

void EngineHandler::unregister_raw_sound_receiver(RawSoundReceiverInterface* receiver){

	PlaybackEngine* engine;

	if(!_raw_sound_receiver.contains(receiver)){
		return;
	}

	_raw_sound_receiver.removeOne(receiver);

	engine = get_playback_engine();
	if(engine){
		get_playback_engine()->set_n_sound_receiver(_raw_sound_receiver.size());
	}
}


void EngineHandler::set_equalizer(int band, int value){
	PlaybackEngine* engine = get_playback_engine();

	if(engine){
		engine->set_equalizer(band, value);
	}
}
