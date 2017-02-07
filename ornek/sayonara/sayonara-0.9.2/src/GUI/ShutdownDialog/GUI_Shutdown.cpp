/* GUI_Shutdown.cpp */

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



#include "GUI_Shutdown.h"
#include "Helper/Shutdown/Shutdown.h"

#ifdef WITH_SHUTDOWN

GUI_Shutdown::GUI_Shutdown(QWidget* parent):
	SayonaraDialog(parent),
	Ui::GUI_Shutdown()
{
	setupUi(this);

	connect(btn_ok, &QPushButton::clicked, this, &GUI_Shutdown::ok_clicked);
	connect(btn_cancel, &QPushButton::clicked, this, &GUI_Shutdown::cancel_clicked);
	connect(rb_after_finished, &QRadioButton::clicked, this, &GUI_Shutdown::rb_after_finished_clicked);
	connect(rb_after_minutes, &QRadioButton::clicked, this, &GUI_Shutdown::rb_after_minutes_clicked);
}


void GUI_Shutdown::ok_clicked(){

	if(sb_minutes->isEnabled()){
		quint64 msec = sb_minutes->value() * 60 * 1000;
		Shutdown::getInstance()->shutdown(msec);
	}

	else {
		Shutdown::getInstance()->shutdown_after_end();
	}


	close();
	emit sig_closed();
}

void GUI_Shutdown::cancel_clicked(){
	close();
	emit sig_closed();
}

void GUI_Shutdown::rb_after_finished_clicked(bool b){
	Q_UNUSED(b)
	rb_after_minutes->setChecked(false);
	sb_minutes->setEnabled(false);
}

void GUI_Shutdown::rb_after_minutes_clicked(bool b){
	Q_UNUSED(b)
	rb_after_minutes->setChecked(false);
	sb_minutes->setEnabled(true);
}

#endif // WITH_SHUTDOWN
