/* GUI_PlayerPreferences.h */

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



#ifndef GUI_PLAYERPREFERENCES_H
#define GUI_PLAYERPREFERENCES_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"
#include "GUI/Preferences/ui_GUI_PlayerPreferences.h"


class GUI_PlayerPreferences :
		public PreferenceWidgetInterface,
		private Ui::GUI_PlayerPreferences
{
	Q_OBJECT

	friend class PreferenceInterface<SayonaraWidget>;
	friend class PreferenceWidgetInterface;

public:
	explicit GUI_PlayerPreferences(QWidget *parent = 0);
	~GUI_PlayerPreferences();


protected:
	void init_ui() override;
	void language_changed() override;


public:
	QString get_action_name() const override;

	void commit() override;
	void revert() override;

private slots:
	void show_tray_icon_toggled(bool b);
};

#endif // GUI_PLAYERPREFERENCES_H
