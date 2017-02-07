/* EnginePlugin.cpp */

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

#include "EnginePlugin.h"
#include "Components/Engine/EngineHandler.h"

EnginePlugin::EnginePlugin(QWidget* parent) :
	PlayerPluginInterface(parent)
{

}

EnginePlugin::~EnginePlugin(){

	if(_ecsc){
		delete _ecsc; _ecsc = nullptr;
	}
}


void EnginePlugin::init_buttons(bool small){

	int height = this->height() - 4;
	int x = 10;
	int y;
	int width;

	if(small){
		width = 15;
		y = 5;
	}

	else{
		width = 20;
		y = 10;
	}

	_btn_config->setGeometry(x, y, width, height);
	x += width + 5;
	_btn_prev->setGeometry(x, y, width, height);
	x += width + 5;
	_btn_next->setGeometry(x, y, width, height);
	x += width + 5;
	_btn_close->setGeometry(x, y, width, height);

	connect(_btn_config, &QPushButton::clicked, this, &EnginePlugin::config_clicked);
	connect(_btn_prev, &QPushButton::clicked, this, &EnginePlugin::prev_clicked);
	connect(_btn_next, &QPushButton::clicked, this, &EnginePlugin::next_clicked);
	connect(_btn_close, &QPushButton::clicked, this, &EnginePlugin::close);
	connect(_btn_close, &QPushButton::clicked, this->parentWidget(), &QWidget::close);

    _btn_config->hide();
    _btn_prev->hide();
    _btn_next->hide();
    _btn_close->hide();
}


void EnginePlugin::config_clicked(){
	_ui_style_settings->show(_cur_style_idx);
}

void EnginePlugin::next_clicked(){
    int n_styles = _ecsc->get_num_color_schemes();
    _cur_style_idx = (_cur_style_idx + 1) % n_styles;

	sl_update_style();
}


void EnginePlugin::prev_clicked(){

    int n_styles = _ecsc->get_num_color_schemes();
    _cur_style_idx = (_cur_style_idx - 1);
    if(_cur_style_idx < 0){
        _cur_style_idx = n_styles - 1;
    }

	sl_update_style();
}


void EnginePlugin::closeEvent(QCloseEvent *e) {
	PlayerPluginInterface::closeEvent(e);
    update();
}

void EnginePlugin::resizeEvent(QResizeEvent* e){

	PlayerPluginInterface::resizeEvent(e);

	if(!is_ui_initialized()){
		return;
	}

	sl_update_style();

	QSize new_size = e->size();

	if(!_btn_config) return;

	if(new_size.height() >= 30){
		_btn_config->setGeometry(10, 10, 20, 20);
		_btn_prev->setGeometry(35, 10, 20, 20);
		_btn_next->setGeometry(60, 10, 20, 20);
		_btn_close->setGeometry(85, 10, 20, 20);

		QFont font = _btn_config->font();
		font.setPointSize(8);
		_btn_config->setFont(font);
		_btn_prev->setFont(font);
		_btn_next->setFont(font);
		_btn_close->setFont(font);
	}

	else {
		_btn_config->setGeometry(10, 5, 15, 15);
		_btn_prev->setGeometry(30, 5, 15, 15);
		_btn_next->setGeometry(50, 5, 15, 15);
		_btn_close->setGeometry(70, 5, 15, 15);

		QFont font = _btn_config->font();
		font.setPointSize(6);
		_btn_config->setFont(font);
		_btn_prev->setFont(font);
		_btn_next->setFont(font);
		_btn_close->setFont(font);
	}

}


void EnginePlugin::mousePressEvent(QMouseEvent *e) {

    switch(e->button()){
        case Qt::LeftButton:
            next_clicked();
            break;
        case Qt::MidButton:
			if(this->parentWidget()){
				this->parentWidget()->close();
			}
            break;
        case Qt::RightButton:
			_ui_style_settings->show(_cur_style_idx);
			break;
        default:
            break;
    }
}


void EnginePlugin::enterEvent(QEvent* e){

	PlayerPluginInterface::enterEvent(e);

    _btn_config->show();
    _btn_prev->show();
    _btn_next->show();
    _btn_close->show();

}

void EnginePlugin::leaveEvent(QEvent* e){

	PlayerPluginInterface::leaveEvent(e);

    _btn_config->hide();
    _btn_prev->hide();
    _btn_next->hide();
	_btn_close->hide();
}


void EnginePlugin::update()
{
	QWidget::update();

	if(!is_ui_initialized()){
		return;
	}
}

void EnginePlugin::init_ui()
{
	_ecsc = new EngineColorStyleChooser(minimumWidth(), minimumHeight());
	_ui_style_settings = new GUI_StyleSettings(this);
	_engine = EngineHandler::getInstance();

	_timer = new QTimer();
	_timer->setInterval(30);
	_timer_stopped = true;

	connect(_timer, &QTimer::timeout, this, &EnginePlugin::timed_out);
	connect(_ui_style_settings, &GUI_StyleSettings::sig_style_update,
			this, &EnginePlugin::sl_update_style);
}

void EnginePlugin::played(){

}

void EnginePlugin::paused(){

}

void EnginePlugin::stopped(){

	if(!is_ui_initialized()){
		return;
	}

	_timer->start();
	_timer_stopped = false;
}


bool EnginePlugin::is_title_shown() const
{
	return false;
}
