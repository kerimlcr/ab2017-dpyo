/* GUI_PlayerPlugin.cpp */

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



/* GUI_PlayerPlugin.cpp */


#include "GUI_PlayerPlugin.h"
#include "Interfaces/PlayerPlugin/PlayerPlugin.h"

GUI_PlayerPlugin::GUI_PlayerPlugin(QWidget *parent) :
	SayonaraWidget(parent),
	Ui::GUI_PlayerPlugin()
{
	setupUi(this);

	connect(btn_close, &QPushButton::clicked, this, &GUI_PlayerPlugin::close);
}


void GUI_PlayerPlugin::show(PlayerPluginInterface* player_plugin)
{
	close_cur_plugin();

	_current_plugin = player_plugin;

	bool show_title = player_plugin->is_title_shown();

	header_widget->setVisible(show_title);
	lab_title->setText(player_plugin->get_display_name());

	verticalLayout->insertWidget(1, player_plugin);

	player_plugin->resize(this->width(), player_plugin->height());
	player_plugin->show();

	SayonaraWidget::show();
}

void GUI_PlayerPlugin::language_changed() {
	if(_current_plugin){
		lab_title->setText(_current_plugin->get_display_name());
	}
}

void GUI_PlayerPlugin::closeEvent(QCloseEvent* e)
{
	close_cur_plugin();

	SayonaraWidget::closeEvent(e);
}

void GUI_PlayerPlugin::close_cur_plugin(){

	if(_current_plugin){
		_current_plugin->close();
	}

	_current_plugin = nullptr;
	_settings->set(Set::Player_ShownPlugin, QString());
}
