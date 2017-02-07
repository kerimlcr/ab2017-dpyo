/* CrossFader.cpp */

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



/* CrossFader.cpp */

#include "CrossFader.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Helper.h"

#include <QThread>

#include <glib.h>
#include <functional>

class FaderThreadData
{

private:

	int						_cycles;
	int						_cycle_time_ms;
	std::function<void ()>	_fn;

public:

	FaderThreadData(const std::function<void ()>& fn){
		reset();
		_fn = fn;
	}

	void set_fading_time(int fading_time){

		_cycle_time_ms = fading_time / _cycles;
	}

	void set_callback(const std::function<void ()>& fn){
		_fn = fn;
	}

	void reset(){
		_cycles = 500;
	}

	bool is_active() const
	{
		return (_cycles > 0);
	}

	void abort()
	{
		_cycles = 0;
	}

	void wait(){
		Helper::sleep_ms(_cycle_time_ms);
		_cycles --;
		_fn();
	}
};

class FaderThread : public QThread
{
private:
	FaderThreadData* _ftd=nullptr;

public:
	FaderThread(FaderThreadData* data) : QThread(nullptr){
		_ftd = data;
	}

protected:
	void run() override {

		while(_ftd && _ftd->is_active())
		{
			_ftd->wait();
		}
	}
};


CrossFader::CrossFader()
{
	_fade_mode = CrossFader::FadeMode::NoFading;

	_fader_data = new FaderThreadData(
				std::bind(&CrossFader::fader_timed_out, this)
	);

}


void CrossFader::init_fader()
{
	if(_fade_mode == CrossFader::FadeMode::NoFading){
		return;
	}

	if(_fader && _fader_data->is_active()){
		_fader_data->abort();

		while(_fader->isRunning()){
			Helper::sleep_ms(10);
		}

		delete _fader; _fader=nullptr;
	}

	int fading_time = Settings::getInstance()->get(Set::Engine_CrossFaderTime);

	_fader_data->reset();
	_fader_data->set_fading_time(fading_time);
	_fader = new FaderThread(_fader_data);
	_fader->start();
}


void CrossFader::fade_in()
{
	double volume = Settings::getInstance()->get(Set::Engine_Vol) / 100.0;

	_fade_mode = CrossFader::FadeMode::FadeIn;

	_fade_step = volume / 500.0;

	set_current_volume(0.0001);

	init_fader();
}

void CrossFader::fade_out()
{
	double volume = Settings::getInstance()->get(Set::Engine_Vol) / 100.0;

	_fade_mode = CrossFader::FadeMode::FadeOut;
	_fade_step = volume / 500.0;

	set_current_volume( volume );

	init_fader();
}


void CrossFader::fader_timed_out()
{
	if(_fade_mode == CrossFader::FadeMode::FadeIn){
		increase_volume();
	}

	else if(_fade_mode == CrossFader::FadeMode::FadeOut){
		decrease_volume();
	}
}


void CrossFader::increase_volume()
{
	double max_volume = Settings::getInstance()->get(Set::Engine_Vol) / 100.0;
	double volume = get_current_volume();

	volume += _fade_step;

	if(volume > max_volume){
		abort_fader();

		return;
	}

	set_current_volume(volume);
}


void CrossFader::decrease_volume()
{
	double volume = get_current_volume();

	volume -= _fade_step;
	if(volume < 0.00001){

		abort_fader();
		return;
	}

	set_current_volume(volume);
}


quint64 CrossFader::get_fading_time() const
{
	Settings* settings = Settings::getInstance();
	if(settings->get(Set::Engine_CrossFaderActive)){
		return settings->get(Set::Engine_CrossFaderTime);
	}

	return 0;
}

void CrossFader::abort_fader()
{
	if(_fader_data->is_active()){
		_fader_data->abort();
	}
}


