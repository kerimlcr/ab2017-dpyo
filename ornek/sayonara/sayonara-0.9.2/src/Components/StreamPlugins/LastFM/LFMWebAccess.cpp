/* LFMWebAccess.cpp */

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
 * LFMWebAccess.cpp
 *
 *  Created on: Oct 22, 2011
 *      Author: Lucio Carreras
 */

#include "LFMWebAccess.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/Helper.h"

#include <QCryptographicHash>




void LFMWebAccess::call_url(const QString& url) {

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished, this, &LFMWebAccess::awa_finished);
	awa->run(url, 10000);
}

void LFMWebAccess::call_post_url(const QString& url, const QByteArray& post_data) {

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished, this, &LFMWebAccess::awa_finished);

	QMap<QByteArray, QByteArray> header;
	header["Content-Type"] = "application/x-www-form-urlencoded";

	awa->set_raw_header(header);
	awa->run_post(url, post_data, 10000);
}


void LFMWebAccess::call_post_url_https(const QString& url, const QByteArray& post_data) {

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished, this, &LFMWebAccess::awa_finished);
	awa->run_post(url, post_data, 10000);
}


void LFMWebAccess::awa_finished(bool success){

	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	QByteArray data = awa->get_data();

	if(check_error(data, success)){
		return;
	}

	emit sig_response(data);
}



QString LFMWebAccess::parse_session_answer(const QString& content) {

	QString str_key = "";
	QDomDocument doc("answer");
	doc.setContent(content);

	QDomNodeList nodeList =  doc.documentElement().elementsByTagName("session");

	for(int i=0; i<nodeList.size(); i++) {
		QDomNodeList nl_session = nodeList.at(i).toElement().elementsByTagName("key");
		bool found = false;

		for(int j=0; j<nl_session.size(); j++) {
			str_key = nl_session.at(j).toElement().text();
			if(str_key.size() != 0) {
				found = true;
				break;
			}
		}

		if(found) break;
	}

	doc.clear();
	return str_key;
}



QString LFMWebAccess::LFMWebAccess::create_std_url(const QString& base_url, const UrlParams& data) {
	QByteArray post_data;

	QString url = create_std_url_post(base_url, data, post_data);
	url += "?";
	url += QString::fromLocal8Bit(post_data);

	return url;
}

QString LFMWebAccess::create_std_url_post(const QString& base_url, const UrlParams& sig_data, QByteArray& post_data) {

	QString url = base_url;

	post_data.clear();

	for(const QByteArray& key : sig_data.keys()) {

		QByteArray entry = key + "=" + sig_data[key];
		entry.replace("&", "%26");
		entry += QChar('&');

		post_data += entry;
	}

	post_data.remove(post_data.size() - 1, 1);

	return url;
}



bool LFMWebAccess::check_error(const QByteArray& data, bool success){

	QString error_str = parse_error_message(data);
	if(!error_str.isEmpty()){
		emit sig_error(error_str);
		return true;
	}

	else if(!success){
		return true;
	}

	return false;
}

QString LFMWebAccess::parse_error_message(const QString& response) {

	if(response.isEmpty()){
		return "";
	}

	if(response.left(100).contains("failed")) {
		return Helper::easy_tag_finder("lfm.error", response).trimmed();
	}

	return "";
}


UrlParams::UrlParams() : 
	QMap<QByteArray, QByteArray>()
{}

void UrlParams::append_signature()
{
	QByteArray signature;

    for(const QByteArray& key : this->keys()) {

        signature += key;
        signature += this->value(key);
    }

    signature += LFM_API_SECRET;

    QByteArray hash = Helper::calc_hash(signature);

    this->insert("api_sig", hash);

}
