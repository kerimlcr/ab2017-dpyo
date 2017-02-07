/* InfoDialogContainer.cpp */

/* Copyright (C) 2011-2015  Lucio Carreras
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



#include "InfoDialogContainer.h"
#include "GUI_InfoDialog.h"
#include "GUI/Helper/GUI_Helper.h"


InfoDialogContainer::InfoDialogContainer(){

}

InfoDialogContainer::~InfoDialogContainer(){

}

void InfoDialogContainer::info_dialog_closed()
{
	// we could delete the dialog here, but that's not really neccessary
}


void InfoDialogContainer::show_info(){
	if(init_dialog()){
		_info_dialog->show(GUI_InfoDialog::Tab::Info);
	}
}

void InfoDialogContainer::show_lyrics(){
	if(init_dialog()){
		_info_dialog->show(GUI_InfoDialog::Tab::Lyrics);
	}
}

void InfoDialogContainer::show_edit(){
	if(init_dialog()){
		_info_dialog->show(GUI_InfoDialog::Tab::Edit);
	}
}

bool InfoDialogContainer::init_dialog()
{
	if(!_info_dialog){
		_info_dialog = new GUI_InfoDialog(this, GUI::get_main_window());
	}

	_info_dialog->set_metadata(get_data_for_info_dialog(), get_metadata_interpretation());

	return _info_dialog->has_metadata();
}

