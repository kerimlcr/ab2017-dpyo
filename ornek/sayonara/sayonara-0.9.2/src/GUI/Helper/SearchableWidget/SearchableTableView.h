/* SearchableTableView.h */

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



#ifndef SEARCHABLETABLEVIEW_H
#define SEARCHABLETABLEVIEW_H

#include "SayonaraSelectionView.h"

#include <QTableView>
#include <QMouseEvent>
#include <QKeyEvent>


class AbstractSearchTableModel;
class MiniSearcher;
class SearchableTableView : public QTableView, public SayonaraSelectionView {
	Q_OBJECT

signals:
	void sig_mouse_moved();
	void sig_mouse_pressed();
	void sig_mouse_released();
	void sig_focus_out();
	void sig_key_pressed(QKeyEvent*);
	void sig_selection_changed(const QModelIndexList& idxs);


private slots:
	void edit_changed(QString str);
	void fwd_clicked();
	void bwd_clicked();

private:
	MiniSearcher*               _mini_searcher=nullptr;
	AbstractSearchTableModel*   _abstr_model=nullptr;
	int							_cur_row;

	virtual QAbstractItemModel* get_model() const override;
	virtual QItemSelectionModel* get_selection_model() const override;
	virtual void set_current_index(int idx) override;


public:
	SearchableTableView(QWidget* parent=nullptr);
	virtual ~SearchableTableView();
	void setAbstractModel(AbstractSearchTableModel* model);

protected:
	void mouseMoveEvent(QMouseEvent *) override;
	void mousePressEvent(QMouseEvent *) override;
	void mouseReleaseEvent(QMouseEvent *) override;
	void keyPressEvent(QKeyEvent *) override;
	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
};

#endif
