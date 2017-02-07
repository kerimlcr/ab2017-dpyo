/* GUI_Shutdown.h */

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



#ifndef GUI_SHUTDOWN_H
#define GUI_SHUTDOWN_H

#include "GUI/ShutdownDialog/ui_GUI_Shutdown.h"
#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"

#include "Helper/Shutdown/Shutdown.h"

#ifdef WITH_SHUTDOWN

class GUI_Shutdown : public SayonaraDialog, protected Ui::GUI_Shutdown
{
	Q_OBJECT

signals:
	void sig_closed();

private slots:

	void ok_clicked();
	void cancel_clicked();
	void rb_after_finished_clicked(bool b);
	void rb_after_minutes_clicked(bool b);

public:
	GUI_Shutdown(QWidget* parent=nullptr);
};

#endif

#endif // GUI_SHUTDOWN_H
