/* DatabaseStreams.cpp */

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


#include "Database/SayonaraQuery.h"
#include "Database/DatabaseStreams.h"

DatabaseStreams::DatabaseStreams(QSqlDatabase db, quint8 db_id) :
	DatabaseModule(db, db_id)
{
}



bool DatabaseStreams::getAllStreams(QMap<QString, QString>& streams) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	streams.clear();

	SayonaraQuery q (_db);
	q.prepare("SELECT name, url FROM savedstreams;");

	if (!q.exec()){
		q.show_error("Cannot get all streams");
		return false;
	}

	while(q.next()) {
		QString name = q.value(0).toString();
		QString url = q.value(1).toString().trimmed();

		streams[name] = url;
	}

	return true;
}



bool DatabaseStreams::deleteStream(const QString& name) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	q.prepare("DELETE FROM savedstreams WHERE name = :name;" );
	q.bindValue(":name", name);

	if(!q.exec()) {
		q.show_error(QString("Could not delete stream ") + name);
		return false;
	}

	return true;
}


bool DatabaseStreams::addStream(const QString& name, const QString& url) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);

	q.prepare("INSERT INTO savedstreams (name, url) VALUES (:name, :url); " );
	q.bindValue(":name", name);
	q.bindValue(":url", url);

	if(!q.exec()) {
		q.show_error(QString("Could not add stream ") + name);
		return false;
	}

	return true;
}

bool DatabaseStreams::renameStream(const QString& name, const QString& url)
{
	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);

	q.prepare("UPDATE savedstreams SET name=:name WHERE url=:url;");
	q.bindValue(":name", name);
	q.bindValue(":url", url);

	if(!q.exec()) {
		q.show_error(QString("Could not rename stream ") + name);
		return false;
	}

	return true;
}



bool DatabaseStreams::updateStreamUrl(const QString& name, const QString& url)
{
	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);

	q.prepare("UPDATE savedstreams SET url=:url WHERE name=:name;");
	q.bindValue(":name", name);
	q.bindValue(":url", url);

	if(!q.exec()) {
		q.show_error(QString("Could not update stream url ") + name);
		return false;
	}

	return true;
}


