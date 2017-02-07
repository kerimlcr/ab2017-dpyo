
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



#ifndef PLAYLISTTABMENU_H
#define PLAYLISTTABMENU_H

#include "Helper/Settings/SayonaraClass.h"
#include "PlaylistMenuEntry.h"

#include <QMenu>
#include <QAction>

class IconLoader;

class PlaylistTabMenu :
		public QMenu,
		private SayonaraClass
{

	Q_OBJECT

signals:
	void sig_delete_clicked();
	void sig_save_clicked();
	void sig_save_as_clicked();
	void sig_close_clicked();
	void sig_close_others_clicked();
	void sig_reset_clicked();
	void sig_rename_clicked();
	void sig_clear_clicked();
	void sig_open_file_clicked();
	void sig_open_dir_clicked();

private:
	QAction*	_action_open_file=nullptr;
	QAction*	_action_open_dir=nullptr;
	QAction*	_action_delete=nullptr;
	QAction*	_action_save=nullptr;
	QAction*	_action_save_as=nullptr;
	QAction*	_action_reset=nullptr;
	QAction*	_action_close=nullptr;
	QAction*	_action_close_others=nullptr;
	QAction*	_action_rename=nullptr;
	QAction*	_action_clear=nullptr;

	IconLoader* _icon_loader;

private slots:
	void language_changed();
	void skin_changed();


public:
	PlaylistTabMenu(QWidget* parent=nullptr);

	~PlaylistTabMenu();

	void show_menu_items(PlaylistMenuEntries entries);
	void show_close(bool b);
};



#endif // PLAYLISTTABMENU_H
