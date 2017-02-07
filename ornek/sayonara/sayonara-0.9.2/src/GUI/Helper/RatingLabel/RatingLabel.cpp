/* RatingLabel.cpp */

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


#include "Helper/globals.h"

#include "RatingLabel.h"
#include "Helper/Random/RandomGenerator.h"
#include "GUI/Helper/GUI_Helper.h"

#include <algorithm>

const int Offset_X = 3;


RatingLabel::RatingLabel(QWidget *parent, bool enabled) :
	QLabel(parent)
{
	RandomGenerator rnd;

	_rating = 0;
	_enabled = enabled;
	_parent = parent;
	_icon_size = 14;
	_pm_active = GUI::get_pixmap("star.png");
	_pm_inactive = GUI::get_pixmap("star_disabled.png");

    QSizePolicy p(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    this->setSizePolicy(p);
	this->setMouseTracking(true);
	this->setStyleSheet("background: transparent;");
}

RatingLabel::~RatingLabel() {
}


int RatingLabel::calc_rating(QPoint pos) const{

	double drating = (double) ((pos.x() * 1.0) / (_icon_size + 2.0)) + 0.5; 
	int rating = (int) (drating);

	rating=std::min(rating, 5);
	rating=std::max(rating, 0);

    return rating;
}

void RatingLabel::paintEvent(QPaintEvent *e) {
	QLabel::paintEvent(e);

    QPainter painter(this);

	painter.save(); 
	int offset_y = (this->height() - _icon_size) / 2;

 	painter.translate(rect().x() + Offset_X, rect().y() + offset_y );
    for(int rating = 0; rating < _rating; rating++) { 
		painter.drawPixmap(0, 0, _icon_size, _icon_size, _pm_active);
		painter.translate(_icon_size + 2, 0);
	}

	for(int rating = _rating; rating < 5; rating++) { 
		painter.drawPixmap(0, 0, _icon_size, _icon_size, _pm_inactive);
		painter.translate(_icon_size + 2, 0);
	}

	painter.restore();
}


void RatingLabel::mouseMoveEvent(QMouseEvent *e) {

	if(!_enabled) {
		return;
	}

	if(!hasFocus()){
		return;
	}

	int rating = calc_rating(e->pos());
	this->update_rating(rating);
}


void RatingLabel::mousePressEvent(QMouseEvent *e) {

	if(!_enabled) {
		return;
	}

    int rating = calc_rating(e->pos());
	update_rating(rating);
}


void RatingLabel::mouseReleaseEvent(QMouseEvent *e) {
	Q_UNUSED(e);

	if(!_enabled) {
		return;
	}

    emit sig_finished(true);
}


void RatingLabel::focusInEvent(QFocusEvent* e) {
	Q_UNUSED(e);

}

void RatingLabel::focusOutEvent(QFocusEvent* e) {
	Q_UNUSED(e);

	if(!_enabled) {
		return;
	}

	emit sig_finished(false);
}

void RatingLabel::update_rating(int rating) {
    _rating = rating;
    update();
}

void RatingLabel::increase() {

	_rating = std::min(5, _rating + 1);

    update_rating(_rating);
}

void RatingLabel::decrease() {

	_rating = std::max(_rating - 1, 0);

    update_rating(_rating);
}

void RatingLabel::set_rating(int rating) {

    _rating = rating;
    update();
}

int RatingLabel::get_rating() const{

    return _rating;
}

void RatingLabel::kill_yourself() {
    emit sig_finished(false);
}

