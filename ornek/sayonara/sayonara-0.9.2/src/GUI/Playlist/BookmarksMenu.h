/* BookmarksMenu.h */

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



#ifndef BOOKMARKS_ACTION_H
#define BOOKMARKS_ACTION_H

class MetaData;
class Bookmarks;
class Bookmark;

#include <QMenu>
#include <QWidget>
#include <QList>


class BookmarksMenu :
	public QMenu
{
	Q_OBJECT

signals:
	void sig_bookmark_pressed(quint32 time_sec);

private:
	Bookmarks*	_bookmarks=nullptr;

public:
	BookmarksMenu(QWidget* parent);
	virtual ~BookmarksMenu();

	void set_metadata(const MetaData& md);
	bool has_bookmarks() const;

private slots:
	void action_pressed();
	void bookmarks_changed(const QList<Bookmark>& bookmarks);

	
};


#endif
