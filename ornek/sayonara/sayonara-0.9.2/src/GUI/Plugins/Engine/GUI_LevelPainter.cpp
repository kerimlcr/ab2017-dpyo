/* GUI_LevelPainter.cpp */

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

#include "GUI_LevelPainter.h"
#include "Components/Engine/Playback/PlaybackEngine.h"
#include "Components/Engine/EngineHandler.h"

#include <QPainter>
#include <QBrush>

#include <cstring>
#include <cmath>
#include <algorithm>


GUI_LevelPainter::GUI_LevelPainter(QWidget *parent) :
	EnginePlugin(parent),
	Ui::GUI_LevelPainter()
{
	_settings->set(Set::Engine_ShowLevel, false);

}

QString GUI_LevelPainter::get_name() const
{
	return "Level";
}

QString GUI_LevelPainter::get_display_name() const
{
	return tr("Level");
}

void GUI_LevelPainter::language_changed(){
	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
}


void GUI_LevelPainter::init_ui()
{
	if(is_ui_initialized()){
		return;
	}

	EnginePlugin::init_ui();
	setup_parent(this);

	_cur_style_idx = _settings->get(Set::Level_Style);
	_cur_style = _ecsc->get_color_scheme_level(_cur_style_idx);
	reload();

	int n_rects = _cur_style.n_rects;

	for(float i=-6; i<=0; i+=0.01f){
		_exp_lot[(int)(-i * 100)] = std::exp(i);
	}

	_steps = new int*[2];

	_steps[0] = new int[n_rects];
	_steps[1] = new int[n_rects];

	std::memset(_steps[0], 0, n_rects * sizeof(int));
	std::memset(_steps[1], 0, n_rects * sizeof(int));


	_level[0] = 0;
	_level[1] = 0;

	_btn_config = new QPushButton("...", this);
	_btn_prev = new QPushButton("<", this);
	_btn_next = new QPushButton(">", this);
	_btn_close = new QPushButton("x", this);

	init_buttons(true);

	PlaybackEngine* playback_engine = _engine->get_playback_engine();
	if(playback_engine){
		playback_engine->add_level_receiver(this);
	}
}


void GUI_LevelPainter::set_level(float level_l, float level_r) {

	if(!is_ui_initialized() || !isVisible()){
		return;
	}

	if(!_timer_stopped) {
		_timer->stop();
		_timer_stopped = true;
	}

	_level[0] = level_l;
	_level[1] = level_r;

	update();
}

void GUI_LevelPainter::paintEvent(QPaintEvent* e) {
	Q_UNUSED(e)

	QPainter painter(this);

	int n_rects = _cur_style.n_rects;
	int border_x = _cur_style.hor_spacing;
	int border_y = _cur_style.ver_spacing;
	int n_fading_steps = _cur_style.n_fading_steps;
	int h_rect = _cur_style.rect_height;
	int w_rect = _cur_style.rect_width;

	int y = 10;
	int num_zero = 0;
	int x_init = (w_rect + border_x);

	for(int c=0; c<2; c++) {

		float level= -std::max(_level[c], -39.9f) * 15.0f;
		int idx = std::max(0, std::min(599, (int) level));

		level = _exp_lot[idx];

		int n_colored_rects = n_rects * level;

		QRect rect(0, y, w_rect, h_rect);
		for(int r=0; r<n_rects; r++) {

			if(r < n_colored_rects) {
				if(!_cur_style.style[r].contains(-1)){
					sp_log(Log::Debug) << "Style does not contain -1";
				}
				painter.fillRect(rect, _cur_style.style[r].value(-1) );
				_steps[c][r] = n_fading_steps - 1;
			}

			else{
				if(!_cur_style.style[r].contains(_steps[c][r])){
					sp_log(Log::Debug) << "2 Style does not contain " << _steps[c][r] << ", " << c << ", " << r;
				}

				painter.fillRect(rect, _cur_style.style[r].value(_steps[c][r]) );
				if(_steps[c][r] > 0) {
					_steps[c][r] -= 1;
				}

				if(_steps[c][r] == 0) {
					num_zero ++;
				}
			}

			rect.translate(x_init, 0);
		}

		if(num_zero == 2 * n_rects) {
			_timer->stop();
			_timer_stopped = true;
		}

		y += h_rect + border_y;
	}
}


void GUI_LevelPainter::timed_out() {

	for(int i=0; i<2; i++){
		_level[i] -= 2.0f;
	}

	update();
}

void GUI_LevelPainter::sl_update_style() {

	_settings->set(Set::Level_Style, _cur_style_idx);
	_ecsc->reload(width(), height());
	_cur_style = _ecsc->get_color_scheme_level(_cur_style_idx);

	resize_steps(_cur_style.n_rects);

	update();

}

void GUI_LevelPainter::resize_steps(int n_rects) {

	if(!_steps){
		_steps = new int*[2];
		memset(_steps, 0, sizeof(int*) * 2);
	}

	for(int i=0; i<2; i++){
		if(_steps[i]){
			delete[] _steps[i];
		}

		_steps[i] = new int[n_rects];
		memset(_steps[i], 0, n_rects * sizeof(int));
	}
}


void GUI_LevelPainter::reload() {
	int new_height = _cur_style.rect_height * 2 + _cur_style.ver_spacing + 12;

	setMinimumHeight(0);
	setMaximumHeight(100);

	setMinimumHeight(new_height);
	setMaximumHeight(new_height);

	if(isVisible()){
		emit sig_reload(this);
	}
}

void GUI_LevelPainter::showEvent(QShowEvent* e){
	_settings->set(Set::Engine_ShowLevel, true);
	EnginePlugin::showEvent(e);
}


void GUI_LevelPainter::closeEvent(QCloseEvent* e){
	_settings->set(Set::Engine_ShowLevel, false);
	EnginePlugin::closeEvent(e);
}
