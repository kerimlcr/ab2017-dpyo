/* Logger.h */

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

#ifndef LOGGER_H
#define LOGGER_H


#include <ostream>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QPoint>


/**
 * @brief The Log enum
 * @ingroup Helper
 */
enum class Log : unsigned char
{
	Warning,
	Error,
	Info,
	Debug
};

/**
 * @brief The Logger class
 * @ingroup Helper
 */
class Logger{

protected:
	std::ostream& _out;

public:

	Logger(std::ostream& out);
	Logger(const char* msg, std::ostream& out);

	~Logger();

	Logger& operator << (const QString& msg);
	Logger& operator << (const QChar& c);
	Logger& operator << (const QStringList& lst);
	Logger& operator << (const QByteArray& arr);
	Logger& operator << (const QPoint& point);

	template <typename T>
	Logger& operator << (const T& msg){
		_out << msg;
		return *this;
	}

	template<typename T, template <typename ELEM> class CONT>
	Logger& operator << (const CONT<T> list){

		for(const T& item : list){
			(*this) << item << ", ";
		}

		return *this;
	}
};

Logger sp_log(Log type);
Logger sp_log(Log type, const QString& module);
#endif // LOGGER_H
