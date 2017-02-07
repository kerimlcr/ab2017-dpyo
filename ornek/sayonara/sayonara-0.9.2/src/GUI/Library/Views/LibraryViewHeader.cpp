/* LibraryViewHeader.cpp */

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

#include "GUI/Library/Views/LibraryView.h"
#include "GUI/Library/Helper/ColumnHeader.h"
#include "GUI/Library/Views/HeaderView.h"

#include <QMouseEvent>
#include <algorithm>

template <typename T>
void switch_sorters(T& srcdst, T src1, T src2) {
	if(srcdst == src1) {
		srcdst = src2;
	}

	else {
		srcdst = src1;
	}
}


void LibraryView::sort_by_column(int col) {

	SortOrder asc_sortorder, desc_sortorder;

	HeaderView* header_view = this->get_header_view();

	int idx_col = header_view->visualIndex(col);
	ColumnHeader* h = header_view->get_column_header(idx_col);
	if(!h){
		return;
	}

	asc_sortorder = h->get_asc_sortorder();
	desc_sortorder = h->get_desc_sortorder();

	switch_sorters( _sort_order, asc_sortorder, desc_sortorder );

	emit sig_sortorder_changed(_sort_order);
}


void LibraryView::set_table_headers(const ColumnHeaderList& headers, const BoolList& shown_cols, SortOrder sorting) {

	HeaderView* header_view = this->get_header_view();

	_model->removeColumns(0, _model->columnCount());
	_model->insertColumns(0, headers.size());

	int i=0;
	for(ColumnHeader* header : headers){
		_model->setHeaderData(i, Qt::Horizontal, header->get_title(), Qt::DisplayRole);
		i++;
	}

	header_view->set_column_headers(headers, shown_cols, sorting);

	language_changed();
}


void LibraryView::header_actions_triggered(const BoolList& shown_cols) {

	SP::Set<int> sel_indexes = get_selections();

	std::for_each(sel_indexes.begin(), sel_indexes.end(), [this](int row){
		this->selectRow(row);
	});

	emit sig_columns_changed(shown_cols);
}

HeaderView*	LibraryView::get_header_view(){
	return static_cast<HeaderView*>(this->horizontalHeader());
}

// header end
