/* Helper.h */

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
 * Helper.cpp
 *
 *  Created on: Apr 4, 2011
 *      Author: Lucio Carreras
 */


#ifndef _HELPER_H
#define _HELPER_H

#include <QDir>
#include <QUrl>
#include <QCryptographicHash>
#include <QFile>
#include <QFontMetrics>
#include <QFileInfo>
#include <QMap>

class MetaData;
class QDateTime;
/**
 * @brief Helper functions
 * @ingroup Helper
 */
namespace Helper {

	class StringDummy : public QObject
	{
		Q_OBJECT

	public:
		StringDummy(QObject* parent=nullptr) :
			QObject(parent)
		{

		}

		QString various();
		QString days();
		QString hours();
		QString minutes();
		QString seconds();
		QString disc();
		QString discs();
	};

	quint64 current_date_to_int();
	quint64 date_to_int(const QDateTime& date);

	/**
	 * @brief Transform all letters after a space to upper case
	 * @param str input string
	 * @return result string
	 */
	QString cvt_str_to_first_upper(const QString& str);

	/**
	 * @brief Transform only first letter to upper case
	 * @param str input string
	 * @return result string
	 */
	QString cvt_str_to_very_first_upper(const QString& str);

	/**
	 * @brief Convert milliseconds to string
	 * @param msec milliseconds
	 * @param empty_zero if false, prepend a zero to numbers < 10
	 * @param colon if true, set colon between minutes and seconds
	 * @param show_days if true, days will be shown, too
	 * @return converted milliseconds
	 */
	QString cvt_ms_to_string(quint64 msec, bool empty_zero = false, bool colon=true, bool show_days=true);



	/**
	 * @brief get error log file
	 * @return
	 */
	QString get_error_file();

	/**
	 * @brief get sayonara path in home directory
	 * @return
	 */
	QString get_sayonara_path();

	/**
	 * @brief get share path of sayonara
	 * @return ./share on windows, share path of unix system
	 */
	QString get_share_path();

	/**
	 * @brief get library path of sayonara
	 * @return ./lib on windows, lib path of unix system
	 */
	QString get_lib_path();

	/**
	 * @brief create a link string
	 * @param name appearing name in link
	 * @param target target url (if not given, name is taken)
	 * @param underline if link should be underlined
	 * @return link string
	 */
	QString create_link(const QString& name,
						bool dark=true,
						const QString& target="",
						bool underline=true);


	/**
	 * @brief get all supported sound file extensions
	 * @return
	 */
	QStringList get_soundfile_extensions();

	/**
	 * @brief get all supported playlist file extensions
	 * @return
	 */
	QStringList get_playlistfile_extensions();

	/**
	 * @brief get all supported podcast file extensions
	 * @return
	 */
    QStringList get_podcast_extensions();


	/**
	 * @brief get a random val between min max
	 * @param min minimum included value
	 * @param max maximum included value
	 * @return random number
	 */
	int get_random_number(int min, int max);


	/**
	 * @brief gets value out of tag
	 * @param tag form: grandparent.parent.child
	 * @param xml_doc content of the xml document
	 * @return extracted string
	 */
	QString easy_tag_finder(const QString&  tag, const QString& xml_doc);

	/**
	 * @brief calculate a md5 hashsum
	 * @param data input data
	 * @return hashsum
	 */
	QByteArray calc_hash(const QByteArray&  data);



	/**
	 * @brief sleep
	 * @param ms milliseconds to sleep
	 */
	void sleep_ms(quint64 ms);

	/**
	 * @brief get all ip addresses of the host
	 * @return list of ip addresses
	 */
	QStringList get_ip_addresses();


	/**
	 * @brief set an environment variable. This function is platform independent
	 * @param key variable name
	 * @param value variable value
	 */
	void set_environment(const QString& key, const QString& value);

}



#endif
