/* GUI_DirectoryWidget.cpp */

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

#include "GUI_DirectoryWidget.h"

#include "GUI/Helper/SearchableWidget/SearchableFileTreeView.h"
#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"
#include "GUI/Helper/Message/GlobalMessage.h"
#include "GUI/Helper/GUI_Helper.h"
#include "GUI/Helper/IconLoader/IconLoader.h"

#include "Components/Library/LocalLibrary.h"
#include "Components/Library/LibraryNamespaces.h"
#include "Components/Playlist/PlaylistHandler.h"

#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Helper/FileHelper.h"
#include "DirectoryDelegate.h"

#include <QItemSelectionModel>
#include <QApplication>
#include <QMouseEvent>



GUI_DirectoryWidget::GUI_DirectoryWidget(QWidget *parent) :
	SayonaraWidget(parent),
	InfoDialogContainer(),
	Ui::GUI_DirectoryWidget()
{
	setupUi(this);

	_selected_widget = SelectedWidget::None;

	_local_library = LocalLibrary::getInstance();
	_dir_model = tv_dirs->get_model();
	_file_model = lv_files->get_model();

	connect(tv_dirs, &QTreeView::clicked, this, &GUI_DirectoryWidget::dir_clicked);
	connect(tv_dirs, &QTreeView::pressed, this, &GUI_DirectoryWidget::dir_pressed);
	connect(tv_dirs, &QTreeView::doubleClicked, this, &GUI_DirectoryWidget::dir_clicked);

	connect(lv_files, &QListView::doubleClicked, this, &GUI_DirectoryWidget::file_dbl_clicked);
	connect(lv_files, &QListView::pressed, this, &GUI_DirectoryWidget::file_pressed);

	connect(btn_search, &QPushButton::clicked, this, &GUI_DirectoryWidget::search_button_clicked);
	connect(le_search, &QLineEdit::returnPressed, this, &GUI_DirectoryWidget::search_button_clicked);
	connect(le_search, &QLineEdit::textChanged, this, &GUI_DirectoryWidget::search_term_changed);

	connect(tv_dirs, &DirectoryTreeView::sig_info_clicked, this, [=](){
		_selected_widget = SelectedWidget::Dirs;
		show_info();
	});

	connect(tv_dirs, &DirectoryTreeView::sig_append_clicked, this, &GUI_DirectoryWidget::dir_append_clicked);
	connect(tv_dirs, &DirectoryTreeView::sig_play_next_clicked, this, &GUI_DirectoryWidget::dir_play_next_clicked);
	connect(tv_dirs, &DirectoryTreeView::sig_delete_clicked, this, &GUI_DirectoryWidget::dir_delete_clicked);

	connect(lv_files, &FileListView::sig_info_clicked, this, [=](){
		_selected_widget = SelectedWidget::Files;
		show_info();
	});

	connect(lv_files, &FileListView::sig_append_clicked, this, &GUI_DirectoryWidget::file_append_clicked);
	connect(lv_files, &FileListView::sig_play_next_clicked, this, &GUI_DirectoryWidget::file_play_next_clicked);
	connect(lv_files, &FileListView::sig_delete_clicked, this, &GUI_DirectoryWidget::file_delete_clicked);

	init_shortcuts();
}

GUI_DirectoryWidget::~GUI_DirectoryWidget()
{

}

QComboBox* GUI_DirectoryWidget::get_libchooser(){
	return combo_libchooser;
}

MetaDataList::Interpretation GUI_DirectoryWidget::get_metadata_interpretation() const
{
	return MetaDataList::Interpretation::Tracks;
}

MetaDataList GUI_DirectoryWidget::get_data_for_info_dialog() const
{
	MetaDataList v_md;

	switch(_selected_widget)
	{
		case SelectedWidget::Dirs:
			return tv_dirs->read_metadata();
		case SelectedWidget::Files:
			return lv_files->read_metadata();
		default:
			return v_md;
	}
}


void GUI_DirectoryWidget::dir_clicked(QModelIndex idx){

	QString dir = _dir_model->fileInfo(idx).absoluteFilePath();
	QModelIndex tgt_idx = _file_model->setRootPath(dir);
	lv_files->setRootIndex(tgt_idx);
}

void GUI_DirectoryWidget::dir_append_clicked(){
	MetaDataList v_md = tv_dirs->read_metadata();
	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->append_tracks(v_md, plh->get_current_idx());
}

void GUI_DirectoryWidget::dir_play_next_clicked(){
	MetaDataList v_md = tv_dirs->read_metadata();
	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->play_next(v_md);
}

void GUI_DirectoryWidget::dir_delete_clicked(){
	GlobalMessage* gm = GlobalMessage::getInstance();
	GlobalMessage::Answer answer = gm->question(tr("Really?"));

	if(answer != GlobalMessage::Answer::Yes){
		return;
	}

	QStringList files = tv_dirs->get_filelist();
	MetaDataList v_md = tv_dirs->read_metadata();
	_local_library->delete_tracks(v_md, Library::TrackDeletionMode::OnlyLibrary);
	sp_log(Log::Debug) << "Delete clicked: " << files;
	Helper::File::delete_files(files);
}


void GUI_DirectoryWidget::file_append_clicked()
{
	MetaDataList v_md = lv_files->read_metadata();
	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->append_tracks(v_md, plh->get_current_idx());
}

void GUI_DirectoryWidget::file_play_next_clicked()
{
	MetaDataList v_md = lv_files->read_metadata();
	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->play_next(v_md);
}

void GUI_DirectoryWidget::file_delete_clicked()
{
	GlobalMessage* gm = GlobalMessage::getInstance();
	GlobalMessage::Answer answer = gm->question(tr("Really?"));

	if(answer != GlobalMessage::Answer::Yes){
		return;
	}

	QStringList files = lv_files->get_filelist();
	MetaDataList v_md = lv_files->read_metadata();
	_local_library->delete_tracks(v_md, Library::TrackDeletionMode::OnlyLibrary);
	Helper::File::delete_files(files);
}



void GUI_DirectoryWidget::dir_pressed(QModelIndex idx)
{
	Q_UNUSED(idx)

	Qt::MouseButtons buttons = QApplication::mouseButtons();
	if(buttons & Qt::MiddleButton){

		QStringList paths = tv_dirs->get_filelist();

		if(!paths.isEmpty()){
			_local_library->psl_prepare_tracks_for_playlist(paths, true);
		}
	}
}



void GUI_DirectoryWidget::file_pressed(QModelIndex idx)
{
	Q_UNUSED(idx)

	Qt::MouseButtons buttons = QApplication::mouseButtons();
	if(buttons & Qt::MiddleButton){

		QStringList paths = lv_files->get_filelist();

		if(!paths.isEmpty()){
			_local_library->psl_prepare_tracks_for_playlist(paths, true);
		}
	}
}



void GUI_DirectoryWidget::file_dbl_clicked(QModelIndex idx){
	QStringList paths;
	paths << _dir_model->filePath(idx);

	_local_library->psl_prepare_tracks_for_playlist(paths, false);
}


void GUI_DirectoryWidget::directory_loaded(const QString& path){

	Q_UNUSED(path)

	if(!_found_idx.isValid()){
		return;
	}

	tv_dirs->scrollTo(_found_idx, QAbstractItemView::PositionAtCenter);
	tv_dirs->selectionModel()->select(_found_idx, QItemSelectionModel::ClearAndSelect);
	dir_clicked(_found_idx);
}

void GUI_DirectoryWidget::files_loaded(const QString& path){

	QString searchstring = le_search->text();
	lv_files->clearSelection();

	if(searchstring.isEmpty()){
		return;
	}
	QModelIndex parent_idx = _file_model->index(path);
	int row_count = _file_model->rowCount(parent_idx);

	for(int row=0; row<row_count; row++){
		QModelIndex idx = _file_model->index(row, 0, parent_idx);
		QString text = idx.data().toString();
		if(text.contains(searchstring, Qt::CaseInsensitive)){
			lv_files->scrollTo(idx, QAbstractItemView::EnsureVisible);
			lv_files->selectionModel()->select(idx, QItemSelectionModel::Select);
		}
	}
}

void GUI_DirectoryWidget::search_button_clicked(){

	if(le_search->text().isEmpty()){
		return;
	}

	if(_search_term == le_search->text()){
		_found_idx = _dir_model->getNextRowIndexOf(_search_term, 0, QModelIndex());
	}
	else{
		_search_term = le_search->text();
		_found_idx = _dir_model->getFirstRowIndexOf(_search_term);
		btn_search->setText(tr("Search next"));
	}

	if(!_found_idx.isValid()){
		return;
	}

	if(_dir_model->canFetchMore(_found_idx)){
		_dir_model->fetchMore(_found_idx);
	}

	tv_dirs->scrollTo(_found_idx, QAbstractItemView::PositionAtCenter);
	tv_dirs->selectionModel()->select(_found_idx, QItemSelectionModel::ClearAndSelect);
	dir_clicked(_found_idx);
}

void GUI_DirectoryWidget::search_term_changed(const QString& term)
{
	if(term != _search_term && !term.isEmpty()){
		btn_search->setText(tr("Search"));
	}
}


void GUI_DirectoryWidget::init_dir_view(){

	connect(_dir_model, &AbstractSearchFileTreeModel::directoryLoaded,
			this, &GUI_DirectoryWidget::directory_loaded);

	connect(_file_model, &QFileSystemModel::directoryLoaded,
			this, &GUI_DirectoryWidget::files_loaded);

}


void GUI_DirectoryWidget::showEvent(QShowEvent* e){

	if(!_dir_model){
		init_dir_view();
	}

	e->accept();
}


void GUI_DirectoryWidget::init_shortcuts()
{
	new QShortcut(QKeySequence("Ctrl+f"), le_search, SLOT(setFocus()), nullptr, Qt::WindowShortcut);
	new QShortcut(QKeySequence("Esc"), le_search, SLOT(clear()), nullptr, Qt::WidgetShortcut);
}


DirectoryLibraryContainer::DirectoryLibraryContainer(QObject* parent) :
	LibraryContainerInterface(parent)
{

}

QString DirectoryLibraryContainer::get_name() const
{
	return "directories";
}

QString DirectoryLibraryContainer::get_display_name() const
{
	return tr("Directories");
}

QIcon DirectoryLibraryContainer::get_icon() const
{
	return IconLoader::getInstance()->get_icon("folder", "folder");
}

QWidget* DirectoryLibraryContainer::get_ui() const
{
	return static_cast<QWidget*>(ui);
}

QComboBox*DirectoryLibraryContainer::get_libchooser()
{
	return ui->get_libchooser();
}

void DirectoryLibraryContainer::init_ui()
{
	ui = new GUI_DirectoryWidget(nullptr);
}
