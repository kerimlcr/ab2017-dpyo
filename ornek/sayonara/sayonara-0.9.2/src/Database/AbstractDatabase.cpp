/* AbstractDatabase.cpp */

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



#include "Database/AbstractDatabase.h"
#include "Database/DatabaseModule.h"
#include "Database/SayonaraQuery.h"

#include "Helper/Helper.h"
#include "Helper/Logger/Logger.h"


AbstractDatabase::AbstractDatabase(quint8 db_id, const QString& db_dir, const QString& db_name, QObject *parent) : QObject(parent)
{
	_db_name = db_name;
	_db_path = Helper::get_sayonara_path() +_db_name;

	_db_id = db_id;
	_db_dir = db_dir;

	_initialized = exists();

	if(!_initialized) {

		sp_log(Log::Warning) << "Database not existent. Creating database";
		_initialized = create_db();
	}
}

AbstractDatabase::~AbstractDatabase()
{
	close_db();

	for(const QString& connection_name : QSqlDatabase::connectionNames()){
		QSqlDatabase::removeDatabase(connection_name);
	}
}

void AbstractDatabase::remove_connections(){
	for(const QString& connection : QSqlDatabase::connectionNames()) {
		QSqlDatabase::removeDatabase(connection);
	}
}

bool AbstractDatabase::is_initialized(){
	return _initialized;
}


bool AbstractDatabase::open_db(){

	_database = QSqlDatabase::addDatabase("QSQLITE", _db_path);
	_database.setDatabaseName( _db_path );

	bool success = _database.open();
	if (!success) {
		sp_log(Log::Error) << "DatabaseConnector database cannot be opened!";
		QSqlError er = _database.lastError();
		sp_log(Log::Error) << er.driverText();
		sp_log(Log::Error) << er.databaseText();
	}

	return success;
}

void AbstractDatabase::close_db() {

	if(!_database.isOpen()){
		return;
	}

	sp_log(Log::Info) << "close database " << _db_name << "...";

	_database.close();
}


bool AbstractDatabase::exists() {

	bool success;
	success = QFile::exists(_db_path);
	if(!success) {
		success = create_db();

		if(!success) {
			sp_log(Log::Error) << "Database could not be created";
			return false;
		}

		else{
			sp_log(Log::Info) << "Database created successfully";
		}
	}

	success = open_db();

	if (success){
		_database.close();
	}

	else{
		sp_log(Log::Error) << "Could not open Database";
	}

	return success;
}



bool AbstractDatabase::create_db() {

	bool success;
	QDir dir = QDir::homePath();

	QString sayonara_path = Helper::get_sayonara_path();
	if(!QFile::exists(sayonara_path)) {
		success = dir.mkdir(".Sayonara");
		if(!success) {
			sp_log(Log::Error) << "Could not create .Sayonara dir";
			return false;
		}

		else{
			sp_log(Log::Info) << "Successfully created .Sayonara dir";
		}
	}

	success = dir.cd(sayonara_path);

	//if ret is still not true we are not able to create the directory
	if(!success) {
		sp_log(Log::Error) << "Could not change to .Sayonara dir";
		return false;
	}

	QString source_db_file = Helper::get_share_path() + "/" + _db_dir + "/" +_db_name;

	success = QFile::exists(_db_path);

	if(success) {
		return true;
	}

	if (!success) {

		sp_log(Log::Info) << "Database " << _db_path << " not existent yet";
		sp_log(Log::Info) << "Copy " <<  source_db_file << " to " << _db_path;

		if (QFile::copy(source_db_file, _db_path)) {
			sp_log(Log::Info) << "DB file has been copied to " <<   _db_path;
			success = true;
		}

		else {
			sp_log(Log::Error) << "Fatal Error: could not copy DB file to " << _db_path;
			success = false;
		}
	}

	return success;
}


void AbstractDatabase::transaction(){
	DB_RETURN_NOT_OPEN_VOID(_database);
	_database.transaction();
}

void AbstractDatabase::commit(){
	DB_RETURN_NOT_OPEN_VOID(_database);
	_database.commit();
}

void AbstractDatabase::rollback(){
	DB_RETURN_NOT_OPEN_VOID(_database);
	_database.rollback();
}




bool AbstractDatabase::check_and_drop_table(QString tablename) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	SayonaraQuery q(_database);
	QString querytext = "DROP TABLE " +  tablename + ";";
	q.prepare(querytext);

	if(!q.exec()){
		q.show_error(QString("Cannot drop table ") + tablename);
		return false;
	}

	return true;
}

bool AbstractDatabase::check_and_insert_column(QString tablename, QString column, QString sqltype) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	SayonaraQuery q (_database);
	QString querytext = "SELECT " + column + " FROM " + tablename + ";";
	q.prepare(querytext);

	if(!q.exec()) {

		SayonaraQuery q2 (_database);
		querytext = "ALTER TABLE " + tablename + " ADD COLUMN " + column + " " + sqltype + ";";
		q2.prepare(querytext);

		if(!q2.exec()){;
			q.show_error(QString("Cannot insert column ") + column + " into " + tablename);
			return false;
		}

		return true;
	}

	return true;
}

bool AbstractDatabase::check_and_create_table(QString tablename, QString sql_create_str) {

	DB_RETURN_NOT_OPEN_BOOL(_database);

	SayonaraQuery q (_database);
	QString querytext = "SELECT * FROM " + tablename + ";";
	q.prepare(querytext);

	if(!q.exec()) {

		SayonaraQuery q2 (_database);
		q2.prepare(sql_create_str);

		if(!q2.exec()){
			q.show_error(QString("Cannot create table ") + tablename);
		}
	}

	return true;
}

quint8 AbstractDatabase::get_id(){
	return _db_id;
}
