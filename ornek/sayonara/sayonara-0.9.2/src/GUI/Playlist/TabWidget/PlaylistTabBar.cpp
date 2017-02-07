/* PlaylistTabBar.cpp */

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



#include "PlaylistTabBar.h"
#include "PlaylistTabMenu.h"
#include "GUI/Helper/CustomMimeData.h"
#include "GUI/Helper/Shortcuts/ShortcutHandler.h"

#include "Helper/DirectoryReader/DirectoryReader.h"

#include <QShortcut>


PlaylistTabBar::PlaylistTabBar(QWidget *parent) :
	QTabBar(parent),
	ShortcutWidget()
{
	_menu = new PlaylistTabMenu(this);
	this->setDrawBase(false);
	this->setAcceptDrops(true);

	_tab_before_dd = -1;
	_drag_origin_tab = -1;


	init_shortcuts();

	connect(_menu, &PlaylistTabMenu::sig_open_file_clicked, this, &PlaylistTabBar::open_file_pressed);
	connect(_menu, &PlaylistTabMenu::sig_open_dir_clicked, this, &PlaylistTabBar::open_dir_pressed);
	connect(_menu, &PlaylistTabMenu::sig_rename_clicked, this, &PlaylistTabBar::rename_pressed);
	connect(_menu, &PlaylistTabMenu::sig_reset_clicked, this, &PlaylistTabBar::reset_pressed);
	connect(_menu, &PlaylistTabMenu::sig_save_clicked, this, &PlaylistTabBar::save_pressed);
	connect(_menu, &PlaylistTabMenu::sig_save_as_clicked, this, &PlaylistTabBar::save_as_pressed);
	connect(_menu, &PlaylistTabMenu::sig_clear_clicked, this, &PlaylistTabBar::clear_pressed);
	connect(_menu, &PlaylistTabMenu::sig_delete_clicked, this, &PlaylistTabBar::delete_pressed);
	connect(_menu, &PlaylistTabMenu::sig_close_clicked, this, &PlaylistTabBar::close_pressed);
	connect(_menu, &PlaylistTabMenu::sig_close_others_clicked, this, &PlaylistTabBar::close_others_pressed);
}


PlaylistTabBar::~PlaylistTabBar(){

}


void PlaylistTabBar::save_pressed(){
	emit sig_tab_save(currentIndex());
}

void PlaylistTabBar::save_as_pressed(){
	int cur_idx = currentIndex();
	QString cur_text = tabText(cur_idx);

	QString name = QInputDialog::getText(
				this,
				tr("Save as..."),
				cur_text + ": " + tr("Save as"));

	emit sig_tab_save_as(currentIndex(), name);
}

void PlaylistTabBar::open_file_pressed(){
	emit sig_open_file(currentIndex());
}

void PlaylistTabBar::open_dir_pressed(){
	emit sig_open_dir(currentIndex());
}

void PlaylistTabBar::clear_pressed()
{
	emit sig_tab_clear(currentIndex());
}

void PlaylistTabBar::delete_pressed(){
	emit sig_tab_delete(currentIndex());
}

void PlaylistTabBar::close_pressed(){
	emit tabCloseRequested(this->currentIndex());
}

void PlaylistTabBar::reset_pressed(){
	emit sig_tab_reset(currentIndex());
}

void PlaylistTabBar::rename_pressed(){
	int cur_idx = currentIndex();
	QString cur_text = tabText(cur_idx);

	QString name = QInputDialog::getText(
				this,
				tr("Rename"),
				cur_text + ": " + tr("Rename"));

	if(name.compare(cur_text) == 0){
		return;
	}

	emit sig_tab_rename(currentIndex(), name);
}


void PlaylistTabBar::close_others_pressed(){
	int my_tab = currentIndex();
	int i=0;

	while( count() > 2){
		if(i < my_tab){
			tabCloseRequested(0);
		}

		else if(i == my_tab){

		}

		else{
			tabCloseRequested(1);
		}

		i++;
	}
}

void PlaylistTabBar::mousePressEvent(QMouseEvent* e){

	int idx = this->tabAt(e->pos());

	if(idx == this->count() - 1){
		emit sig_add_tab_clicked();
		return;
	}

	else{
		this->setCurrentIndex(idx);
	}

	if(e->button() == Qt::RightButton){
		_menu->exec(e->globalPos());
	}

	else if(e->button() == Qt::MiddleButton){
		if(this->count() > 2){
			emit tabCloseRequested(idx);
		}
	}
}

void PlaylistTabBar::wheelEvent(QWheelEvent* e)
{
	QTabBar::wheelEvent(e);
	if(this->currentIndex() == this->count() - 1 &&
	   this->count() > 1)
	{
		this->setCurrentIndex(this->count() - 2);
	}
}

void PlaylistTabBar::init_shortcuts()
{
	ShortcutHandler* sch = ShortcutHandler::getInstance();
	Shortcut sc1 = sch->add(Shortcut(this, "add_tab", tr("Add tab"), "Ctrl+t"));
	Shortcut sc2 = sch->add(Shortcut(this, "close_tab", tr("Close tab"), "Ctrl+w"));

	sc1.create_qt_shortcut(this, this, SIGNAL(sig_add_tab_clicked()));
	sc2.create_qt_shortcut(this, this, SLOT(close_pressed()));
}


QString PlaylistTabBar::get_shortcut_text(const QString& shortcut_identifier) const
{
	if(shortcut_identifier == "add_tab"){
		return tr("Add tab");
	}

	if(shortcut_identifier == "close_tab"){
		return tr("Close tab");
	}

	return "";
}



void PlaylistTabBar::show_menu_items(PlaylistMenuEntries entries){
	_menu->show_menu_items(entries);
}

void PlaylistTabBar::setTabsClosable(bool b){
	QTabBar::setTabsClosable(b);
	_menu->show_close(b);
}

bool PlaylistTabBar::was_drag_from_playlist() const
{
	return _drag_from_playlist;
}

int PlaylistTabBar::get_drag_origin_tab() const
{
	return _drag_origin_tab;
}



void PlaylistTabBar::dragEnterEvent(QDragEnterEvent* e)
{
	QString object_name;
	if(e->source()){
		object_name = e->source()->objectName();
	}

	_drag_origin_tab = -1;
	_drag_from_playlist = object_name.contains("playlist_view");

	if(!_drag_from_playlist){
		_tab_before_dd = -1;
	}

	else if(_tab_before_dd < 0){
		_tab_before_dd = currentIndex();
	}

	e->accept();

	int tab = tabAt(e->pos());
	this->setCurrentIndex(tab);
}

void PlaylistTabBar::dragMoveEvent(QDragMoveEvent* e)
{
	e->accept();

	int tab = tabAt(e->pos());
	this->setCurrentIndex(tab);
}

void PlaylistTabBar::dragLeaveEvent(QDragLeaveEvent* e)
{
	if(_tab_before_dd >= 0 && currentIndex() == count() - 1){
		this->setCurrentIndex(_tab_before_dd);
		_tab_before_dd = -1;
	}

	e->accept();
}

void PlaylistTabBar::dropEvent(QDropEvent* e)
{
	e->accept();
	int tab = this->tabAt(e->pos());

	_drag_origin_tab = _tab_before_dd;

	if(_tab_before_dd >= 0 && currentIndex() == count() - 1){
		this->setCurrentIndex(_tab_before_dd);
	}

	_tab_before_dd = -1;

	const QMimeData* mime_data = e->mimeData();
	if(!mime_data){
		return;
	}

	const CustomMimeData* cmd = dynamic_cast<const CustomMimeData*>(mime_data);
	if(!cmd){
		if(!mime_data->hasUrls()){
			return;
		}

		MetaDataList v_md;
		DirectoryReader dir_reader;
		QList<QUrl> urls = mime_data->urls();
		QStringList files;
		for(const QUrl& url : urls){
			files << url.toLocalFile();
		}

		v_md = dir_reader.get_md_from_filelist(files);
		emit sig_metadata_dropped(tab, v_md);
		return;
	}

	if(!cmd->hasMetaData()){
		return;
	}

	MetaDataList v_md = cmd->getMetaData();

	emit sig_metadata_dropped(tab, v_md);
}



