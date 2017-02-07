/* LFMTrackChangedThread.cpp

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

#include "LFMTrackChangedThread.h"
#include "LFMWebAccess.h"
#include "LFMGlobals.h"
#include "LFMSimArtistsParser.h"

#include "Database/DatabaseHandler.h"
#include "Database/DatabaseConnector.h"

#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/Artist.h"
#include "Helper/Compressor/Compressor.h"

#include <QMap>
#include <QStringList>
#include <QUrl>


LFMTrackChangedThread::LFMTrackChangedThread(const QString& username, const QString& session_key, QObject* parent) :
	QObject(parent),
	SayonaraClass()
{

	_username = username;
	_session_key = session_key;

	ArtistList artists;
	DatabaseConnector::getInstance()->getAllArtists(artists);

	_smart_comparison = new SmartCompare(artists);
}


LFMTrackChangedThread::~LFMTrackChangedThread() {
	delete _smart_comparison; _smart_comparison = nullptr;
}

void LFMTrackChangedThread::set_session_key(const QString& session_key) {
	_session_key = session_key;
}

void LFMTrackChangedThread::set_username(const QString& username) {
	_username = username;
}



void LFMTrackChangedThread::update_now_playing(const MetaData& md) {

	_md = md;

	LFMWebAccess* lfm_wa = new LFMWebAccess();

	connect(lfm_wa, &LFMWebAccess::sig_response, this, &LFMTrackChangedThread::response_update);
	connect(lfm_wa, &LFMWebAccess::sig_error, this, &LFMTrackChangedThread::error_update);

	QString artist = _md.artist;
	QString title = _md.title;

	if(artist.trimmed().size() == 0) artist = "Unknown";
	artist.replace("&", "&amp;");

	UrlParams sig_data;
	sig_data["api_key"] = LFM_API_KEY;
	sig_data["artist"] = artist.toLocal8Bit();
	sig_data["duration"] = QString::number(_md.length_ms / 1000).toLocal8Bit();
	sig_data["method"] = QString("track.updatenowplaying").toLocal8Bit();
	sig_data["sk"] = _session_key.toLocal8Bit();
	sig_data["track"] =  title.toLocal8Bit();

	sig_data.append_signature();


	QByteArray post_data;
	QString url = lfm_wa->create_std_url_post(
				QString("http://ws.audioscrobbler.com/2.0/"),
				sig_data,
				post_data);

	lfm_wa->call_post_url(url, post_data);
}


void LFMTrackChangedThread::response_update(const QByteArray& data){
	Q_UNUSED(data)
	LFMWebAccess* lfm_wa = static_cast<LFMWebAccess*>(sender());
	lfm_wa->deleteLater();
}


void LFMTrackChangedThread::error_update(const QString& error){
	LFMWebAccess* lfm_wa = static_cast<LFMWebAccess*>(sender());

	sp_log(Log::Warning) << "Last.fm: Cannot update track";
	sp_log(Log::Warning) << "Last.fm: " << error;

	lfm_wa->deleteLater();
}




void LFMTrackChangedThread::search_similar_artists(const MetaData& md) {

	if(md.db_id != 0) {
		return;
	}

	if(md.radio_mode() == RadioMode::Station){
		return;
	}

	_md = md;

	if(_md.artist.trimmed().isEmpty()){
		return;
	}

	// check if already in cache
	if(_sim_artists_cache.contains(_md.artist)) {
		const ArtistMatch& artist_match = _sim_artists_cache.value(_md.artist);
		evaluate_artist_match(artist_match);
		return;
	}

	_artist = md.artist;

	LFMWebAccess* lfm_wa = new LFMWebAccess();

	connect(lfm_wa, &LFMWebAccess::sig_response, this, &LFMTrackChangedThread::response_sim_artists);
	connect(lfm_wa, &LFMWebAccess::sig_error, this, &LFMTrackChangedThread::error_sim_artists);

	QString url = 	QString("http://ws.audioscrobbler.com/2.0/?");
	QString encoded = QUrl::toPercentEncoding( _md.artist );
	url += QString("method=artist.getsimilar&");
	url += QString("artist=") + encoded + QString("&");
	url += QString("api_key=") + LFM_API_KEY;

	lfm_wa->call_url(url);
	return;
}


void LFMTrackChangedThread::evaluate_artist_match(const ArtistMatch& artist_match){

	if(!artist_match.is_valid()){
		return;
	}

	QByteArray arr = Compressor::compress(artist_match.to_string().toLocal8Bit());
	Helper::File::create_directories(Helper::get_sayonara_path() + "/similar_artists/");
	Helper::File::write_file(arr, Helper::get_sayonara_path() + "/similar_artists/" + artist_match.get_artist_name() + ".comp");

	// if we always take the best, it's boring
	ArtistMatch::Quality quality, quality_org;

	int rnd_number = Helper::get_random_number(1, 999);

	if(rnd_number > 350) {
		quality = ArtistMatch::Quality::Very_Good;	// [250-999]
	}

	else if(rnd_number > 75){
		quality = ArtistMatch::Quality::Well;		// [50-250]
	}

	else {
		quality = ArtistMatch::Quality::Poor;
	}

	quality_org = quality;
	QMap<QString, int> possible_artists;

	while(possible_artists.isEmpty()) {

		possible_artists = filter_available_artists(artist_match, quality);

		switch(quality){
			case ArtistMatch::Quality::Poor:
				quality = ArtistMatch::Quality::Very_Good;
				break;
			case ArtistMatch::Quality::Well:
				quality = ArtistMatch::Quality::Poor;
				break;
			case ArtistMatch::Quality::Very_Good:
				quality = ArtistMatch::Quality::Well;
				break;
			default: // will never be executed
				quality = quality_org;
				break;
		}

		if(quality == quality_org) {
			break;
		}
	}

	if(possible_artists.isEmpty()){
		return;
	}

	IDList chosen_ids;
	for(auto it = possible_artists.begin(); it != possible_artists.end(); it++) {
		chosen_ids << it.value();
	}

	emit sig_similar_artists_available(chosen_ids);
}


QMap<QString, int> LFMTrackChangedThread::filter_available_artists(const ArtistMatch& artist_match, ArtistMatch::Quality quality) {

	DatabaseConnector* db = DatabaseConnector::getInstance();
	QMap<ArtistMatch::ArtistDesc, double> bin = artist_match.get(quality);
	QMap<QString, int> possible_artists;

	for(const ArtistMatch::ArtistDesc& key : bin.keys()) {

#if SMART_COMP

		QMap<QString, float> sc_map = _smart_comparison->get_similar_strings(key);
		for(const QString& sc_key : sc_map.keys() ){
			int artist_id = db->getArtistID(sc_key);
			if(artist_id >= 0 && sc_map[sc_key] > 5.0f){

				possible_artists[sc_key] = artist_id;
			}

		}

#else
		int artist_id = db->getArtistID(key.artist_name);
		if(artist_id >= 0 ){

			possible_artists[key.artist_name] = artist_id;
		}

#endif
	}

	return possible_artists;
}


void LFMTrackChangedThread::response_sim_artists(const QByteArray& data){

	LFMSimArtistsParser parser(_artist, data);

	ArtistMatch artist_match = parser.get_artist_match();

	if(artist_match.is_valid()){
		QString artist_name = artist_match.get_artist_name();
		_sim_artists_cache[artist_name] = artist_match;
	}

	evaluate_artist_match(artist_match);
}

void LFMTrackChangedThread::error_sim_artists(const QString& error){
	sp_log(Log::Warning) << "Last.fm: Search similar artists" << error;
}
