/* TagTextInput.cpp */

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



#include "Helper/Helper.h"
#include "TagTextInput.h"
#include <QCursor>


TagTextInput::TagTextInput(QWidget* parent) :
	QLineEdit(parent),
	SayonaraClass()
{
	init_context_menu();

	REGISTER_LISTENER(Set::Player_Language, language_changed);
}


void TagTextInput::init_context_menu() 
{
    _context_menu = createStandardContextMenu();

//	_context_menu = new QMenu(this);
	_action_cvt_to_very_first_upper = new QAction("Convert to very first upper", _context_menu);
	_action_cvt_to_first_upper = new QAction("Convert to first upper", _context_menu);

	_context_menu->addSeparator();
	_context_menu->addAction(_action_cvt_to_very_first_upper);
	_context_menu->addAction(_action_cvt_to_first_upper);

	connect(_action_cvt_to_first_upper, &QAction::triggered, this, &TagTextInput::cvt_to_first_upper);
	connect(_action_cvt_to_very_first_upper, &QAction::triggered, this, &TagTextInput::cvt_to_very_first_upper);

}

void TagTextInput::contextMenuEvent(QContextMenuEvent* event){

	QPoint pos = event->pos();

	pos.setX(QCursor::pos().x());	
	pos.setY(QCursor::pos().y());	
	_context_menu->exec(pos);
}

void TagTextInput::cvt_to_first_upper(){
	QString text = this->text();
	text = Helper::cvt_str_to_first_upper(text);
	this->setText(text);
}

void TagTextInput::cvt_to_very_first_upper(){
	QString text = this->text();
	text = Helper::cvt_str_to_very_first_upper(text);
	this->setText(text);

}

void TagTextInput::language_changed(){
	_action_cvt_to_very_first_upper->setText(tr("Very first letter to upper case"));
	_action_cvt_to_first_upper->setText(tr("First letters to upper case"));
}
