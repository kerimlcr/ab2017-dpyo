/* LibraryViewAlbum.h */

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



#ifndef LIBRARYVIEWALBUM_H
#define LIBRARYVIEWALBUM_H

#include <QTimer>

#include "Helper/MetaData/Album.h"
#include "GUI/Library/Views/LibraryView.h"
#include <QModelIndex>

class DiscPopupMenu;
class LibraryViewAlbum : public LibraryView
{
	Q_OBJECT

signals:
	void sig_disc_pressed(int);

protected slots:
	void index_clicked(const QModelIndex& idx);

public:
	explicit LibraryViewAlbum(QWidget *parent=nullptr);


protected:
	QList< QList<quint8> >		_discnumbers;
	DiscPopupMenu*				_discmenu=nullptr;
	QPoint						_discmenu_point;

	virtual void calc_discmenu_point(QModelIndex idx);
	virtual void delete_discmenu();
	virtual void init_discmenu(QModelIndex idx);
	virtual void show_discmenu();

	virtual void rc_menu_show(const QPoint& p);

public:
	template <typename T, typename ModelType>
	void fill(const T& input_data){
		_discnumbers.clear();

		for(const Album& album: input_data){
			_discnumbers << album.discnumbers;
		}

		LibraryView::fill<T, ModelType>(input_data);
	}
};

#endif // LIBRARYVIEWALBUM_H
