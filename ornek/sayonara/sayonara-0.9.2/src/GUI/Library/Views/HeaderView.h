
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



#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QHeaderView>
#include <QWidget>
#include <QMenu>
#include <QTableView>

#include "GUI/Library/Helper/ColumnHeader.h"
#include "Helper/Settings/SayonaraClass.h"
#include "Components/Library/Sorting.h"


class HeaderView :
		public QHeaderView,
		protected SayonaraClass
{

	Q_OBJECT

signals:
	void sig_columns_changed(const BoolList& shown_cols);

private:
	QMenu*				_context_menu=nullptr;
	ColumnHeaderList	_column_headers;

private:
	void init_header_action(ColumnHeader* header, bool is_shown);

private slots:
	void action_triggered(bool b);
	void language_changed();

public:

	HeaderView(Qt::Orientation orientation, QWidget* parent=nullptr);
	QSize sizeHint() const override;

	void set_column_headers(const ColumnHeaderList& column_headers, const BoolList& shown_columns, SortOrder sorting );
	void refresh_sizes(QTableView* view);
	BoolList refresh_active_columns();

	ColumnHeader* get_column_header(int idx);

};



#endif // HEADERVIEW_H
