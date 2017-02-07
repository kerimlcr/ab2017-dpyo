/* Logger.cpp */

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

#include "Helper/Logger/Logger.h"

#include <iostream>


Logger sp_log(Log type, const QString& module){

	QString type_str;
	switch(type){

	case Log::Info:
		break;
	case Log::Debug:
		type_str = "Debug";
		break;
	case Log::Warning:
		type_str = "Warning";
		break;
	case Log::Error:
		type_str = "Error";
		break;

	default:
		type_str = "Debug";
		break;
	}

	if(!module.isEmpty()){
		type_str += ": " + module;
	}

	if(!type_str.isEmpty()){
		type_str += ": ";
	}

	return Logger(type_str.toLocal8Bit(), std::clog);
}


Logger sp_log(Log type){
	return sp_log(type, QString());
}


Logger::Logger(std::ostream& out) :	_out(out)
{

}

Logger::Logger(const char* msg, std::ostream& out) :
	Logger(out)
{
	_out << msg;
}


Logger::~Logger(){
	_out << std::endl;
	_out.flush();
}

Logger& Logger::operator << (const QString& msg){
	(*this) << msg.toLocal8Bit().constData();
	return *this;
}

Logger& Logger::operator << (const QStringList& lst){
	for(const QString& str : lst){
		(*this) << str << ", ";
	}

	return *this;
}


Logger& Logger::operator << (const QChar& c){

	(*this) << c.toLatin1();


	return *this;
}

Logger& Logger::operator << (const QPoint& point){
	(*this) << "Point(" << point.x() << "," << point.y() << ")";
	return *this;
}

Logger& Logger::operator << (const QByteArray& arr){

	_out << std::endl;
	QString line_str;

	for(int i=0; i<arr.size(); i++){

		char c = arr[i];

		QChar qc = QChar(c);

		if(qc.isPrint()){
			line_str += qc;
		}

		else{
			line_str += ".";
		}

		_out << std::hex << (unsigned int) (c & (0xff)) << " ";

		if(i % 8 == 7){
			_out << "\t";
			_out << line_str.toLocal8Bit().constData() << std::endl;
			line_str.clear();
		}
	}

	if(!line_str.isEmpty()){
		for(int i=0; i<8-line_str.size(); i++){
			_out << "   ";
		}

		_out << "\t" << line_str.toLocal8Bit().constData() << std::endl;
	}

	return *this;
}
