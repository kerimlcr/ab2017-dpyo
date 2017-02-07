/* PlaylistTabWidget.cpp */

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

#include "PlaylistTabWidget.h"
#include "PlaylistTabBar.h"
#include "Helper/MetaData/MetaDataList.h"


PlaylistTabWidget::PlaylistTabWidget(QWidget* parent) :
	QTabWidget(parent)
{
	_tab_bar = new PlaylistTabBar(this);
	this->setTabBar(_tab_bar);


	connect(_tab_bar, &PlaylistTabBar::sig_open_file, this, &PlaylistTabWidget::sig_open_file);
	connect(_tab_bar, &PlaylistTabBar::sig_open_dir, this, &PlaylistTabWidget::sig_open_dir);
	connect(_tab_bar, &PlaylistTabBar::sig_tab_reset, this, &PlaylistTabWidget::sig_tab_reset);
	connect(_tab_bar, &PlaylistTabBar::sig_tab_save, this, &PlaylistTabWidget::sig_tab_save);
	connect(_tab_bar, &PlaylistTabBar::sig_tab_save_as, this, &PlaylistTabWidget::sig_tab_save_as);
	connect(_tab_bar, &PlaylistTabBar::sig_tab_rename, this, &PlaylistTabWidget::sig_tab_rename);
	connect(_tab_bar, &PlaylistTabBar::sig_tab_delete, this, &PlaylistTabWidget::sig_tab_delete);
	connect(_tab_bar, &PlaylistTabBar::sig_tab_clear, this, &PlaylistTabWidget::sig_tab_clear);
	connect(_tab_bar, &PlaylistTabBar::sig_cur_idx_changed, this, &PlaylistTabWidget::currentChanged);
	connect(_tab_bar, &PlaylistTabBar::sig_add_tab_clicked, this, &PlaylistTabWidget::sig_add_tab_clicked);
	connect(_tab_bar, &PlaylistTabBar::sig_metadata_dropped, this, &PlaylistTabWidget::sig_metadata_dropped);
}

PlaylistTabWidget::~PlaylistTabWidget(){

}

void PlaylistTabWidget::show_tabbar(){
	this->tabBar()->show();
}

void PlaylistTabWidget::hide_tabbar(){
	this->tabBar()->hide();
}

void PlaylistTabWidget::show_menu_items(PlaylistMenuEntries entries){
	_tab_bar->show_menu_items(entries);
}

void PlaylistTabWidget::check_last_tab()
{
	int cur_idx, num_tabs;

	cur_idx = currentIndex();
	num_tabs = count();

	if(_tab_bar){
		QWidget* close_button;

		_tab_bar->setTabsClosable(num_tabs > 2);
		close_button = _tab_bar->tabButton(num_tabs - 1, QTabBar::RightSide);

		if(close_button){
			close_button->setMaximumWidth(0);
			close_button->hide();
		}

		_tab_bar->setTabIcon(num_tabs - 1, QIcon());
	}

	if(cur_idx == num_tabs - 1 && num_tabs >= 2){
		this->setCurrentIndex(num_tabs - 2);
	}
}

void PlaylistTabWidget::removeTab(int index){
	QTabWidget::removeTab(index);
	check_last_tab();
}

void PlaylistTabWidget::addTab(QWidget *widget, const QIcon &icon, const QString &label){
	QTabWidget::addTab(widget, icon, label);
	check_last_tab();
}

void PlaylistTabWidget::addTab(QWidget *widget, const QString& label){
	QTabWidget::addTab(widget, label);
	check_last_tab();
}

void PlaylistTabWidget::insertTab(int index, QWidget *widget, const QString &label){
	QTabWidget::insertTab(index, widget, label);
	this->setCurrentIndex(index);
	check_last_tab();
}

void PlaylistTabWidget::insertTab(int index, QWidget *widget, const QIcon &icon, const QString &label){
	QTabWidget::insertTab(index, widget, icon, label);
	this->setCurrentIndex(index);
	check_last_tab();
}

bool PlaylistTabWidget::was_drag_from_playlist() const
{
	return _tab_bar->was_drag_from_playlist();
}

int PlaylistTabWidget::get_drag_origin_tab() const
{
	return _tab_bar->get_drag_origin_tab();
}


