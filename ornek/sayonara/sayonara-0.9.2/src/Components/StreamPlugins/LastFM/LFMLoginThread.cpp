/* LFMLoginThread.cpp */

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



#include "LFMLoginThread.h"
#include "LFMGlobals.h"
#include "LFMWebAccess.h"
#include "Helper/Helper.h"
#include "GUI/Helper/Message/Message.h"

LFMLoginStuff::LFMLoginStuff(){
	logged_in = false;
	subscriber = false;
}

LFMLoginThread::LFMLoginThread(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{

}

LFMLoginThread::~LFMLoginThread() {}

void LFMLoginThread::get_token() {

	LFMWebAccess* lfm_wa = new LFMWebAccess();
	connect(lfm_wa, &LFMWebAccess::sig_response, this, &LFMLoginThread::wa_response_token);
	connect(lfm_wa, &LFMWebAccess::sig_error, this, &LFMLoginThread::wa_error_token);

    UrlParams signature_data;
        signature_data["api_key"] = LFM_API_KEY;
        signature_data["method"] = "auth.gettoken";

		signature_data.append_signature();


	QString url = lfm_wa->create_std_url(QString("https://ws.audioscrobbler.com/2.0/"), signature_data);

	lfm_wa->call_url(url);
}


void LFMLoginThread::wa_response_token(const QByteArray& data){

	QString str = QString::fromUtf8(data);
	LFMWebAccess* lfm_wa = static_cast<LFMWebAccess*>(sender());
	QString token = Helper::easy_tag_finder("lfm.token", str);

	if(token.size() != 32) {
		sp_log(Log::Warning) << "Last.fm: Invalid token = " << token;
		return;
	}

	_login_info.token = token;

	lfm_wa->deleteLater();

	request_authorization();
}


void LFMLoginThread::wa_error_token(const QString& error){

	LFMWebAccess* lfm_wa =  static_cast<LFMWebAccess*>(sender());

	sp_log(Log::Warning) << "LFM: could not call login url " << error;
	_login_info.logged_in = false;

	lfm_wa->deleteLater();
}


bool LFMLoginThread::request_authorization() {

    UrlParams signature_data;
        signature_data["api_key"] = LFM_API_KEY;
		signature_data["token"] = _login_info.token.toLocal8Bit();

	sp_log(Log::Debug) << "auth: token = " << _login_info.token;

	QString url = LFMWebAccess::create_std_url("https://www.last.fm/api/auth/", signature_data);

	QString message_str =  QString("<b>First login to Last.fm from Sayonara</b><br /><br />") +
			"You will be redirected to this link when clicking on it<br /><br />" +
			"<a href=\"" + url + "\">"+ url + "</a><br /><br />" +
			"When you finished authorization, click OK";

	GlobalMessage::Answer answer = Message::question_ok(message_str, "Last.fm");

	if(answer == GlobalMessage::Answer::Ok) {
		return true;
	}

    return false;
}



void LFMLoginThread::login(const QString& username, const QString& password) {

	LFMWebAccess* lfm_wa = new LFMWebAccess();
	connect(lfm_wa, &LFMWebAccess::sig_response, this, &LFMLoginThread::wa_response);
	connect(lfm_wa, &LFMWebAccess::sig_error, this, &LFMLoginThread::wa_error);

	_login_info.logged_in = false;
	_login_info.session_key = "";
	_login_info.subscriber = false;


    UrlParams signature_data;
        signature_data["api_key"] = LFM_API_KEY;
		signature_data["method"] = "auth.getMobileSession";
		signature_data["password"] = password.toLocal8Bit();
		signature_data["username"] = username.toLocal8Bit();

		signature_data.append_signature();

	QByteArray post_data;
	QString url = lfm_wa->create_std_url_post("https://ws.audioscrobbler.com/2.0/", signature_data, post_data);

	lfm_wa->call_post_url(url, post_data);
}


void LFMLoginThread::wa_response(const QByteArray& data){

	LFMWebAccess* lfm_wa = static_cast<LFMWebAccess*>(sender());
	QString str = QString::fromUtf8(data);

	_login_info.logged_in = true;
	_login_info.session_key = Helper::easy_tag_finder("lfm.session.key", str);

	sp_log(Log::Debug) << "Last.fm Got session key: " << _login_info.session_key;

	_login_info.subscriber = (Helper::easy_tag_finder("lfm.session.subscriber", str).toInt() == 1);
	_login_info.error = str;

	if(_login_info.session_key.size() >= 32){
		emit sig_logged_in(true);
	}

	else {
		emit sig_logged_in(false);
	}

	lfm_wa->deleteLater();
}


void LFMLoginThread::wa_error(const QString& error){

	LFMWebAccess* lfm_wa = static_cast<LFMWebAccess*>(sender());

	sp_log(Log::Warning) << "LastFM: Cannot login";
	sp_log(Log::Warning) << error;

	emit sig_error(error);

	lfm_wa->deleteLater();
}


LFMLoginStuff LFMLoginThread::getLoginStuff() {
    return _login_info;
}

