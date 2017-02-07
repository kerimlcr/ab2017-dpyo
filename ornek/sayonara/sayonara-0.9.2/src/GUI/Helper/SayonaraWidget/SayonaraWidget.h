/* SayonaraWidget.h */

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



#ifndef SAYONARAWIDGET_H
#define SAYONARAWIDGET_H

#include <QObject>
#include <QWidget>
#include <QMainWindow>

#include "SayonaraWidgetTemplate.h"


class SayonaraDialog;

/**
 * @brief Widget with Settings connection. Also contains triggers for language_changed() and skin_changed()
 * \nThe widget's content can be exported to a SayonaraDialog via the box_into_dialog() method.
 * @ingroup Widgets
 * @ingroup Interfaces
 */
class SayonaraWidget :
		public SayonaraWidgetTemplate<QWidget>
{

	Q_OBJECT

public:
	SayonaraWidget(QWidget* parent=nullptr);
	virtual ~SayonaraWidget();

	SayonaraDialog* box_into_dialog();


protected:
	SayonaraDialog* _boxed_dialog=nullptr;

protected slots:
	virtual void language_changed();
	virtual void skin_changed();
};

/**
 * @brief The SayonaraMainWindow class
 * @ingroup Widgets
 */
class SayonaraMainWindow : public SayonaraWidgetTemplate<QMainWindow> {

	Q_OBJECT

public:
	SayonaraMainWindow(QWidget* parent=nullptr);
	virtual ~SayonaraMainWindow();

	void raise();


protected slots:
	virtual void language_changed();
	virtual void skin_changed();
};


#endif // SAYONARAWIDGET_H
