/* LFMTrackChangedThread.h

 * Copyright (C) 2012-2016 Lucio Carreras
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras,
 * Jul 18, 2012
 *
 */

#ifndef LFMTRACKCHANGEDTHREAD_H_
#define LFMTRACKCHANGEDTHREAD_H_

#include <QThread>
#include <QList>
#include <QMap>
#include <QHash>
#include <QDomDocument>

#include "LFMGlobals.h"
#include "ArtistMatch.h"

#include "Helper/SmartCompare/SmartCompare.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/Settings/SayonaraClass.h"

#define LFM_THREAD_TASK_UPDATE_TRACK 		1<<0
#define LFM_THREAD_TASK_SIM_ARTISTS 		1<<1


class LFMTrackChangedThread : public QObject, protected SayonaraClass {

	Q_OBJECT


signals:

	void sig_similar_artists_available(const IDList& artist_ids);


public:
	LFMTrackChangedThread(const QString& username=QString(), const QString& session_key=QString(), QObject* parent=nullptr);
	~LFMTrackChangedThread();


	void set_session_key(const QString& session_key);
	void set_username(const QString& username);


	void search_similar_artists(const MetaData& md);
	void update_now_playing(const MetaData& md);


private:

	/* update track */
	QString						_artist;
	QString						_username;
	QString						_session_key;
	QHash<QString, ArtistMatch>  _sim_artists_cache;
	MetaData					_md;
	SmartCompare*				_smart_comparison=nullptr;



private:
	void evaluate_artist_match(const ArtistMatch& artist_match);

	QMap<QString, int> filter_available_artists(const ArtistMatch& artist_match, ArtistMatch::Quality quality);


private slots:
	void response_sim_artists(const QByteArray& data);
	void error_sim_artists(const QString& error);

	void response_update(const QByteArray& response);
	void error_update(const QString& error);
};

#endif /* LFMTRACKCHANGEDTHREAD_H_ */
