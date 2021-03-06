/* ComboBoxDelegate.h */

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



#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QStyledItemDelegate>

/**
 * @brief The ComboBoxDelegate class\n
 * Set this delegate to ComboBoxes if they should look nice
 * @ingroup GUIHelper
 */
class ComboBoxDelegate : public QStyledItemDelegate
{
Q_OBJECT

public:
  ComboBoxDelegate(QObject *parent = nullptr);

  virtual void paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const override;
  virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};


#endif // COMBOBOXDELEGATE_H
