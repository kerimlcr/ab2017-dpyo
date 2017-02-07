/* SearchableListView.cpp */

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



#include "SearchableListView.h"
#include "AbstractSearchModel.h"
#include "MiniSearcher.h"

SearchableListView::SearchableListView(QWidget* parent) :
	QListView(parent)
{

	_mini_searcher = new MiniSearcher(this, MiniSearcherButtons::BothButtons);
	_abstr_model = nullptr;
	_cur_row = -1;

	connect(_mini_searcher, &MiniSearcher::sig_text_changed, this, &SearchableListView::edit_changed);
	connect(_mini_searcher, &MiniSearcher::sig_find_next_row, this, &SearchableListView::fwd_clicked);
	connect(_mini_searcher, &MiniSearcher::sig_find_prev_row, this, &SearchableListView::bwd_clicked);
}

SearchableListView::~SearchableListView() {
}


void SearchableListView::setAbstractModel(AbstractSearchListModel* model) {
	 _abstr_model = model;
	 _mini_searcher->setExtraTriggers(_abstr_model->getExtraTriggers());
}

QAbstractItemModel* SearchableListView::get_model() const
{
	return _abstr_model;
}

QItemSelectionModel* SearchableListView::get_selection_model() const
{
	return this->selectionModel();
}

void SearchableListView::set_current_index(int idx){
	this->setCurrentIndex(_abstr_model->index(idx, 0));
}

void SearchableListView::mouseMoveEvent(QMouseEvent *e) {
	emit sig_mouse_moved();
	QListView::mouseMoveEvent(e);

}

void SearchableListView::mousePressEvent(QMouseEvent *e) {

	emit sig_mouse_pressed();
	QListView::mousePressEvent(e);
}


void SearchableListView::mouseReleaseEvent(QMouseEvent *e) {
	emit sig_mouse_released();
	QListView::mouseReleaseEvent(e);
}

void SearchableListView::keyPressEvent(QKeyEvent *e) {

	bool was_initialized = _mini_searcher->isInitialized();
	bool initialized = _mini_searcher->check_and_init(e);

	if(e->key() == Qt::Key_Tab && !was_initialized) {
		return;
	}

	if(initialized || was_initialized) {
		_mini_searcher->keyPressEvent(e);
		e->setAccepted(false);
		return;
	}

	QListView::keyPressEvent(e);
	e->setAccepted(true);
}


void SearchableListView::edit_changed(const QString& str) {

	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getFirstRowIndexOf(str);
	if(idx.row() == -1) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->setCurrentIndex(idx);

	SP::Set<int> indexes;
	indexes.insert(_cur_row);
	this->select_rows(indexes);
}

void SearchableListView::fwd_clicked() {
	QString str = _mini_searcher->getCurrentText();
	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getNextRowIndexOf(str, _cur_row + 1);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	SP::Set<int> indexes;
	indexes.insert(idx.row());

	this->scrollTo(idx);
	this->select_rows(indexes);
}

void SearchableListView::bwd_clicked() {

	QString str = _mini_searcher->getCurrentText();
	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getPrevRowIndexOf(str, _cur_row - 1);
	if(!idx.isValid()) return;

	_cur_row = idx.row();


	SP::Set<int> indexes;
	indexes.insert(idx.row());

	this->scrollTo(idx);
	this->select_rows(indexes);
}

void SearchableListView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	QListView::selectionChanged(selected, deselected);

	emit sig_selection_changed(selected.indexes());
}
