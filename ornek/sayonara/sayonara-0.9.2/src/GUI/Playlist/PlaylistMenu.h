/* PlaylistMenu.h */

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



#ifndef PLAYLISTMENU_H
#define PLAYLISTMENU_H

#include "Helper/Settings/SayonaraClass.h"

#include <QMenu>
#include <QAction>
#include <QWidget>
#include <QTimer>
#include <QShowEvent>

class GUI_PlaylistEntryLook;

class PlaylistMenu :
		public QMenu,
		protected SayonaraClass
{

	Q_OBJECT

signals:
	void sig_shutdown(bool);

public:
	PlaylistMenu(QWidget* parent=nullptr);
	void set_shutdown(bool b);

private:
	QAction* _action_rep1=nullptr;
	QAction* _action_repAll=nullptr;
	QAction* _action_append=nullptr;
	QAction* _action_dynamic=nullptr;
	QAction* _action_shuffle=nullptr;
	QAction* _action_gapless=nullptr;
	QAction* _action_shutdown=nullptr;

	QTimer*  _timer=nullptr;

	GUI_PlaylistEntryLook* _entry_look_dialog=nullptr;

	void showEvent(QShowEvent* e) override;

private slots:

	void plm_changed();
	void change_plm();
	void timed_out();

	void language_changed();
};



#endif // PLAYLISTMENU_H
