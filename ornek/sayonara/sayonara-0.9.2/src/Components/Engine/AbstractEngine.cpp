/* Engine.cpp */

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

#include "AbstractEngine.h"

Engine::Engine(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{
	_name = EngineName::Undefined;
	_cur_pos_ms = 0;
	_playing_stream = false;
	_broadcast_active = false;
}

Engine::~Engine()
{

}

EngineName Engine::get_name() const {
	return _name;
}

void Engine::set_track_finished(GstElement* src){
	Q_UNUSED(src)

}

void Engine::async_done(GstElement* src){
	Q_UNUSED(src)

}

void Engine::update_md(const MetaData& md, GstElement* src){
	Q_UNUSED(src)
	Q_UNUSED(md)
}

void Engine::update_cover(const QImage& img, GstElement* src){
	Q_UNUSED(src)
	Q_UNUSED(img);
}

void Engine::update_duration(GstElement* src){
	Q_UNUSED(src)

}

void Engine::update_time(qint32 time, GstElement* src){
	Q_UNUSED(src)
	Q_UNUSED(time)
}

void Engine::update_bitrate(quint32 br, GstElement* src){
	Q_UNUSED(src)
	Q_UNUSED(br)
}

void Engine::set_track_ready(GstElement* src){
	Q_UNUSED(src)
}

void Engine::set_about_to_finish(qint64 ms){
	Q_UNUSED(ms)
}

void Engine::set_cur_position_ms(qint64 ms){
	_cur_pos_ms = ms;
	emit sig_pos_changed_ms(ms);
}


void Engine::set_buffer_state(int progress, GstElement* src)
{
	Q_UNUSED(progress)
	Q_UNUSED(src)
}
