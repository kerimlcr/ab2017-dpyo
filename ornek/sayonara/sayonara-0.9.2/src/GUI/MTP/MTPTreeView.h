
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




#ifndef MTPTREEVIEW_H
#define MTPTREEVIEW_H

#include <QTreeWidget>
#include "GUI/Helper/CustomMimeData.h"
#include "Helper/Logger/Logger.h"

class MTPTreeView : public QTreeWidget
{
	Q_OBJECT

signals:
	void sig_metadata_dropped(const MetaDataList& v_md, int idx);

public:
	explicit MTPTreeView(QWidget *parent = 0);


protected:
	virtual bool dropMimeData(QTreeWidgetItem* parent, int index, const QMimeData * data, Qt::DropAction action) override;


};



#endif // MTPTREEVIEW_H
