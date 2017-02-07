/* PlayerPluginHandler.h */

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



#ifndef PLAYERPLUGINHANDLER_H
#define PLAYERPLUGINHANDLER_H

#include <QObject>
#include <QList>
#include <QString>

#include "Helper/Settings/SayonaraClass.h"


class PlayerPluginInterface;
class PlayerPluginHandler : public QObject, public SayonaraClass
{
    Q_OBJECT

public:
    PlayerPluginHandler(QObject *parent=nullptr);
    ~PlayerPluginHandler();


signals:
	void sig_show_plugin(PlayerPluginInterface*);
    void sig_hide_all_plugins();


private slots:
	void plugin_action_triggered(PlayerPluginInterface*, bool);
	void reload_plugin(PlayerPluginInterface* p);
	void language_changed();


private:
	QList<PlayerPluginInterface*>	_plugins;


public:
	void add_plugin(PlayerPluginInterface* plugin);
	void load_dynamic_plugins();

	PlayerPluginInterface*        find_plugin(const QString& name);
	QList<PlayerPluginInterface*> get_all_plugins() const;

};

#endif // PLAYERPLUGINHANDLER_H
