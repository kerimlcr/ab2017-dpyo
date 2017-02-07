/* LastFM.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * LastFM.h
 *
 *  Created on: Apr 19, 2011
 *      Author: Lucio Carreras
 */

#ifndef LASTFM_H_
#define LASTFM_H_

#include "Helper/Settings/SayonaraClass.h"
#include "Helper/MetaData/MetaData.h"

#include <QMap>
#include <QtXml>


// singleton base LastFM API class
// signals and slots are handled by the adapter class
class PlayManager;
class LFMLoginThread;
class LFMTrackChangedThread;
class LastFM : public QObject, protected SayonaraClass{

	Q_OBJECT
	SINGLETON(LastFM)

	signals:
		void sig_logged_in(bool);

	public slots:
		void psl_login();
		void psl_logout();


	private slots:
		void sl_login_thread_finished(bool success);
		void sl_similar_artists_available(IDList artist_ids);
		void sl_track_changed(const MetaData&);
		void sl_position_ms_changed(quint64);
		void sl_scrobble_response(const QByteArray& data);
		void sl_scrobble_error(const QString& str);


	public:
		bool is_logged_in();
		static void get_login(QString& user, QString& pw);


	private:

		LastFM(const LastFM&);
		LastFM& operator=(const LastFM&);


		bool						_logged_in;
		bool						_active;
		bool						_scrobbled;

		QString						_username;

		QString						_auth_token;
		QString						_session_key;

		LFMTrackChangedThread*		_track_changed_thread;
		LFMLoginThread*				_login_thread;

		PlayManager*				_play_manager;

		quint64						_old_pos;
		quint64						_old_pos_difference;

		MetaData					_md;




		bool init_track_changed_thread();
		bool update_track(const MetaData&);
		void get_similar_artists(const QString&);


		void reset_scrobble();
		bool check_scrobble(quint64 pos_ms);
		void scrobble(const MetaData&);

};

#endif /* LASTFM_H_ */
