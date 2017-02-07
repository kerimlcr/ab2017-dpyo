/* AlternativeCoverItemModel.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * AlternativeCoverItemModel.h
 *
 *  Created on: Jul 1, 2011
 *      Author: Lucio Carreras
 */

#ifndef ALTERNATIVECOVERITEMMODEL_H_
#define ALTERNATIVECOVERITEMMODEL_H_


#include <QList>
#include <QPair>
#include <QModelIndex>
#include <QAbstractTableModel>

class CoverLocation;
struct RowColumn {
	int row;
	int col;
    bool valid;

    RowColumn(){
		row = -1;
		col = -1;
        valid = false;
    }
};


/**
 * @brief The AlternativeCoverItemModel class
 * @ingroup GUICovers
 */
class AlternativeCoverItemModel : public QAbstractTableModel {

Q_OBJECT

public:
	AlternativeCoverItemModel(QObject* parent);
	virtual ~AlternativeCoverItemModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::DisplayRole) override;

	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex()) override;
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex()) override;

	bool is_valid(int row, int col);


	RowColumn cvt_2_row_col(int idx) const ;
	int cvt_2_idx(int row, int col) const ;


private:
	QStringList _pathlist;
};


#endif
