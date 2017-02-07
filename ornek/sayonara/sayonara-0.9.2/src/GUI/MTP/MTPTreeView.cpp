
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


#include "MTPTreeView.h"

MTPTreeView::MTPTreeView(QWidget *parent) :
	QTreeWidget(parent)
{
}

bool MTPTreeView::dropMimeData(QTreeWidgetItem* parent, int index, const QMimeData* data, Qt::DropAction action)
{

	Q_UNUSED(action)

	QTreeWidget::dropMimeData(parent, index, data, action);

	parent->setSelected(true);
	const CustomMimeData* cmd = dynamic_cast<const CustomMimeData*>(data);
	if(!cmd){
		sp_log(Log::Debug) << "Cannot drop mimedata";
		return false;
	}

	sp_log(Log::Debug) << "Successfully dropped metadata";

	MetaDataList v_md = cmd->getMetaData();
	emit sig_metadata_dropped(v_md, index);

	return true;

}
