/* DirectoryTreeView.h */

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



#ifndef DIRECTORYTREEVIEW_H
#define DIRECTORYTREEVIEW_H

#include "Helper/Settings/SayonaraClass.h"
#include "Helper/MetaData/MetaDataList.h"

#include <QTreeView>
#include <QModelIndexList>

class LibraryContextMenu;
class AbstractSearchFileTreeModel;
class IconProvider;

class DirectoryTreeView :
		public QTreeView,
		private SayonaraClass
{

	Q_OBJECT

signals:
	void sig_info_clicked();
	void sig_delete_clicked();
	void sig_play_next_clicked();
	void sig_append_clicked();

public:
	explicit DirectoryTreeView(QWidget* parent=nullptr);
	virtual ~DirectoryTreeView();

	AbstractSearchFileTreeModel* get_model() const;

	QModelIndexList		get_selected_rows() const;
	MetaDataList 		read_metadata() const;
	QStringList			get_filelist() const;

private:
	LibraryContextMenu*				_context_menu=nullptr;
	AbstractSearchFileTreeModel*	_model = nullptr;
	IconProvider*					_icon_provider = nullptr;

private:
	void mousePressEvent(QMouseEvent* event) override;
	void init_context_menu();


private slots:
	void _sl_library_path_changed();

};

#endif // DIRECTORYTREEVIEW_H
