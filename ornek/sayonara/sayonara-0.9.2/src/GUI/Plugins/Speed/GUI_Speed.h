/* GUI_Speed.h */

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



#ifndef GUI_SPEED_H
#define GUI_SPEED_H

#include "GUI/Plugins/Speed/ui_GUI_Speed.h"

#include "Interfaces/PlayerPlugin/PlayerPlugin.h"

class GUI_Speed : public PlayerPluginInterface, private Ui::GUI_Speed
{
	Q_OBJECT

	friend class PlayerPluginInterface;

public:
	explicit GUI_Speed(QWidget *parent=nullptr);
	QString get_name() const override;
	QString get_display_name() const override;

private slots:
	void speed_changed(int value);
	void active_changed(bool enabled);
	void preserve_pitch_changed(bool enabled);
	void pitch_changed(int pitch);
	
	void revert_speed_clicked();
	void revert_pitch_clicked();

	void pitch_hovered(int val);
	void speed_hovered(int val);

	void _sl_pitch_found_changed();

protected:
	void language_changed() override;
	void init_ui() override;
};



#endif // GUI_SPEED_H
