/* SayonaraQuery.cpp */

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
#include "Helper/Logger/Logger.h"

SayonaraQuery::SayonaraQuery(const QString& query, QSqlDatabase db) :
	QSqlQuery(query, db)
{
	_query_string = query;
}

SayonaraQuery::SayonaraQuery(QSqlResult * result) :
	QSqlQuery(result)
{
}


SayonaraQuery::SayonaraQuery(QSqlDatabase db) :
	QSqlQuery(db)
{
}

SayonaraQuery::SayonaraQuery(const QSqlQuery & other) :
	QSqlQuery(other)
{
}

SayonaraQuery::~SayonaraQuery(){
}

bool SayonaraQuery::prepare(const QString& query){


	_query_string = query;
	return QSqlQuery::prepare(query);
}

void SayonaraQuery::bindValue(const QString& placeholder, const QVariant& val, QSql::ParamType param_type ){

	QString replace_str;

	if(val.canConvert<int>()){
		replace_str=QString::number(val.toInt());
	}

	else{
		replace_str = QString("'") + val.toString() + "'";
	}

	_query_string.replace(placeholder + " ", replace_str + " ");
	QSqlQuery::bindValue(placeholder, val, param_type);
}

//#define DB_DEBUG

#ifdef DB_DEBUG
	#include <QTime>
#endif

bool SayonaraQuery::exec()
{
#ifdef DB_DEBUG
	QTime timer;
	timer.start();
#endif

	bool success = QSqlQuery::exec();

#ifdef DB_DEBUG

	sp_log(Log::Debug) << _query_string << ": " << timer.elapsed() << "ms";
#endif

	return success;

}


QString SayonaraQuery::get_query_string() const {
	return _query_string;
}


void SayonaraQuery::show_error(const QString& err_msg) const{
	sp_log(Log::Error) << "SQL ERROR: " << err_msg;
	sp_log(Log::Error) << this->lastError().text();
	sp_log(Log::Error) << this->get_query_string();
}
