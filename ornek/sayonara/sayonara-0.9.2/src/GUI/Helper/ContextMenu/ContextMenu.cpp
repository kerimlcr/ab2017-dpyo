/* ContextMenu.cpp */

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

#include "ContextMenu.h"
#include "GUI/Helper/IconLoader/IconLoader.h"
#include "GUI/Helper/GUI_Helper.h"

#include <algorithm>

ContextMenu::ContextMenu(QWidget *parent) :
	QMenu(parent)
{
	_timer = new QTimer(this);
	_icon_loader = IconLoader::getInstance();

	_action_open = new QAction(GUI::get_icon("open"), tr("Open"), this);
	_action_new = new QAction(GUI::get_icon("new"), tr("New"), this);
	_action_undo = new QAction(GUI::get_icon("undo"), tr("Undo"), this);
	_action_default = new QAction(GUI::get_icon("undo"), tr("Default"), this);
	_action_save = new QAction(GUI::get_icon("save"), tr("Save"), this);
	_action_save_as = new QAction(GUI::get_icon("save_as"), tr("Save as"), this);
	_action_rename = new QAction(GUI::get_icon("edit"), tr("Rename"), this);
	_action_delete = new QAction(GUI::get_icon("delete"), tr("Delete"), this);


	_actions << addSeparator()
			<< _action_new
			<< _action_open
			<< _action_save
			<< _action_save_as
			<< _action_rename
			<< addSeparator()
			<< _action_undo
			<< _action_default
			<< addSeparator()
			<< _action_delete
			<< addSeparator();

	this->addActions(_actions);

	for(QAction* action : this->actions()){
		action->setVisible(false);
	}

	connect(_action_open, &QAction::triggered, this, &ContextMenu::sig_open);
	connect(_action_new, &QAction::triggered, this, &ContextMenu::sig_new);
	connect(_action_undo, &QAction::triggered, this, &ContextMenu::sig_undo);
	connect(_action_default, &QAction::triggered, this, &ContextMenu::sig_default);
	connect(_action_save, &QAction::triggered, this, &ContextMenu::sig_save);
	connect(_action_save_as, &QAction::triggered, this, &ContextMenu::sig_save_as);
	connect(_action_rename, &QAction::triggered, this, &ContextMenu::sig_rename);
	connect(_action_delete, &QAction::triggered, this, &ContextMenu::sig_delete);

	REGISTER_LISTENER(Set::Player_Language, language_changed);
	REGISTER_LISTENER(Set::Player_Style, skin_changed);
}

void ContextMenu::language_changed(){
	 _action_new->setText(tr("New"));
	 _action_open->setText(tr("Open"));
	 _action_save->setText(tr("Save"));
	 _action_save_as->setText(tr("Save as"));
	 _action_rename->setText(tr("Rename"));
	 _action_undo->setText(tr("Undo"));
	 _action_default->setText(tr("Default"));
	 _action_delete->setText(tr("Delete"));
}

void ContextMenu::skin_changed()
{
	_action_open->setIcon(_icon_loader->get_icon( "document-open", "open") );
	_action_new->setIcon(_icon_loader->get_icon( "document-new", "new") );
	_action_undo->setIcon(_icon_loader->get_icon( "edit-undo", "undo") );
	_action_default->setIcon(_icon_loader->get_icon( "edit-undo", "undo") );
	_action_save->setIcon(_icon_loader->get_icon( "document-save", "save") );
	_action_save_as->setIcon(_icon_loader->get_icon( "document-save-as", "save_as") );
	_action_rename->setIcon(_icon_loader->get_icon( "accessories-text-editor", "edit") );
	_action_delete->setIcon(_icon_loader->get_icon( "edit-delete", "delete") );
}

void ContextMenu::register_action(QAction *action){
	_actions << action;

	clear();
	addActions(_actions);
}

void ContextMenu::show_actions(ContextMenuEntries entries){

	_action_new->setVisible(entries & ContextMenu::EntryNew);
	_action_open->setVisible(entries & ContextMenu::EntryOpen);
	_action_undo->setVisible(entries & ContextMenu::EntryUndo);
	_action_default->setVisible(entries & ContextMenu::EntryDefault);
	_action_save->setVisible(entries & ContextMenu::EntrySave);
	_action_save_as->setVisible(entries & ContextMenu::EntrySaveAs);
	_action_rename->setVisible(entries & ContextMenu::EntryRename);
	_action_delete->setVisible(entries & ContextMenu::EntryDelete);

}

void ContextMenu::show_action(ContextMenu::Entry entry, bool visible)
{
	ContextMenuEntries entries = this->get_entries();
	if(visible){
		entries |= entry;
	}

	else{
		entries &= ~(entry);
	}

	show_actions(entries);
}


bool ContextMenu::has_actions() {

	return std::any_of(_actions.begin(), _actions.end(), [](QAction* a){
		return a->isVisible();
	});
}

ContextMenuEntries ContextMenu::get_entries() const
{
	ContextMenuEntries entries = ContextMenu::EntryNone;

	if(_action_new->isVisible()){
		entries |= ContextMenu::EntryNew;
	}

	if(_action_delete->isVisible()){
		entries |= ContextMenu::EntryDelete;
	}
	if(_action_open->isVisible()){
		entries |= ContextMenu::EntryOpen;
	}
	if(_action_rename->isVisible()){
		entries |= ContextMenu::EntryRename;
	}
	if(_action_save->isVisible()){
		entries |= ContextMenu::EntrySave;
	}
	if(_action_save_as->isVisible()){
		entries |= ContextMenu::EntrySaveAs;
	}
	if(_action_undo->isVisible()){
		entries |= ContextMenu::EntryUndo;
	}
	if(_action_default->isVisible()){
		entries |= ContextMenu::EntryDefault;
	}

	return entries;
}



void ContextMenu::show_all(){
	for(QAction* action: _actions){
		action->setVisible(true);
	}
}

void ContextMenu::showEvent(QShowEvent* e){
	QMenu::showEvent(e);
	for(QAction* action: _actions){
		action->setDisabled(true);
	}

	QTimer::singleShot(300, this, SLOT(timed_out()));
}

void ContextMenu::timed_out(){
	for(QAction* action: _actions){
		action->setDisabled(false);
	}
}


