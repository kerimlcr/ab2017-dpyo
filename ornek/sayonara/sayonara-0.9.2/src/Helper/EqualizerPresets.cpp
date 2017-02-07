/* EqualizerPresets.cpp */

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



#include "EqualizerPresets.h"
#include "Helper/Logger/Logger.h"
#include <QStringList>

EQ_Setting::EQ_Setting(const QString& name){
	_name = name;

	for(int i=0; i<10; i++){
		_values.push_back(0);
	}
}


EQ_Setting::EQ_Setting(const EQ_Setting& s){
	_values = s.values();
	_name = s.name();
}

EQ_Setting::~EQ_Setting()
{

}


EQ_Setting EQ_Setting::fromString(const QString& str){

	EQ_Setting eq;
	QStringList list = str.split(':');
	if(list.size() < 11) {
		sp_log(Log::Debug) << "no valid eq string: " << str;
		return eq;
	}

	eq.set_name(list.first());
	list.pop_front();

	for(int i=0; i<list.size(); i++){
		if( i == eq.values().size() ){
			break;
		}

		eq.set_value(i, list[i].toInt());
	}

	return eq;
}


QString EQ_Setting::toString() const {

	QString str = _name;

	for(int i=0; i<_values.size(); i++){
		str += QString(":") + QString::number(_values[i]);
	}

	return str;
}


bool EQ_Setting::operator==(const EQ_Setting& s) const
{
	QString str = toString();
	QString other = s.toString();
	return ( str.compare(other, Qt::CaseInsensitive) == 0 );
}


QList<EQ_Setting> EQ_Setting::get_defaults(){

	QList<EQ_Setting> defaults;

	defaults << fromString(QString(":0:0:0:0:0:0:0:0:0:0"));
	defaults << fromString(QString("Flat:0:0:0:0:0:0:0:0:0:0"));
	defaults << fromString(QString("Rock:2:4:8:3:1:3:7:10:14:14"));
	defaults << fromString(QString("Light Rock:1:1:2:1:-2:-3:-1:3:5:8"));
	defaults << fromString(QString("Treble:0:0:-3:-5:-3:2:8:15:17:13"));
	defaults << fromString(QString("Bass:13:17:15:8:2:-3:-5:-3:0:0"));
	defaults << fromString(QString("Mid:0:0:5:9:15:15:12:7:2:0"));

	return defaults;
}

QList<int> EQ_Setting::get_default_values(const QString& name)
{
	QList<EQ_Setting> defaults = EQ_Setting::get_defaults();
	for(const EQ_Setting& def : defaults){
		if(def.name().compare(name, Qt::CaseInsensitive) == 0){
			return def.values();
		}
	}

	return QList<int>();
}


bool EQ_Setting::is_default_name(const QString& name)
{
	QList<EQ_Setting> defaults = EQ_Setting::get_defaults();
	for(const EQ_Setting& def : defaults){
		if(def.name().compare(name, Qt::CaseInsensitive) == 0){
			return true;
		}
	}

	return false;
}


QString EQ_Setting::name() const
{
	return _name;
}

void EQ_Setting::set_name(const QString& name)
{
	_name = name;
}



void EQ_Setting::set_value(int idx, int val)
{
	if(idx < 0 || idx >= _values.size()){
		return;
	}

	_values[idx] = val;
}

void EQ_Setting::set_values(const QList<int> values)
{
	_values = values;

	if(_values.size() != 10){
		sp_log(Log::Warning) << "EQ Preset " << _name << " should have 10 values. But it has " << _values.size();
	}

	while(_values.size() < 10){
		_values << 0;
	}

	while(_values.size() > 10)
	{
		_values.pop_back();
	}
}

void EQ_Setting::append_value(int val)
{
	if(_values.size() == 10){
		sp_log(Log::Warning) << "EQ Preset " << _name << " already has 10 values";
		return;
	}

	_values << val;
}



QList<int> EQ_Setting::values() const
{
	return _values;
}

int EQ_Setting::value(int idx) const
{
	if(idx < 0 || idx >= _values.size()){
		return 0;
	}

	return _values[idx];
}

bool EQ_Setting::is_default_name() const
{
	QList<EQ_Setting> defaults = EQ_Setting::get_defaults();
	for(const EQ_Setting& def : defaults){
		if(def.name().compare(_name, Qt::CaseInsensitive) == 0){
			return true;
		}
	}

	return false;

}

bool EQ_Setting::is_default() const
{
	QList<EQ_Setting> defaults = EQ_Setting::get_defaults();

	for(const EQ_Setting& def : defaults){
		if(def.name().compare(_name, Qt::CaseInsensitive) == 0){
			return( def == *this );
		}
	}

	return true;
}

