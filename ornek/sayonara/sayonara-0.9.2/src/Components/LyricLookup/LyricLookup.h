/* LyricLookup.h */

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
 * LyricLookup.h
 *
 *  Created on: May 21, 2011
 *      Author: Lucio Carreras
 */

#ifndef LYRICLOOKUP_H_
#define LYRICLOOKUP_H_

#include <QObject>
#include <QStringList>
#include <QString>
#include <QList>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QMap>

struct ServerTemplate;

/**
 * @brief The LyricLookupThread class
 * @ingroup Lyrics
 */
class LyricLookupThread : public QObject {

	Q_OBJECT

signals:
	void sig_finished();

public:
	LyricLookupThread(QObject* parent=nullptr);
	virtual ~LyricLookupThread();

	QStringList get_servers();
	QString get_lyric_data();

	void run(const QString& artist, const QString& title, int server_idx);


private:
	QString					_artist;
	QString					_title;
	int						_cur_server;
	QList<ServerTemplate>	_server_list;
	QString					_final_wp;
	QMap<QString, QString>  _regex_conversions;

	QString convert_to_regex(const QString& str);
	void init_server_list();
	QString parse_webpage(const QByteArray& raw);
	QString calc_url(QString artist, QString song);

	void parse_xml();


private slots:
	void content_fetched(bool success);
};

#endif /* LYRICLOOKUP_H_ */
