/* GUI_Spectrum.cpp */

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



#include "GUI_Spectrum.h"
#include "Helper/globals.h"
#include "Helper/Logger/Logger.h"
#include "Components/Engine/Playback/PlaybackEngine.h"
#include "Components/Engine/EngineHandler.h"

#include <QPainter>
#include <QList>
#include <QTimer>

#include <cstring>
#include <cmath>
#include <algorithm>

float log_lu[1100];


GUI_Spectrum::GUI_Spectrum(QWidget *parent) :
	EnginePlugin(parent),
	Ui::GUI_Spectrum()
{
	_settings->set(Set::Engine_ShowSpectrum, false);
}

QString GUI_Spectrum::get_name() const
{
	return "Spectrum";
}

QString GUI_Spectrum::get_display_name() const
{
	return tr("Spectrum");
}

void GUI_Spectrum::language_changed(){

}


void GUI_Spectrum::init_ui()
{
	if(is_ui_initialized()){
		return;
	}

	EnginePlugin::init_ui();

	setup_parent(this);

	_cur_style_idx = _settings->get(Set::Spectrum_Style);
	_cur_style = _ecsc->get_color_scheme_spectrum(_cur_style_idx);

	for(int i=0; i<N_BINS; i++){
		_spec << 0.0f;
	}

	for(int i=0; i<1100; i++) {
		log_lu[i] = std::log( (i * 1.0f) / 10.0f ) * 0.60f;
	}

	_steps = new int*[N_BINS];
	for(int i=0; i<N_BINS; i++) {
		_steps[i] = new int[_cur_style.n_rects];
		std::memset(_steps[i], 0, (_cur_style.n_rects * sizeof(int)) );
	}

	_btn_config = new QPushButton("...", this);
	_btn_prev = new QPushButton("<", this);
	_btn_next = new QPushButton(">", this);
	_btn_close = new QPushButton("x", this);

	init_buttons(false);

	update();

	PlaybackEngine* playback_engine = _engine->get_playback_engine();
	if(playback_engine){
		playback_engine->add_spectrum_receiver(this);
	}
}


void
GUI_Spectrum::set_spectrum(const QList<float>& lst) {

	if(!is_ui_initialized() || !isVisible()){
		return;
	}

	if(!_timer_stopped) {
		_timer->stop();
	}

    _spec = lst;

	update();
}


void
GUI_Spectrum::paintEvent(QPaintEvent *e) {

	Q_UNUSED(e)

     QPainter painter(this);

	float widget_height = (float) height();

     int n_rects = _cur_style.n_rects;
     int n_fading_steps = _cur_style.n_fading_steps;
	 int h_rect = (widget_height / n_rects) - _cur_style.ver_spacing;
     int border_y = _cur_style.ver_spacing;
     int border_x = _cur_style.hor_spacing;


    int x=3;
    int ninety = (_spec.size() * 500) / 1000;
    int offset = 0;
    int n_zero = 0;

	if(ninety == 0) {
		return;
	}

	int w_bin = ((width() + 10) / (ninety - offset)) - border_x;

    // run through all bins
    for(int i=offset; i<ninety + 1; i++) {

		float f = _spec[i] * log_lu[ i*10 + 54];

        // if this is one bar, how tall would it be?
        int h =  f * widget_height;

        // how many colored rectangles would fit into this bar?
		int colored_rects = h / (h_rect + border_y) - 1 ;

		colored_rects = std::max(colored_rects, 0);

        // we start from bottom with painting
        int y = widget_height - h_rect;

        // run vertical

		QRect rect(x, y, w_bin, h_rect);
		QColor col;
		for(int r=0; r<n_rects; r++) {

            // 100%
            if( r < colored_rects) {
                col = _cur_style.style[r].value(-1);
                _steps[i][r] = n_fading_steps;
            }

            // fading out
            else{
                col = _cur_style.style[r].value(_steps[i][r]);

				if(_steps[i][r] > 0) {
					_steps[i][r]--;
				}

				else {
					n_zero++;
				}
            }

			painter.fillRect(rect, col);

			rect.translate(0, -(h_rect + border_y));
        }

        x += w_bin + border_x;
    }

    if(n_zero == (ninety - offset) * n_rects && _timer->isActive()) {
        _timer->stop();
        _timer_stopped = true;
    }
}


void GUI_Spectrum::timed_out() {

	for(auto it=_spec.begin(); it!= _spec.begin(); it++) {
		*it -= 0.024f;
    }

    update();
}


void GUI_Spectrum::resize_steps(int n_bins, int rects) {

	if(!is_ui_initialized()){
		return;
	}

	if(_steps){
		for(int b=0; b<n_bins; b++) {
			delete[] _steps[b];
			_steps[b] = nullptr;
		}

		delete[] _steps;
	}

	_steps = new int*[n_bins];

	for(int i=0; i<n_bins; i++) {
        _steps[i] = new int[rects];
		std::memset(_steps[i], 0, rects * sizeof(int));
    }
}


void GUI_Spectrum::sl_update_style() {

	if(!is_ui_initialized()){
		return;
	}

   _ecsc->reload(width(), height());
   _cur_style = _ecsc->get_color_scheme_spectrum(_cur_style_idx);
   _settings->set(Set::Spectrum_Style, _cur_style_idx);

   resize_steps(N_BINS, _cur_style.n_rects);

   update();
}


void GUI_Spectrum::showEvent(QShowEvent* e){
	_settings->set(Set::Engine_ShowSpectrum, true);
	EnginePlugin::showEvent(e);
}

void GUI_Spectrum::closeEvent(QCloseEvent* e){
	_settings->set(Set::Engine_ShowSpectrum, false);
	EnginePlugin::closeEvent(e);
}
