/* GUI_PlaylistPreferences.h */

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



/* GUI_PlaylistPreferences.h */


#ifndef GUI_PLAYLISTPREFERENCES_H
#define GUI_PLAYLISTPREFERENCES_H


#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"
#include "GUI/Preferences/ui_GUI_PlaylistPreferences.h"


class GUI_PlaylistPreferences :
		public PreferenceWidgetInterface,
		protected Ui::GUI_PlaylistPreferences
{
	Q_OBJECT

	friend class PreferenceWidgetInterface;
	friend class PreferenceInterface<SayonaraWidget>;

public:
	explicit GUI_PlaylistPreferences(QWidget *parent = 0);
	~GUI_PlaylistPreferences();

	void commit() override;
	void revert() override;

private:
	void init_ui() override;
	QString get_action_name() const override;

private slots:
	void language_changed() override;
	void cb_toggled(bool b);

};

#endif // GUI_PLAYLISTPREFERENCES_H
