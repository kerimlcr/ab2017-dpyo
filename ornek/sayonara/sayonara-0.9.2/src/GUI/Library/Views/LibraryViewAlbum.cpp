/* LibraryViewAlbum.cpp */

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



#include "LibraryViewAlbum.h"
#include "GUI/Library/Helper/DiscPopupMenu.h"

#include <QHeaderView>

LibraryViewAlbum::LibraryViewAlbum(QWidget *parent) :
	LibraryView(parent)
{
	connect(this, &QTableView::clicked, this, &LibraryViewAlbum::index_clicked);
}


void LibraryViewAlbum::rc_menu_show(const QPoint & p){
	delete_discmenu();

	LibraryView::rc_menu_show(p);
}


void LibraryViewAlbum::index_clicked(const QModelIndex &idx){

	if(idx.column() != COL_ALBUM_MULTI_DISC){
		return;
	}

	QModelIndexList selections = this->selectionModel()->selectedRows();
	if(selections.size() == 1){
		init_discmenu(idx);
		show_discmenu();
	}
}


/* where to show the popup */
void LibraryViewAlbum::calc_discmenu_point(QModelIndex idx){

	_discmenu_point = QCursor::pos();

	QRect box = this->geometry();
	box.moveTopLeft(this->parentWidget()->mapToGlobal(box.topLeft()));

	if(!box.contains(_discmenu_point)){
		_discmenu_point.setX(box.x() + (box.width() * 2) / 3);
		_discmenu_point.setY(box.y());

		QPoint dmp_tmp = parentWidget()->pos();
		dmp_tmp.setY(dmp_tmp.y() - this->verticalHeader()->sizeHint().height());

		while(idx.row() != indexAt(dmp_tmp).row()){
			  dmp_tmp.setY(dmp_tmp.y() + 10);
			  _discmenu_point.setY(_discmenu_point.y() + 10);
		}
	}
}

void LibraryViewAlbum::init_discmenu(QModelIndex idx){

	int row = idx.row();
	QList<quint8> discnumbers;
	delete_discmenu();

	if( !idx.isValid() ||
		(row > _discnumbers.size()) ||
		(row < 0) )
	{
		return;
	}

	discnumbers = _discnumbers[row];
	if(discnumbers.size() < 2) {
		return;
	}

	calc_discmenu_point(idx);

	_discmenu = new DiscPopupMenu(this, discnumbers);

	connect(_discmenu, &DiscPopupMenu::sig_disc_pressed, this, &LibraryViewAlbum::sig_disc_pressed);
}


void LibraryViewAlbum::delete_discmenu(){

	if(!_discmenu) {
		return;
	}

	_discmenu->hide();
	_discmenu->close();

	disconnect(_discmenu, &DiscPopupMenu::sig_disc_pressed, this, &LibraryViewAlbum::sig_disc_pressed);

	_discmenu->deleteLater();
	_discmenu = nullptr;
}


void LibraryViewAlbum::show_discmenu(){

	if(!_discmenu) return;

	_discmenu->popup(_discmenu_point);
}

