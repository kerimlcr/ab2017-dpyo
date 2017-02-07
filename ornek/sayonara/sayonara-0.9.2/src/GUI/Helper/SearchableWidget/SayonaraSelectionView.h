/* SayonaraSelectionView.h */

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



#ifndef SAYONARASELECTIONVIEW_H
#define SAYONARASELECTIONVIEW_H

#include "Helper/globals.h"
#include "Helper/Set.h"

#include <QList>
#include <QItemSelectionModel>
#include <QAbstractItemModel>



class SayonaraSelectionView {

protected:
	virtual QItemSelectionModel* get_selection_model() const=0;
	virtual QAbstractItemModel* get_model() const =0;
	virtual void set_current_index(int idx)=0;

	virtual void select_all();
	virtual void select_rows(const SP::Set<int>& indexes, int min_col=0, int max_col=0);
	virtual void select_row(int row);
	virtual void clear_selection();
	virtual int get_min_selected() const;
	virtual int get_max_selected() const;

public:
	SP::Set<int> get_selections() const;
};




#endif // SAYONARASELECTIONVIEW_H
