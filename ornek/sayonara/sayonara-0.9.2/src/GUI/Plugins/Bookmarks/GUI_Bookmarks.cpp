
/* GUI_Bookmarks.cpp */

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


#include "GUI_Bookmarks.h"
#include "GUI/Helper/Message/Message.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"

#include "Helper/Helper.h"

#include "Components/Bookmarks/Bookmarks.h"

#define TXT_NO_BOOKMARK "--:--"

GUI_Bookmarks::GUI_Bookmarks(QWidget *parent) :
	PlayerPluginInterface(parent),
	Ui::GUI_Bookmarks()
{
	_bookmarks = new Bookmarks(this);
}

QString GUI_Bookmarks::get_name() const
{
	return "Bookmarks";
}

QString GUI_Bookmarks::get_display_name() const
{
	return tr("Bookmarks");
}


void GUI_Bookmarks::language_changed(){

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
}


void GUI_Bookmarks::init_ui()
{
	if(is_ui_initialized()){
		return;
	}

	setup_parent(this);

	this->cb_bookmarks->setItemDelegate(new ComboBoxDelegate(this));

	connect(_bookmarks, &Bookmarks::sig_bookmarks_changed, this, &GUI_Bookmarks::bookmarks_changed);
	connect(_bookmarks, &Bookmarks::sig_next_changed, this, &GUI_Bookmarks::next_changed);
	connect(_bookmarks, &Bookmarks::sig_prev_changed, this, &GUI_Bookmarks::prev_changed);

	connect(btn_tool, &MenuToolButton::sig_new, this, &GUI_Bookmarks::new_clicked);
	connect(btn_tool, &MenuToolButton::sig_delete, this, &GUI_Bookmarks::del_clicked);
	connect(btn_bw, &QPushButton::clicked, this, &GUI_Bookmarks::prev_clicked);
	connect(btn_fw, &QPushButton::clicked, this, &GUI_Bookmarks::next_clicked);
	connect(cb_loop, &QCheckBox::clicked, this, &GUI_Bookmarks::loop_clicked);
	connect(cb_bookmarks, combo_current_index_changed_int, this, &GUI_Bookmarks::combo_changed);

	btn_tool->show_action(ContextMenu::EntryNew, false);
	btn_tool->show_action(ContextMenu::EntryDelete, false);

	disable_prev();
	disable_next();

	bookmarks_changed(_bookmarks->get_all_bookmarks());
}


void GUI_Bookmarks::bookmarks_changed(const QList<Bookmark>& bookmarks){

	if(!is_ui_initialized()){
		return;
	}

	disconnect(cb_bookmarks, combo_current_index_changed_int, this, &GUI_Bookmarks::combo_changed);

	cb_bookmarks->clear();
	for(const Bookmark& bookmark : bookmarks){
		cb_bookmarks->addItem(bookmark.get_name());
	}

	MetaData md = _bookmarks->get_cur_track();

	btn_tool->show_action(ContextMenu::EntryNew, (md.id >= 0) );
	btn_tool->show_action(ContextMenu::EntryDelete, !bookmarks.isEmpty() );

	if(md.id >= 0 && bookmarks.size() > 0){
		stackedWidget->setCurrentIndex(0);
	}

	else{
		stackedWidget->setCurrentIndex(1);
	}

	connect(cb_bookmarks, combo_current_index_changed_int, this, &GUI_Bookmarks::combo_changed);
}

void GUI_Bookmarks::disable_prev() {

	if(!is_ui_initialized()){
		return;
	}

	btn_bw->setEnabled( false );
	lab_last->setText( TXT_NO_BOOKMARK );
}

void GUI_Bookmarks::disable_next() {

	if(!is_ui_initialized()){
		return;
	}

	btn_fw->setEnabled(false);
	lab_next->setText( TXT_NO_BOOKMARK );
}

void GUI_Bookmarks::prev_changed(const Bookmark& bookmark){

	if(!is_ui_initialized()){
		return;
	}

	btn_bw->setEnabled( bookmark.is_valid() );
	cb_loop->setEnabled( btn_fw->isEnabled() );

	if( !bookmark.is_valid() ){
		disable_prev();
		return;
	}


	lab_last->setText(Helper::cvt_ms_to_string(bookmark.get_time() * 1000, true, true, false));
}

void GUI_Bookmarks::next_changed(const Bookmark& bookmark){

	if(!is_ui_initialized()){
		return;
	}

	btn_fw->setEnabled( bookmark.is_valid() );
	cb_loop->setEnabled( btn_fw->isEnabled() );

	if( !bookmark.is_valid() ){
		disable_next();
		return;
	}


	lab_next->setText(Helper::cvt_ms_to_string(bookmark.get_time() * 1000, true, true, false));
}


void GUI_Bookmarks::combo_changed(int cur_idx) {

	btn_tool->show_action(ContextMenu::EntryDelete, (cur_idx >= 0));

	if(cur_idx >= 0){
		_bookmarks->jump_to(cur_idx);
	}
}


void GUI_Bookmarks::next_clicked() {
	_bookmarks->jump_next();
}

void GUI_Bookmarks::prev_clicked() {
	_bookmarks->jump_prev();
}

void GUI_Bookmarks::loop_clicked(bool b){

	bool success = _bookmarks->set_loop(b);
	if(!success){
		cb_loop->setChecked(success);
	}
}

void GUI_Bookmarks::new_clicked() {

	MetaData md = _bookmarks->get_cur_track();
	if( md.id < 0 || md.db_id != 0 ){
		Message::warning(tr("Sorry, bookmarks can only be set for library tracks at the moment."),
						tr("Bookmarks"));
		return;
	}

	_bookmarks->save();
}


void GUI_Bookmarks::del_clicked() {

	int idx = cb_bookmarks->currentIndex();

	if(idx < 0){
		return;
	}

	_bookmarks->remove(idx);
	if(_bookmarks->get_size() == 0){
		stackedWidget->setCurrentIndex(1);
	}
}

void GUI_Bookmarks::del_all_clicked() {
	_bookmarks->remove_all();

	if(_bookmarks->get_size() == 0){
		stackedWidget->setCurrentIndex(1);
	}
}
