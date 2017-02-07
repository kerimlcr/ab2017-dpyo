
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


#include "HeaderView.h"
#include <QFontMetrics>
#include <QTableView>

#include <algorithm>

HeaderView::HeaderView(Qt::Orientation orientation, QWidget* parent) :
	QHeaderView(orientation, parent),
	SayonaraClass()
{
	_context_menu = new QMenu(this);

	this->setSectionsClickable(true);
	this->setStretchLastSection(true);
	this->setHighlightSections(false);

	REGISTER_LISTENER_NO_CALL(Set::Player_Language, language_changed);
}

QSize HeaderView::sizeHint() const
{
	QSize size = QHeaderView::sizeHint();
	QFontMetrics fm(this->font());

	int height = std::max(fm.height() + 10, 20);

	size.setHeight(height);

	return size;
}

void HeaderView::init_header_action(ColumnHeader* header, bool is_shown)
{
	QAction* action = header->get_action();
	action->setChecked(is_shown);
	connect(action, &QAction::toggled, this, &HeaderView::action_triggered);

	this->addAction(action);
}

void HeaderView::action_triggered(bool b)
{
	Q_UNUSED(b)

	QTableView* table_view = static_cast<QTableView*>(this->parentWidget());

	BoolList shown_cols = refresh_active_columns();

	if(table_view){
		this->refresh_sizes(table_view);
	}

	emit sig_columns_changed(shown_cols);
}


void HeaderView::set_column_headers(const ColumnHeaderList& column_headers, const BoolList& shown_actions, SortOrder sorting)
{
	_column_headers = column_headers;

	int i=0;

	for(ColumnHeader* header : _column_headers) {

		if( header->get_asc_sortorder() == sorting) {
			this->setSortIndicator(i, Qt::AscendingOrder);
		}

		else if( header->get_desc_sortorder() == sorting) {
			this->setSortIndicator(i, Qt::DescendingOrder);
		}

		bool is_shown = true;
		if(between(i, shown_actions)){
			is_shown = shown_actions[i];
		}

		init_header_action(header, is_shown);

		i++;
	}

	refresh_active_columns();

	this->setContextMenuPolicy(Qt::ActionsContextMenu);
}



void HeaderView::refresh_sizes(QTableView* view)
{

	int altogether_width = 0;
	int desired_width = 0;
	int tolerance = 30;
	double altogether_percentage = 0;

	int n_cols = _column_headers.get_shown_columns();

	for(int i=0; i<n_cols; i++) {

		int preferred_size = 0;
		int col = _column_headers.get_nth_shown_col(i);

		if(!between(col, _column_headers)){
			continue;
		}

		ColumnHeader* h = _column_headers[col];

		if(h->get_size_type() == ColumnHeader::SizeType::Abs) {
			preferred_size = h->get_preferred_size_abs();
		}

		else{
			altogether_percentage += h->get_preferred_size_rel();
			desired_width += h->get_preferred_size_abs();
		}

		altogether_width += preferred_size;
	}

	altogether_width += tolerance;

	int target_width = view->width() - altogether_width;


	if(target_width < desired_width) {
		target_width = desired_width;
		view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}

	else{
		view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}

	// width for percentage stuff
	for(int i=0; i<n_cols; i++) {

		int col = _column_headers.get_nth_shown_col(i);
		int preferred_size = 0;

		ColumnHeader* h = _column_headers[col];
		if(h->get_size_type() == ColumnHeader::SizeType::Rel) {
			preferred_size = (h->get_preferred_size_rel() * target_width) / altogether_percentage;
		}

		else{
			preferred_size = h->get_preferred_size_abs();
		}

		view->setColumnWidth(col, preferred_size);
	}
}

BoolList HeaderView::refresh_active_columns()
{
	BoolList lst;
	int n_cols = _column_headers.size();

	for(int i=0; i<n_cols; i++){
		ColumnHeader* section =_column_headers[i];
		bool is_hidden = section->is_hidden();
		if(is_hidden){
			this->hideSection(i);
		}

		else{
			this->showSection(i);
		}

		lst << section->is_visible();
	}

	return lst;
}

ColumnHeader* HeaderView::get_column_header(int idx)
{
	if(!between(idx, _column_headers)){
		return nullptr;
	}

	return _column_headers[idx];
}


void HeaderView::language_changed()
{
	for(ColumnHeader* header : _column_headers) {
		header->retranslate();
	}
}
