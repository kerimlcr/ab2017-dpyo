/* MetaData.h */

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
 * MetaData.h
 *
 *  Created on: Mar 10, 2011
 *      Author: Lucio Carreras
 */

#ifndef METADATA_H_
#define METADATA_H_

#include "Helper/MetaData/LibraryItem.h"

#include <QMetaType>
#include <QStringList>
#include <QString>

/**
 * @brief The RadioMode enum
 * @ingroup MetaDataHelper
 */
enum class RadioMode : quint8
{

	Off = 0,
	Station,
	Soundcloud
};

class MetaData :
		public LibraryItem
{

private:
	QString _filepath;
	RadioMode _radio_mode;

public:
	qint32 id;
	qint32 album_id;
	qint32 artist_id;
	QString title;
	QString artist;
	QString album;          // if radio station: name of radio station
	QStringList genres;
	quint8 rating;
	quint64 length_ms;
	quint16 year;

	quint16 track_num;
	quint32 bitrate;
	quint64 filesize;
	//QString comment;
	quint8 discnumber;
	quint8 n_discs;

	bool played;
	bool is_extern;
	bool pl_playing;

	bool is_disabled;

	MetaData ();
	MetaData (const MetaData& );
	MetaData ( MetaData&& );
	MetaData (const QString& path);
	virtual ~MetaData();

	QUrl url() const;
	QString filepath() const;
	QString set_filepath(QString filepath);

	RadioMode radio_mode() const;

	MetaData& operator=(const MetaData& md);
	bool operator==(const MetaData& md) const;
	bool operator!=(const MetaData& md) const;
	bool is_equal(const MetaData& md) const;
	bool is_equal_deep(const MetaData& md) const;

	QString to_string() const;

	static QVariant toVariant(const MetaData& md);
	static bool fromVariant(const QVariant& v, MetaData& md);
};

Q_DECLARE_METATYPE(MetaData)




#endif /* METADATA_H_ */
