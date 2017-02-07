/* AbstractDatabase.h */

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



#ifndef ABSTRACTDATABASE_H
#define ABSTRACTDATABASE_H

#include <QSqlDatabase>
#include <QFile>
#include <QDir>
#include <QSqlError>
#include <QMap>
#include <QList>


class AbstractDatabase : public QObject
{

public:
	explicit AbstractDatabase(quint8 db_id, const QString& db_dir, const QString& db_name, QObject *parent=nullptr);
	virtual ~AbstractDatabase();

	virtual void close_db();
	virtual bool is_initialized();

	virtual void transaction();
	virtual void commit();
	virtual void rollback();

	static void remove_connections();

	quint8 get_id();


protected:
	QSqlDatabase	_database;
	QString			_db_path;
	QString			_db_name;
	QString			_db_dir;
	quint8			_db_id;

	/**
	  * Check if db of m_databaseFileContainer is existent
	  * @return true if we can load the db false if not
	  */
	bool _initialized;

	virtual bool exists();
	virtual bool create_db();
	virtual bool open_db();
	virtual bool apply_fixes()=0;

	virtual bool check_and_insert_column(QString tablename, QString column, QString sqltype);
	virtual bool check_and_create_table(QString tablename, QString sql_create_str);
	virtual bool check_and_drop_table(QString tablename);
};

#endif // ABSTRACTDATABASE_H
