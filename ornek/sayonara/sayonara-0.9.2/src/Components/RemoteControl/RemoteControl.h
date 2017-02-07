/* RemoteControl.h */

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



#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include "Helper/Settings/SayonaraClass.h"
#include "Components/PlayManager/PlayManager.h"
#include "Components/Playlist/Playlist.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QByteArray>

#include <functional>

class PlaylistHandler;

typedef std::function<void()> RemoteFunction ;
typedef std::function<void(int)> RemoteFunctionInt ;

/**
 * @brief Remote control allows to control Sayonara from an external application via network.
 * Various commands are implemented. Sayonara also delivers information about state changes,
 * @ingroup Components
 *
 * The current implemented commands are:\n
 *
 *
 * <B>play</B> \t start playing \n
 * <B>pause</B> \t pause playing \n
 * <B>prev</B> \t previous song \n
 * <B>next</B> \t next song \n
 * <B>playpause</B> \t toggle play/pause \n
 * <B>stop</B> \t stop playing \n
 * <B>volup</B> \t increase volume \n
 * <B>voldown</B> \t decrease volume \n
 * <B>setvol <int></B>\t change volume \n
 * <B>pl</B> \t fetch the active playlist \n
 * <B>curSong</B> \t fetch the current song index \n
 * <B>seekrel <int></B> \t seek within song in percent \n
 * <B>seekrelms <int></B> \t seek within song in relative to current position in seconds \n
 * <B>chtrk <int></B> \t change track \n
 * <B>state</B> \t request state: every answer except playlists are returned \n
 * \n\n
 * The current implemented answers are: (multiple parameters are separated by the tab character.
 * And answer always ends with th four bytes combination 0x00 0x01 0x00 0x01)
 * \n
 * <B>curPos:<int></B> \t current position in seconds \n
 * <B>vol:<int></B> \t current volume \n
 * <B>curIdx:<int></B> \t current track index in active playlist \n
 * <B>title:<string></B> \t current track title \n
 * <B>artist:<string></B> \t current track artist \n
 * <B>album:<string></B> \t current track album \n
 * <B>totalPos:<string></B> \t length of track in seconds \n
 * <B>playstate:[playing|paused|stopped]</B> \t current playback state \n
 * <B>playlist:<stringlist></B> \t active playlist \n
 * <B>broadcast:<int,int></B> \t broadcast settings: <active, broadcast port> \n
 * <B>coverinfo:<width,height,format></B> \t width, height, qt image format.\n
 * \tfollowed by 0x00,0x01,0x00,0x01<cover data>0x00,0x01,0x00,0x01
 * @ingroup Components
 */
class RemoteControl : public QObject, protected SayonaraClass
{
	Q_OBJECT

public:
	RemoteControl(QObject *parent = 0);
	virtual ~RemoteControl();


	bool is_connected() const;

private slots:
	void new_connection();
	void socket_disconnected();
	void new_request();

	void pos_changed_ms(const quint64 pos);
	void track_changed(const MetaData& md);
	void volume_changed(int vol);
	void playstate_changed(PlayManager::PlayState playstate);
	void playlist_changed(PlaylistConstPtr pl);

	void _sl_active_changed();
	void _sl_port_changed();
	void _sl_broadcast_changed();


private:

	bool _initialized;

	QMap<QByteArray, RemoteFunction> _fn_call_map;
	QMap<QByteArray, RemoteFunctionInt> _fn_int_call_map;

	QTcpServer*		_server=nullptr;
	QTcpSocket*		_socket=nullptr;
	PlayManager*	_play_manager=nullptr;
	PlaylistHandler* _plh=nullptr;

private:

	void init();

	void set_volume(int vol);
	void seek_rel(int pos_percent);
	void seek_rel_ms(int pos_ms);
	void change_track(int idx);

	void show_api();
	void request_state();

	int extract_parameter_int(const QByteArray& arr, int cmd_len);

	void write_playstate();
	void write_broadcast_info();
	void write_cur_track();
	void write_volume();
	void write_cur_pos();
	void write_playlist();

	void write_cover();
	void write_cover(const MetaData& md);

	void write(const QByteArray& arr);



};

#endif // REMOTECONTROL_H
