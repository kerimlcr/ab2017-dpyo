/* FileListView.h */

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



#ifndef FILELISTVIEW_H
#define FILELISTVIEW_H

#include "Helper/Settings/SayonaraClass.h"
#include "Helper/MetaData/MetaDataList.h"
#include "GUI/Helper/SearchableWidget/SearchableListView.h"

#include <QModelIndexList>
#include <QFileSystemModel>


class LibraryContextMenu;
class FileListView :
		public SearchableListView,
		private SayonaraClass

{

	Q_OBJECT

signals:
	void sig_info_clicked();
	void sig_delete_clicked();
	void sig_play_next_clicked();
	void sig_append_clicked();

public:
	FileListView(QWidget* parent=nullptr);

	QModelIndexList get_selected_rows() const;
	QFileSystemModel* get_model() const override;
	MetaDataList read_metadata() const;
	QStringList get_filelist() const;

private:
	LibraryContextMenu*	_context_menu=nullptr;
	QFileSystemModel*	_model=nullptr;

private:
	void mousePressEvent(QMouseEvent* event) override;
	void init_context_menu();

};


#endif // FILELISTVIEW_H
