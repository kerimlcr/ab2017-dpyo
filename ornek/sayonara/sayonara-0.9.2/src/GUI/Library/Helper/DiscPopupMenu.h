/* DiscPopupMenu.h */

/* Copyright (C) 2013  Lucio Carreras
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



#ifndef DISC_POPUP_MENU_H_
#define DISC_POPUP_MENU_H_

#include <QMenu>
#include <QAction>
#include <QList>
#include <QString>
#include <QDebug>
#include <QMouseEvent>


class DiscAction : public QAction {

Q_OBJECT

	signals:
		void sig_disc_pressed(int);

	private slots:
		void disc_hover();

	public:
		DiscAction(QWidget* parent, const QIcon& icon);
};


class DiscPopupMenu : public QMenu {

	Q_OBJECT

	signals:
		void sig_disc_pressed(int);

	private slots:
		void disc_pressed(int disc);


	public: 
		DiscPopupMenu(QWidget* parent, QList<quint8> discs);
		~DiscPopupMenu();

	protected: 
		void mouseReleaseEvent(QMouseEvent* e);

	private:
		QList<DiscAction*> _actions;

		void clean_up();
};





#endif
