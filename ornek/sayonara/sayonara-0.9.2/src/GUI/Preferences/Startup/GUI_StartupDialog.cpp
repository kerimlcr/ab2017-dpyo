/* GUI_StartupDialog.cpp */

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



#include "GUI_StartupDialog.h"

GUI_StartupDialog::GUI_StartupDialog(QWidget *parent) :
	PreferenceWidgetInterface(parent),
	Ui::GUI_StartupOptions()
{

}

GUI_StartupDialog::~GUI_StartupDialog() {

}

void GUI_StartupDialog::init_ui()
{
	setup_parent(this);

	revert();

}

void GUI_StartupDialog::language_changed() {

	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
	PreferenceWidgetInterface::language_changed();
}


void GUI_StartupDialog::commit() {


}

void GUI_StartupDialog::revert() {



}

QString GUI_StartupDialog::get_action_name() const
{
	return tr("Start up");
}



