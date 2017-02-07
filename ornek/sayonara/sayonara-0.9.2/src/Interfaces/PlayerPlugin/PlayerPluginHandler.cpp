/* PlayerPluginHandler.cpp */

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


#include "PlayerPlugin.h"
#include "PlayerPluginHandler.h"
#include "Helper/Helper.h"

#include <QAction>
#include <QPluginLoader>


PlayerPluginHandler::PlayerPluginHandler(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{
	REGISTER_LISTENER(Set::Player_Language, language_changed);
}

PlayerPluginHandler::~PlayerPluginHandler() {}


PlayerPluginInterface* PlayerPluginHandler::find_plugin(const QString& name) {

	sp_log(Log::Debug) << "Search for plugin " << name;

	for(PlayerPluginInterface* p : _plugins) {
		if(p->get_name().compare(name) == 0){
			return p;
		}
	}	

	return nullptr;
}

void PlayerPluginHandler::add_plugin(PlayerPluginInterface* p) {

	_plugins.push_back(p);

	connect(p, &PlayerPluginInterface::sig_action_triggered, this, &PlayerPluginHandler::plugin_action_triggered);
	connect(p, &PlayerPluginInterface::sig_reload, this,  &PlayerPluginHandler::reload_plugin);

	QString last_plugin = _settings->get(Set::Player_ShownPlugin);
	if(p->get_name() == last_plugin){
		p->get_action()->setChecked(true);
		plugin_action_triggered(p, true);
	}
}


void PlayerPluginHandler::plugin_action_triggered(PlayerPluginInterface* p, bool b) {

	if(!p){
		return;
	}

	if(!p->get_action()){
		return;
	}

	if(b) {
		emit sig_show_plugin(p);
		_settings->set(Set::Player_ShownPlugin, p->get_name());
	}

	else{
		emit sig_hide_all_plugins();
		_settings->set(Set::Player_ShownPlugin, QString());
	}
}


void PlayerPluginHandler::reload_plugin(PlayerPluginInterface* p) {
	if(p) {
		emit sig_show_plugin(p);
	}
}

void PlayerPluginHandler::language_changed()
{
	for(PlayerPluginInterface* p : _plugins) {
		p->language_changed();
		p->get_action()->setText(p->get_display_name());
	}
}

void PlayerPluginHandler::load_dynamic_plugins()
{
	QString lib_dir;
	QDir plugin_dir;
	QStringList dll_filenames;
	QString cur_plugin;

	lib_dir = Helper::get_lib_path();
	plugin_dir = QDir(lib_dir);
	dll_filenames = plugin_dir.entryList(QDir::Files);
	cur_plugin = _settings->get(Set::Lib_CurPlugin);

	for(const QString& filename : dll_filenames) {

		QObject* raw_plugin;
		PlayerPluginInterface* plugin;

		QPluginLoader loader(plugin_dir.absoluteFilePath(filename));

		raw_plugin = loader.instance();
		if(!raw_plugin) {
			loader.unload();
			continue;
		}

		plugin = dynamic_cast<PlayerPluginInterface*>(raw_plugin);
		if(!plugin) {
			loader.unload();
			continue;
		}

		sp_log(Log::Info) << "Found player plugin " << plugin->get_display_name();
		_plugins << plugin;
	}
}


QList<PlayerPluginInterface*> PlayerPluginHandler::get_all_plugins() const {
	return _plugins;
}
