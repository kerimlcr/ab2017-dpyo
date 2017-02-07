
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




#ifndef INSTANCETHREAD_H
#define INSTANCETHREAD_H

#include <QThread>
#include <QSharedMemory>
#include <QMainWindow>
#include "Components/Playlist/Playlist.h"

enum InstanceMessage {
	InstanceMessageNone,
	InstanceMessageWithoutFiles,
	InstanceMessageWithFiles
};

class PlaylistHandler;
class GUI_Player;
class InstanceThread : public QThread
{
	Q_OBJECT

signals:
	void sig_player_raise();
	void sig_create_playlist(const QStringList& playlist, const QString& name, bool temporary);

public:
		InstanceThread(InstanceMessage* instance_message, QObject* parent=nullptr);
		~InstanceThread();

		void stop();

private:
		bool				_may_run;
		InstanceMessage*	_instance_message=nullptr;
		QSharedMemory		_memory;


private:
		void run() override;
		void parse_memory();

};

#endif // INSTANCETHREAD_H
