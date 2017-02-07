
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



#include "PlaylistTabMenu.h"

#include "GUI/Helper/IconLoader/IconLoader.h"
#include "GUI/Helper/GUI_Helper.h"


PlaylistTabMenu::PlaylistTabMenu(QWidget* parent) :
	QMenu(parent),
	SayonaraClass()
{

	_action_open_file = new QAction(GUI::get_icon("play"), tr("Open File"), this);
	_action_open_dir = new QAction(GUI::get_icon("play"), tr("Open Folder"), this);
	_action_reset = new QAction(GUI::get_icon("undo"), tr("Reset"), this);
	_action_rename = new QAction(GUI::get_icon("edit"), tr("Rename"), this);
	_action_delete = new QAction(GUI::get_icon("delete"), tr("Delete"), this);
	_action_save = new QAction(GUI::get_icon("save"), tr("Save"), this);
	_action_save_as = new QAction(GUI::get_icon("save_as"), tr("Save as"), this);
	_action_clear = new QAction(GUI::get_icon("broom.png"), tr("Clear"), this);
	_action_close = new QAction(GUI::get_icon("power_off"), tr("Close"), this);
	_action_close_others = new QAction(GUI::get_icon("power_on"), tr("Close others"), this);

	_icon_loader = IconLoader::getInstance();


	QList<QAction*> actions;
	actions << _action_open_file
			<< _action_open_dir
			<< this->addSeparator()
			<< _action_reset
			<< this->addSeparator()
			<< _action_rename
			<< _action_save
			<< _action_save_as
			<< _action_delete
			<< this->addSeparator()
			<< _action_clear
			<< this->addSeparator()
			<< _action_close_others
			<< _action_close;

	this->addActions(actions);

	connect(_action_open_file, &QAction::triggered, this, &PlaylistTabMenu::sig_open_file_clicked);
	connect(_action_open_dir, &QAction::triggered, this, &PlaylistTabMenu::sig_open_dir_clicked);
	connect(_action_reset, &QAction::triggered, this, &PlaylistTabMenu::sig_reset_clicked);
	connect(_action_rename, &QAction::triggered, this, &PlaylistTabMenu::sig_rename_clicked);
	connect(_action_delete, &QAction::triggered, this, &PlaylistTabMenu::sig_delete_clicked);
	connect(_action_save, &QAction::triggered, this, &PlaylistTabMenu::sig_save_clicked);
	connect(_action_save_as, &QAction::triggered, this, &PlaylistTabMenu::sig_save_as_clicked);
	connect(_action_clear, &QAction::triggered, this, &PlaylistTabMenu::sig_clear_clicked);
	connect(_action_close, &QAction::triggered, this, &PlaylistTabMenu::sig_close_clicked);
	connect(_action_close_others, &QAction::triggered, this, &PlaylistTabMenu::sig_close_others_clicked);

	REGISTER_LISTENER(Set::Player_Language, language_changed);
	REGISTER_LISTENER(Set::Player_Style, skin_changed);
}

PlaylistTabMenu::~PlaylistTabMenu(){
	this->clear();
}

void PlaylistTabMenu::language_changed(){
	_action_open_file->setText(tr("Open File"));
	_action_open_dir->setText(tr("Open Folder"));
	_action_reset->setText(tr("Reset"));
	_action_rename->setText(tr("Rename"));
	_action_delete->setText(tr("Delete"));
	_action_save->setText(tr("Save"));
	_action_save_as->setText(tr("Save as"));
	_action_clear->setText(tr("Clear"));
	_action_close->setText(tr("Close"));
	_action_close_others->setText(tr("Close others"));
}

void PlaylistTabMenu::skin_changed(){
	_action_open_file->setIcon(_icon_loader->get_icon( "document-open", "play_small") );
	_action_open_dir->setIcon(_icon_loader->get_icon( "document-open", "play_small") );

	_action_reset->setIcon(_icon_loader->get_icon( "edit-undo", "undo") );
	_action_rename->setIcon(_icon_loader->get_icon( "accessories-text-editor", "edit") );
	_action_delete->setIcon(_icon_loader->get_icon( "edit-delete", "delete") );
	_action_save->setIcon(_icon_loader->get_icon( "document-save", "save") );
	_action_save_as->setIcon(_icon_loader->get_icon( "document-save-as", "save_as") );

	_action_clear->setIcon(_icon_loader->get_icon( "edit-clear", "broom.png") );
	_action_close->setIcon(_icon_loader->get_icon( "window-close", "power_off") );

	_action_close_others->setIcon(_icon_loader->get_icon( "window-close", "power_on") );

}

void PlaylistTabMenu::show_menu_items(PlaylistMenuEntries entries){
	_action_open_file->setVisible(entries & PlaylistMenuEntry::OpenFile);
	_action_open_dir->setVisible(entries & PlaylistMenuEntry::OpenDir);
	_action_reset->setVisible(entries & PlaylistMenuEntry::Reset);
	_action_rename->setVisible(entries & PlaylistMenuEntry::Rename);
	_action_delete->setVisible(entries & PlaylistMenuEntry::Delete);
	_action_save->setVisible(entries & PlaylistMenuEntry::Save);
	_action_save_as->setVisible(entries & PlaylistMenuEntry::SaveAs);
	_action_clear->setVisible(entries & PlaylistMenuEntry::Clear);
	_action_close->setVisible(entries & PlaylistMenuEntry::Close);
	_action_close_others->setVisible(entries & PlaylistMenuEntry::CloseOthers);

}


void PlaylistTabMenu::show_close(bool b){
	_action_close->setVisible(b);
	_action_close_others->setVisible(b);
}

