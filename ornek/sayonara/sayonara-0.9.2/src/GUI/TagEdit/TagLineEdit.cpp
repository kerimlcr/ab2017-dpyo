/* TagLineEdit.cpp */

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



#include "TagLineEdit.h"

TagLineEdit::TagLineEdit(QWidget* parent) :
	QLineEdit(parent)
{
	this->setReadOnly(true);
}


void TagLineEdit::focusInEvent(QFocusEvent* e){
	_text_selection.reset();
	QLineEdit::focusInEvent(e);
}

void TagLineEdit::focusOutEvent(QFocusEvent* e){

	QString selected_text = this->selectedText();
	if(selected_text.isEmpty()){
		_text_selection.reset();
	}

	else{
		_text_selection.selection_start = this->selectionStart();
		_text_selection.selection_size = selected_text.size();
	}

	QLineEdit::focusOutEvent(e);
}



TextSelection TagLineEdit::get_text_selection() const
{
	return _text_selection;
}

