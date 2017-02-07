/* GUI_DirectoryWidget.h */

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



#ifndef GUI_DIRECTORYWIDGET_H
#define GUI_DIRECTORYWIDGET_H

#include "GUI/DirectoryWidget/ui_GUI_DirectoryWidget.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"
#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"
#include "GUI/InfoDialog/InfoDialogContainer.h"

#include <QWidget>
#include <QFileSystemModel>
#include <QShowEvent>
#include <QMouseEvent>

class LocalLibrary;
class IconProvider;
class LocalLibrary;
class AbstractSearchFileTreeModel;
class LibraryContextMenu;

class GUI_DirectoryWidget :
		public SayonaraWidget,
		public InfoDialogContainer,
		private Ui::GUI_DirectoryWidget
{
	Q_OBJECT

public:
	explicit GUI_DirectoryWidget(QWidget *parent = 0);
	~GUI_DirectoryWidget();

	QComboBox*	get_libchooser();


private:

	enum SelectedWidget
	{
		None=0,
		Dirs,
		Files
	} _selected_widget;

	IconProvider*						_icon_provider=nullptr;
	QFileSystemModel*					_file_model=nullptr;
	LocalLibrary*						_local_library=nullptr;
	AbstractSearchFileTreeModel*		_dir_model=nullptr;

	QModelIndex							_found_idx;
	QString								_search_term;
	QStringList							_found_strings;


private:
	void showEvent(QShowEvent* e) override;
	void init_shortcuts();

private slots:
	void search_button_clicked();
	void search_term_changed(const QString& term);
	void directory_loaded(const QString& path);
	void files_loaded(const QString& path);
	void init_dir_view();
	void dir_clicked(QModelIndex idx);
	void dir_pressed(QModelIndex idx);
	void file_dbl_clicked(QModelIndex idx);
	void file_pressed(QModelIndex idx);

	void dir_append_clicked();
	void dir_play_next_clicked();
	void dir_delete_clicked();

	void file_append_clicked();
	void file_play_next_clicked();
	void file_delete_clicked();

	// InfoDialogContainer interface
protected:
	MetaDataList::Interpretation get_metadata_interpretation() const;
	MetaDataList get_data_for_info_dialog() const;
};



// for showing up in library tree
class DirectoryLibraryContainer :
		public LibraryContainerInterface
{

	Q_OBJECT

private:
	GUI_DirectoryWidget*		ui=nullptr;

public:

	DirectoryLibraryContainer(QObject* parent=nullptr);

	QIcon				get_icon() const override;
	QString				get_display_name() const override;
	QString				get_name() const override;
	QWidget*			get_ui() const override;
	QComboBox*			get_libchooser() override;
	void				init_ui() override;

};



#endif // GUI_DIRECTORYWIDGET_H
