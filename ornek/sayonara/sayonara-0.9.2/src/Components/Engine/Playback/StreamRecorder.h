/* StreamRecorder.h */

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



#ifndef STREAMRECORDER_H
#define STREAMRECORDER_H

#include "Components/PlayManager/PlayManager.h"
#include "Helper/Settings/SayonaraClass.h"
#include "Helper/MetaData/MetaDataList.h"


#include <QDateTime>

class StreamRecorder : public QObject, protected SayonaraClass
{

private:

	PlayManager*	_play_manager;
	QString			_sr_recording_dst;				// recording destination
	QString			_session_path;					// where to store the mp3 files of the session
	QString			_session_playlist_name;			// playlist name
	MetaDataList	_session_collector;				// gather all tracks of a session
	MetaData		_md;							// current track

	bool            _recording;						// is in a session currently
	int				_idx;							// index of track (used for filename)

	// set metadata, add to session collector
    bool save();

	// saves session collector into playlist, creates new session,
	void new_session();

	// check and create session path
	QString check_session_path(const QString& sr_path);


public:
	StreamRecorder(QObject *parent=nullptr);
	virtual ~StreamRecorder();

	// change recording destination, create session path
	// returns destination file
	QString change_track(const MetaData& md);

	// start or end a session
	void record(bool b);

	// destination path
	QString get_dst_file() const ;

	// is in a session currently
	bool is_recording() const;

private slots:
	void playstate_changed(PlayManager::PlayState state);


};

#endif // STREAMRECORDER_H
