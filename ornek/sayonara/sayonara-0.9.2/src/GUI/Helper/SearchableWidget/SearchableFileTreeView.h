
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




#ifndef SEARCHABLEFileTreeView_H
#define SEARCHABLEFileTreeView_H

#include "AbstractSearchModel.h"

#include <QTreeView>
#include <QThread>
#include <QFileSystemModel>

class AbstractSearchFileTreeModel :
		public QFileSystemModel,
		private AbstractSearchModelInterface
{

	Q_OBJECT

public:
	AbstractSearchFileTreeModel(QObject* parent=nullptr);

public:
	virtual QModelIndex getFirstRowIndexOf(QString substr) override;
	virtual QModelIndex getNextRowIndexOf(QString substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
	virtual QModelIndex getPrevRowIndexOf(QString substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
	virtual QMap<QChar, QString> getExtraTriggers() override;

private:
	QStringList		_found_strings;
	int				_cur_idx;
};




#endif // SEARCHABLEFileTreeView_H
