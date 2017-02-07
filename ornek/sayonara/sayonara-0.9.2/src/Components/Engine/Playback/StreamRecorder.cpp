/* StreamRecorder.cpp */

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

#include "StreamRecorder.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Parser/PlaylistParser.h"
#include "Helper/Tagging/Tagging.h"


static QString get_time_str() {

    QString time_str;
    QDateTime cur = QDateTime::currentDateTime();

    QString weekday = cur.date().longDayName(cur.date().dayOfWeek() );
    QString year = QString::number(cur.date().year());
    QString month = cur.date().shortMonthName(cur.date().month());
    QString day = QString("%1").arg(cur.date().day(), 2, 10, QLatin1Char('0'));
    QString hr = QString("%1").arg(cur.time().hour(), 2, 10, QLatin1Char('0'));
    QString min = QString("%1").arg(cur.time().minute(), 2, 10, QLatin1Char('0'));

    time_str = weekday + "_" + year + "-" + month + "-" + day + "_" + hr + "h" + min;
    return time_str;
}



StreamRecorder::StreamRecorder(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{
	_play_manager = PlayManager::getInstance();
    _session_path = get_time_str();
	_recording = false;
	_idx = 1;

	QDir d(Helper::get_sayonara_path());

    // delete old stream ripper files
    QStringList lst = d.entryList(Helper::get_soundfile_extensions());
	for( const QString& str : lst) {
		sp_log(Log::Info) << "Remove " << d.absolutePath() + QDir::separator() + str;
        QFile f(d.absolutePath() + QDir::separator() + str);
        f.remove();
    }

	connect(_play_manager, &PlayManager::sig_playstate_changed, this, &StreamRecorder::playstate_changed);
}


StreamRecorder::~StreamRecorder() {

}


void StreamRecorder::new_session(){

    _md.title = "";
	_session_path = get_time_str();
	_session_collector.clear();
    _sr_recording_dst = "";
	_idx = 1;

    sp_log(Log::Info) << "New session: " << _session_path;
}



QString StreamRecorder::change_track(const MetaData& md) {

	QString sr_path;
	QString session_path;
	QString title;

	if(!_recording){
		return "";
	}

	if(md.title == _md.title) {
		return _sr_recording_dst;
	}

	save();

	if(!Helper::File::is_www(md.filepath())) {
		_recording = false;
		_sr_recording_dst = "";
		return "";
	}
	
	_md = md;
	_md.year = QDateTime::currentDateTime().date().year();
	_md.track_num = _idx;
	
	title = QString("%1").arg(_idx, 3, 10, QLatin1Char('0')) + "_" + md.title;
	title.replace(" ", "_");
	title.replace("/", "_");
	title.replace("\\", "_");
	title.replace(":", "");

	sr_path = _settings->get(Set::Engine_SR_Path);
	session_path = check_session_path(sr_path);

	if(session_path.isEmpty()){
		_sr_recording_dst = "";
		_session_playlist_name = "";
		_recording = false;
	}

	else{
		_session_playlist_name = session_path + "/playlist.m3u";
		_sr_recording_dst = session_path + "/" + title + ".mp3";
	}

	_idx++;

	return _sr_recording_dst;
}


bool  StreamRecorder::save() {

    if(!QFile::exists(_sr_recording_dst)){
        return false;
    }

	sp_log(Log::Info) << "Finalize file " << _sr_recording_dst;

	_md.set_filepath(_sr_recording_dst);

	Tagging::setMetaDataOfFile(_md);
	_session_collector.push_back(_md);

	PlaylistParser::save_playlist(_session_playlist_name, _session_collector, true);
	
    return true;
}


QString StreamRecorder::check_session_path(const QString& sr_path) {

	bool create_session_path =_settings->get(Set::Engine_SR_SessionPath);

	if(!create_session_path) {
		return sr_path;
	}

	QString recording_dst = sr_path + QDir::separator() + _session_path;
    if(!QFile::exists(recording_dst)) {

		Helper::File::create_directories(recording_dst);
    }

	QFileInfo fi(recording_dst);
	
	if(!fi.isWritable()){
		return "";
	}

    return recording_dst;
}



void StreamRecorder::record(bool b){

	if(b == _recording) {
		return;
	}

	sp_log(Log::Debug) << "Stream recorder: activate: " << b;

    if(b){
        new_session();
    }

    else{
        save();
		_md.title = "";
		_session_collector.clear();
		_sr_recording_dst = "";
		_idx = 1;
    }

	_recording = b;
}

QString StreamRecorder::get_dst_file() const
{
	if(!_recording) return "";

    return _sr_recording_dst;
}

bool StreamRecorder::is_recording() const
{
	return _recording;
}


void StreamRecorder::playstate_changed(PlayManager::PlayState state){

	if(state == PlayManager::PlayState::Stopped){

		if(_recording){
			save();
			_md.title = "";
			_session_collector.clear();
			_sr_recording_dst = "";
			_recording = false;
			_idx = 1;
		}
	}
}
