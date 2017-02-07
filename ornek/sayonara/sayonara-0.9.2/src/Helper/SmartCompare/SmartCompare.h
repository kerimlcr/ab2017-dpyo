/* SmartCompare.h */

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



#ifndef SmartCompare_H
#define SmartCompare_H
#include <QMap>
#include <QString>
#include <QList>
#include <QStringList>

class ArtistList;
class DatabaseConnector;
enum class HashLevel : quint8
{
	zero=0,
	medium,
	strong
};

struct Hash{
	float penalty;
	QString hash;
	QString org_str;

	void print() const;
};

/**
 * @brief The SmartCompare class
 * @ingroup Helper
 */
class SmartCompare
{

private:

	QStringList  _strlist;
	QMap< QString, QMap<QString, float> > _cache;
	QList<QChar> _vocals;
	QList<QChar> _consonants;
	QList<QChar> _numbers;
	QList<QChar> _special;

	QString remove_special_chars(const QString& str);
	QString remove_vocals(const QString& str);
	QString sort_letters(const QString& str);

	void equalize(QMap<QString, float>&, float min, float max);


	Hash create_hash(const QString& str, HashLevel level);

	float compare(const QString& str1, const QString& str2, HashLevel level, bool reverse=false);
	float compare_hashes(const Hash& str1, const Hash& str2, HashLevel level, bool reverse=false);

	//float find_artist(const QString&, HashLevel level);

	SmartCompare();


public:
	SmartCompare(const QStringList& strlist);
	SmartCompare(const ArtistList& strlist);

	QMap<QString, float> get_similar_strings(const QString& str);


};

#endif // SmartCompare_H
