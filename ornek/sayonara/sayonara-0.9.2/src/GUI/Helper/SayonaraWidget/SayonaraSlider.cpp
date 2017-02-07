/* SayonaraSlider.cpp */

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



#include "SayonaraSlider.h"

SayonaraSlider::SayonaraSlider(QWidget *parent) :
	QSlider(parent)
{
	this->setTracking(true);
	this->setMouseTracking(true);
	this->setSingleStep(1);
	this->setPageStep(1);
}

bool SayonaraSlider::event(QEvent *e){

	/** We need this for activate an item as soon it is hovered.
	Otherwise, the curve functionality with the mouse wheel event does not work **/
	switch(e->type()){

		case QEvent::HoverEnter:
			emit sig_slider_got_focus();
			break;

		case QEvent::HoverLeave:

			if(!this->hasFocus()){
				emit sig_slider_lost_focus();
			}

			break;

		default: 
			break;
	}

	return QSlider::event(e);
}

void SayonaraSlider::focusInEvent(QFocusEvent* e){
	QSlider::focusInEvent(e);
	emit sig_slider_got_focus();
}

void SayonaraSlider::focusOutEvent(QFocusEvent* e){
	QSlider::focusOutEvent(e);
	emit sig_slider_lost_focus();
}

void SayonaraSlider::mousePressEvent(QMouseEvent* e)
{
	this->setSliderDown(true);

	int new_val = get_val_from_pos(e->pos());
	setValue(new_val);
}

void SayonaraSlider::mouseReleaseEvent(QMouseEvent* e)
{
	int new_val = get_val_from_pos(e->pos());
	setValue(new_val);

	this->setSliderDown(false);
}

void SayonaraSlider::mouseMoveEvent(QMouseEvent* e) {

	int new_val = get_val_from_pos(e->pos());

	if(this->isSliderDown()){
		setValue(new_val);
	}

	else{
		emit sig_slider_hovered(new_val);
	}
}

void SayonaraSlider::sliderChange(SliderChange change){
	QSlider::sliderChange(change);
}

int SayonaraSlider::get_val_from_pos(const QPoint& pos) const
{
	int percent;
	if(this->orientation() == Qt::Vertical){
		percent = 100 - (pos.y() * 100) / geometry().height();
	}

	else{
		percent = (pos.x() * 100) / geometry().width();
	}

	int range = this->maximum() - this->minimum();
	return  ( range * percent) / 100 + this->minimum();
}

