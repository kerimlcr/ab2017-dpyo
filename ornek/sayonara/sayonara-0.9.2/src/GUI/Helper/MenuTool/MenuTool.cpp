/* MenuTool.cpp */

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

#include "MenuTool.h"

MenuToolButton::MenuToolButton(QWidget *parent) :
	MenuButton(parent)
{
	_menu = new ContextMenu(this);

	connect(_menu, &ContextMenu::sig_open, this,  &MenuToolButton::sig_open);
	connect(_menu, &ContextMenu::sig_new, this, &MenuToolButton::sig_new);
	connect(_menu, &ContextMenu::sig_undo, this, &MenuToolButton::sig_undo);
	connect(_menu, &ContextMenu::sig_default, this, &MenuToolButton::sig_default);
	connect(_menu, &ContextMenu::sig_save, this, &MenuToolButton::sig_save);
	connect(_menu, &ContextMenu::sig_save_as, this, &MenuToolButton::sig_save_as);
	connect(_menu, &ContextMenu::sig_rename, this, &MenuToolButton::sig_rename);
	connect(_menu, &ContextMenu::sig_delete, this, &MenuToolButton::sig_delete);

	prove_enabled();
}

MenuToolButton::~MenuToolButton()
{

}


void MenuToolButton::register_action(QAction *action)
{
	_menu->register_action(action);
}


void MenuToolButton::show_menu(QPoint pos)
{
	MenuButton::show_menu(pos);

	this->setDisabled(true);
	_menu->popup(pos);
	this->setEnabled(true);
}


bool MenuToolButton::prove_enabled()
{
	bool enabled = _menu->has_actions();
	this->setEnabled(enabled);
	return enabled;
}

void MenuToolButton::show_all(){
	_menu->show_all();
	prove_enabled();
}

void MenuToolButton::show_action(ContextMenu::Entry entry, bool visible){
	_menu->show_action(entry, visible);
	prove_enabled();
}

void MenuToolButton::show_actions(ContextMenuEntries entries)
{
	_menu->show_actions(entries);
	prove_enabled();
}

ContextMenuEntries MenuToolButton::get_entries() const
{
	return _menu->get_entries();
}
