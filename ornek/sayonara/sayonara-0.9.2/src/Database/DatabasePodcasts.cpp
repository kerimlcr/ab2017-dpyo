/* DatabasePodcasts.cpp */

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
#include "Database/DatabasePodcasts.h"
#include "Helper/Logger/Logger.h"

DatabasePodcasts::DatabasePodcasts(QSqlDatabase db, quint8 db_id) :
	DatabaseModule(db, db_id)
{
}



bool DatabasePodcasts::getAllPodcasts(QMap<QString, QString> & podcasts) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	podcasts.clear();

	SayonaraQuery q (_db);
	q.prepare("SELECT name, url FROM savedpodcasts;");

	if (!q.exec()){
		q.show_error("Cannot fetch podcasts");
		return false;
	}

	while(q.next()) {
		QString name = q.value(0).toString();
		QString url = q.value(1).toString();

		podcasts[name] = url;
	}

	return true;
}



bool DatabasePodcasts::deletePodcast(const QString& name) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);

	q.prepare("DELETE FROM savedpodcasts WHERE name = :name;" );
	q.bindValue(":name", name);

	if(!q.exec()) {
		q.show_error(QString("Could not delete podcast ") + name);
		return false;
	}

	return true;
}




bool DatabasePodcasts::addPodcast(const QString& name, const QString& url) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	q.prepare("INSERT INTO savedpodcasts (name, url) VALUES (:name, :url); " );
	q.bindValue(":name", name);
	q.bindValue(":url", url);

	if(!q.exec()) {
		sp_log(Log::Warning) << "Could not add podcast " << name << ", " << url;
		return false;
	}

	sp_log(Log::Info) << "podcast " << name << ", " << url << " successfully added";
	return true;
}

bool DatabasePodcasts::renamePodcast(const QString& name, const QString& url)
{
	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);

	q.prepare("UPDATE savedpodcasts SET name=:name WHERE url=:url;");
	q.bindValue(":name", name);
	q.bindValue(":url", url);

	if(!q.exec()) {
		q.show_error(QString("Could not rename podcast ") + name);
		return false;
	}

	return true;
}



bool DatabasePodcasts::updatePodcastUrl(const QString& name, const QString& url)
{
	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);

	q.prepare("UPDATE savedpodcasts SET url=:url WHERE name=:name;");
	q.bindValue(":name", name);
	q.bindValue(":url", url);

	if(!q.exec()) {
		q.show_error(QString("Could not update podcast url ") + name);
		return false;
	}

	return true;
}



