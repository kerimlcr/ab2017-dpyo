/* ComboBoxDelegate.cpp */

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



#include "ComboBoxDelegate.h"
#include <QVariant>
#include <QModelIndex>

ComboBoxDelegate::ComboBoxDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{

}



void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QStyledItemDelegate::paint(painter, option, index);
}


QSize ComboBoxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
	Q_UNUSED(option)
	Q_UNUSED(index)

	return QSize(200, 20);

}
