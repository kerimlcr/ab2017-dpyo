/* SearchableTableView.cpp */

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



#include "SearchableTableView.h"
#include "AbstractSearchModel.h"
#include "GUI/Library/Models/LibraryItemModel.h"
#include "MiniSearcher.h"



SearchableTableView::SearchableTableView(QWidget* parent) :
	QTableView(parent)
{
	_mini_searcher = new MiniSearcher(this, MiniSearcherButtons::BothButtons);
	_abstr_model = 0;
	_cur_row = -1;

	connect(_mini_searcher, &MiniSearcher::sig_text_changed, this, &SearchableTableView::edit_changed);
	connect(_mini_searcher, &MiniSearcher::sig_find_next_row, this, &SearchableTableView::fwd_clicked);
	connect(_mini_searcher, &MiniSearcher::sig_find_prev_row, this, &SearchableTableView::bwd_clicked);
}

SearchableTableView::~SearchableTableView() {

}

void SearchableTableView::setAbstractModel(AbstractSearchTableModel* model) {
	 _abstr_model = model;
	 _mini_searcher->setExtraTriggers(_abstr_model->getExtraTriggers());
}

QAbstractItemModel* SearchableTableView::get_model() const
{
	return _abstr_model;
}

QItemSelectionModel* SearchableTableView::get_selection_model() const
{
	return this->selectionModel();
}

void SearchableTableView::set_current_index(int idx){
	this->setCurrentIndex(_abstr_model->index(idx, 0));
}

void SearchableTableView::mouseMoveEvent(QMouseEvent *e) {
	emit sig_mouse_moved();
	QTableView::mouseMoveEvent(e);

}

void SearchableTableView::mousePressEvent(QMouseEvent *e) {

	emit sig_mouse_pressed();
	QTableView::mousePressEvent(e);
}


void SearchableTableView::mouseReleaseEvent(QMouseEvent *e) {
	emit sig_mouse_released();
	QTableView::mouseReleaseEvent(e);
}

void SearchableTableView::keyPressEvent(QKeyEvent *e) {

	bool was_initialized = _mini_searcher->isInitialized();
	bool initialized = _mini_searcher->check_and_init(e);

	if(e->key() == Qt::Key_Tab && !was_initialized) {
		QWidget::keyPressEvent(e);
		return;
	}

	if(initialized || was_initialized) {
		_mini_searcher->keyPressEvent(e);
		e->setAccepted(false);
		return;
	}

	QTableView::keyPressEvent(e);
	e->setAccepted(true);
}


void SearchableTableView::edit_changed(QString str) {

	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getFirstRowIndexOf(str);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->selectRow(_cur_row);
}


void SearchableTableView::fwd_clicked() {
	QString str = _mini_searcher->getCurrentText();
	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getNextRowIndexOf(str, _cur_row + 1);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->selectRow(_cur_row);
}


void SearchableTableView::bwd_clicked() {

	QString str = _mini_searcher->getCurrentText();
	if(str.size() == 0) return;
	if(!_abstr_model) return;

	QModelIndex idx = _abstr_model->getPrevRowIndexOf(str, _cur_row -1);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->selectRow(_cur_row);
}


void SearchableTableView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	QTableView::selectionChanged(selected, deselected);

	emit sig_selection_changed(selected.indexes());
}
