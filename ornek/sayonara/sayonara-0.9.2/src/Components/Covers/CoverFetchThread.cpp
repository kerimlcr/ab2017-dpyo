/* CoverFetchThread.cpp */

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
 * CoverFetchThread.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: Lucio Carreras
 */

#include "CoverFetchThread.h"
#include "CoverLocation.h"

#include "Helper/Logger/Logger.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/FileHelper.h"

#include <QFile>
#include <QDir>

CoverFetchThread::CoverFetchThread(QObject* parent, const CoverLocation& cl, const int n_covers) :
	QObject(parent)
{
	_covers_found = 0;
	_n_covers = n_covers;
	_url = cl.search_url();

	_target_file = cl.cover_path();
}


CoverFetchThread::~CoverFetchThread() {

}

bool CoverFetchThread::start(){

	_covers_found = 0;

	if(!_url.contains("google", Qt::CaseInsensitive))
	{
		_addresses.clear();
		_addresses << _url;
		more();
		return true;
	}

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished, this, &CoverFetchThread::content_fetched);

	awa->run(_url);
	return true;

}


bool CoverFetchThread::more(){

	if(_n_covers == _covers_found || _addresses.isEmpty()){
		emit sig_finished(true);
		return false;
	}

	QString address = _addresses[0];
	AsyncWebAccess* awa = new AsyncWebAccess(this);

	if(_n_covers == 1){
		connect(awa, &AsyncWebAccess::sig_finished, this, &CoverFetchThread::single_image_fetched);
	}

	else{
		connect(awa, &AsyncWebAccess::sig_finished, this, &CoverFetchThread::multi_image_fetched);
	}


	awa->run(address);
	_addresses.pop_front();

	return true;
}




void CoverFetchThread::content_fetched(bool success){

	if(!success){
		sp_log(Log::Warning) << "Could not fetch content";
		return;
	}

	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	QByteArray website = awa->get_data();

	_addresses = calc_addresses_from_google(10, website);

	if(_addresses.isEmpty()){
		emit sig_finished(false);
		return;
	}

	more();
}


void CoverFetchThread::single_image_fetched(bool success){

	if(!success){
		sp_log(Log::Warning) << "Could not fetch cover";
		return;
	}

	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	QImage img  = awa->get_image();

	if(!img.isNull()){
		_covers_found++;
		save_and_emit_image(_target_file, img);
	}

	awa->deleteLater();
}


void CoverFetchThread::multi_image_fetched(bool success){

	if(!success){
		sp_log(Log::Warning) << "Could not fetch cover";
		return;
	}

	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	QImage img  = awa->get_image();

	if(!img.isNull()){
		QString filename, dir;
		Helper::File::split_filename(_target_file, dir, filename);

		QString cover_path = dir + "/" + QString::number(_covers_found) + "_" + filename;

		save_and_emit_image(cover_path, img);

		_covers_found++;
	}

	awa->deleteLater();
}



void CoverFetchThread::save_and_emit_image(const QString& filepath,
										   const QImage& img)
{

	QString filename = filepath;
	QString ext = Helper::File::calc_file_extension(filepath);
	if(ext.compare("gif", Qt::CaseInsensitive) == 0){
		filename = filename.left(filename.size() - 3);
		filename += "png";
	}

	filename = Helper::File::get_absolute_filename(filename);

	bool success = img.save(filename);
	if(!success){
		sp_log(Log::Warning) << "Cannot save image to " << filename;
	}

	else{
		emit sig_cover_found(filename);
	}
}


QStringList CoverFetchThread::calc_addresses(int num,
											const QByteArray& website,
											const QString& regex)
const
{

	QStringList addresses;
	if (website.isEmpty()) {
		sp_log(Log::Error) << "Cover Fetch Thread: website empty";
		return addresses;
	}

	int n_covers = 0;
	int idx=50000;


	QString website_str = QString::fromLocal8Bit(website);

	while(n_covers < num) {
		QRegExp re(regex);
		re.setMinimal(true);
		idx = re.indexIn(website_str, idx);

		if(idx == -1) break;

		QString str = re.cap(0);

		idx += str.length();
		str.remove("\"");
		addresses << str;
		n_covers++;
	}

	return addresses;

}

QStringList CoverFetchThread::calc_addresses_from_google(int num,
														const QByteArray& website)
const
{
	return calc_addresses(num, website, QString("(https://encrypted-tbn)(\\S)+(\")"));
}
