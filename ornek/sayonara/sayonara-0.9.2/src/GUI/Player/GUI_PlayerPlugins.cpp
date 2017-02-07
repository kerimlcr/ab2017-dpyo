/* GUI_PlayerPlugins.cpp */

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

#include "GUI_Player.h"

#include "Interfaces/PlayerPlugin/PlayerPlugin.h"
#include "Interfaces/PlayerPlugin/PlayerPluginHandler.h"

void GUI_Player::hide_all_plugins()
{
	QList<PlayerPluginInterface*> plugins = _pph->get_all_plugins();

	for(PlayerPluginInterface* p : plugins){
		QAction* action = p->get_action();
		action->setChecked(false);
	}

	plugin_widget->close();
}


void GUI_Player::show_plugin(PlayerPluginInterface* plugin)
{
	hide_all_plugins();

	QAction* action = plugin->get_action();
	if(action){
		action->setChecked(true);
	}

	plugin_widget->show(plugin);
}
