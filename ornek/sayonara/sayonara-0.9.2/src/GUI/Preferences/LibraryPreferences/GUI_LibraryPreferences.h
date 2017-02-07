/* GUI_LibraryPreferences.h */

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

#ifndef GUI_LIBRARYPREFERENCES_H
#define GUI_LIBRARYPREFERENCES_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"
#include "GUI/Preferences/ui_GUI_LibraryPreferences.h"


class GUI_LibraryPreferences :
		public PreferenceWidgetInterface,
		protected Ui::GUI_LibraryPreferences
{
	friend class PreferenceWidgetInterface;
	friend class PreferenceInterface<SayonaraWidget>;

	Q_OBJECT

public:
	GUI_LibraryPreferences(QWidget* parent=nullptr);
	void commit() override;
	void revert() override;


private:
	void init_ui() override;
	QString get_action_name() const override;


private slots:
	void language_changed() override;
};

#endif // GUI_LIBRARYPREFERENCES_H
