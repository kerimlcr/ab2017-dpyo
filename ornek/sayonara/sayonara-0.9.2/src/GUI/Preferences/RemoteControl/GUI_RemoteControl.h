/* GUI_RemoteControl.h

 * Copyright (C) 2012-2016 Lucio Carreras  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Sep 3, 2012 
 *
 */

#ifndef GUISOCKETSETUP_H_
#define GUISOCKETSETUP_H_

#include "GUI/Preferences/ui_GUI_RemoteControl.h"

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"

class GUI_RemoteControl:
		public PreferenceWidgetInterface,
		private Ui::GUI_RemoteControl
{

	Q_OBJECT

	friend class PreferenceWidgetInterface;
	friend class PreferenceInterface<SayonaraWidget>;


public:
	GUI_RemoteControl(QWidget* parent=nullptr);
	virtual ~GUI_RemoteControl();

	void commit() override;
	void revert() override;

	QString get_action_name() const override;


protected:

	void init_ui() override;
	void language_changed() override;

private slots:
	void active_toggled(bool b);
	void port_changed(int port);


private:
	QString get_url_string();
	void refresh_url();

};

#endif /* GUISOCKETSETUP_H_ */
