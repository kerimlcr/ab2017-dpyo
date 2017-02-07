/* AsyncWebAccess.h */

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



#ifndef ASYNCWEBACCESS_H_
#define ASYNCWEBACCESS_H_

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QByteArray>
#include <QImage>
#include <QTimer>

/**
 * @brief Asynchgronous web access class
 * @ingroup Helper
 */
class AsyncWebAccess : public QObject{

	Q_OBJECT

signals:

	void sig_finished(bool success);

public:

	/**
	 * @brief The Behavior enum. Responsible for the user-agent variable in the HTTP header
	 */
	enum class Behavior : quint8 {
		AsBrowser=0,
		AsSayonara
	};

	/**
	 * @brief AsyncWebAccess constructor
	 * @param parent standard QObject parent
	 * @param header a modified header, see set_raw_header(const QByteArray& header)
	 */
	AsyncWebAccess(QObject* parent=nullptr, const QByteArray& header=QByteArray(),
				   AsyncWebAccess::Behavior behavior=AsyncWebAccess::Behavior::AsBrowser);

	virtual ~AsyncWebAccess();

	/**
	 * @brief get fetched data
	 * @return fetched data
	 */
	QByteArray get_data() const;

	/**
	 * @brief get fetched data formatted as image
	 * @return image
	 */
	QImage get_image() const;

	/**
	 * @brief get last called url. \n
	 * This url may differ from the originally called url when request has been redirected.
	 * @return url string
	 */
	QString	get_url() const;

	/**
	 * @brief Set the behaviour how sayonara should be recognized by the server. This variable will set
	 * the user-agent string in the http header
	 * @param behavior
	 */
	void set_behavior(AsyncWebAccess::Behavior behavior);


	/**
	 * @brief starts a GET request
	 * @param url url to call
	 * @param timeout timeout until request is aborted and error is emitted
	 */
	void run(const QString& url, int timeout=4000);

	/**
	 * @brief starts a POST request
	 * @param url url to call
	 * @param post_data QByteArray formatted postdata containing ?, = and & characters
	 * @param timeout timeout until request is aborted and error is emitted
	 */
	void run_post(const QString& url, const QByteArray& post_data, int timeout=4000);

	/**
	 * @brief modify header.
	 * @param header new header field. e.g. "Content-Type" "text/css"
	 */
	void set_raw_header(const QMap<QByteArray, QByteArray>& header);


private slots:

	/**
	 * @brief Called when request has finished. Emits sig_finished(bool success)
	 * @param reply information about redirection, success or errors
	 */
	void finished(QNetworkReply* reply);

	/**
	 * @brief Request has timed out. Emits sig_finished(false);\n
	 * finished(QNetworkReply *reply) is not called anymore
	 */
	void timeout();

private:

	QNetworkAccessManager*	_nam=nullptr;
	QString					_url;
	QByteArray				_data;
	AsyncWebAccess::Behavior _behavior;


	QTimer*					_timer=nullptr;
	QNetworkReply*			_reply=nullptr;

	QMap<QByteArray, QByteArray> _header;

};


#endif
