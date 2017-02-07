/* GUI_Speed.cpp */

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

#include "GUI_Speed.h"
#include <QToolTip>
#include <QCursor>

GUI_Speed::GUI_Speed(QWidget *parent) :
	PlayerPluginInterface(parent),
	Ui::GUI_Speed()
{

}

void GUI_Speed::language_changed(){

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
}

void GUI_Speed::init_ui()
{
	setup_parent(this);


	bool active = _settings->get(Set::Engine_SpeedActive);
	float speed = _settings->get(Set::Engine_Speed) * 1.0f;
	int pitch = _settings->get(Set::Engine_Pitch) * 10;

	active_changed(active);

	sli_speed->setValue(speed * 100);
	sli_speed->setMouseTracking(true);

	speed_changed(speed * 100);

	cb_preserve_pitch->setChecked( _settings->get(Set::Engine_PreservePitch));

	sli_pitch->setValue(pitch);
	sli_pitch->setMouseTracking(true);
	pitch_changed(pitch);


	connect(sli_speed, &QSlider::valueChanged, this, &GUI_Speed::speed_changed);
	connect(cb_active, &QCheckBox::toggled, this, &GUI_Speed::active_changed);
	connect(cb_preserve_pitch, &QCheckBox::toggled, this, &GUI_Speed::preserve_pitch_changed);
	connect(sli_pitch, &QSlider::valueChanged, this, &GUI_Speed::pitch_changed);
	connect(btn_revert_speed, &QPushButton::clicked, this, &GUI_Speed::revert_speed_clicked);
	connect(btn_revert_pitch, &QPushButton::clicked, this, &GUI_Speed::revert_pitch_clicked);
	connect(sli_speed, &SayonaraSlider::sig_slider_hovered, this, &GUI_Speed::speed_hovered);
	connect(sli_pitch, &SayonaraSlider::sig_slider_hovered, this, &GUI_Speed::pitch_hovered);

	REGISTER_LISTENER(SetNoDB::Pitch_found, _sl_pitch_found_changed);
}


QString GUI_Speed::get_name() const
{
	return "Speed";
}

QString GUI_Speed::get_display_name() const
{
	return tr("Speed") + "/" + tr("Pitch");
}


void GUI_Speed::speed_changed(int val) {

	float val_f = val / 100.0f;

	lab_speed->setText(QString::number(val_f, 'f', 2));
	_settings->set(Set::Engine_Speed, sli_speed->value() / 100.0f);
}


void GUI_Speed::active_changed(bool active) {

	cb_active->setChecked(active);

	sli_speed->setEnabled( active);
	btn_revert_speed->setEnabled(active);
	sli_pitch->setEnabled(active);
	cb_preserve_pitch->setEnabled(active);
	btn_revert_pitch->setEnabled(active);

	_settings->set(Set::Engine_SpeedActive, active);
}

void GUI_Speed::preserve_pitch_changed(bool enabled)
{
	_settings->set(Set::Engine_PreservePitch, enabled);
}

void GUI_Speed::pitch_changed(int pitch)
{
	pitch = pitch / 10;
	_settings->set(Set::Engine_Pitch, pitch);
	lab_pitch->setText(QString::number(pitch) + " Hz");
}

void GUI_Speed::revert_speed_clicked()
{
	sli_speed->setValue(100);
	//speed_changed(100);
}

void GUI_Speed::revert_pitch_clicked()
{
	sli_pitch->setValue(4400);
	//pitch_changed(440);
}

void GUI_Speed::pitch_hovered(int val)
{
	QToolTip::showText( QCursor::pos(), QString::number(val / 10));
}

void GUI_Speed::speed_hovered(int val)
{
	QToolTip::showText( QCursor::pos(), QString::number((float) (val / 100.0f)));
}

void GUI_Speed::_sl_pitch_found_changed()
{
	bool pitch_found = _settings->get(SetNoDB::Pitch_found);
	if(!pitch_found){
		cb_active->setChecked(false);
		active_changed(false);
		cb_active->setToolTip(tr("%1 not found").arg("gstreamer bad plugins") + "<br />" +
							  tr("%1 not found").arg("libsoundtouch"));
	}

	else{
		cb_active->setToolTip("");
	}

	cb_active->setEnabled(pitch_found);
}
