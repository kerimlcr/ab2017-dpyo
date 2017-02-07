/* SettingConverter.h */

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



#ifndef SETTINGCONVERTER_H
#define SETTINGCONVERTER_H

#include <QString>
#include <QSize>
#include <QPoint>
#include <QList>

// generic
template<typename T>
/**
 * @brief The SettingConverter class
 * @ingroup Settings
 */
class SettingConverter{
public:
	static QString cvt_to_string(const T& val){
		return val.toString();
	}

	static bool cvt_from_string(QString val, T& ret){
		ret = T::fromString(val);
		return true;
	}
};


// from bool
template<>
/**
 * @brief The SettingConverter<bool> class
 * @ingroup Settings
 */
class SettingConverter<bool>{
public:
	static QString cvt_to_string(const bool& val){
		if(val) {
			return QString("true");
		}

		else {
			return QString("false");
		}
	}

	static bool cvt_from_string(QString val, bool& b){
		if( val.compare("true", Qt::CaseInsensitive) == 0 ||
			val.toInt() > 0)
		{
			b = true;
		}

		else
		{
			b = false;
		}

		return true;
	}
};


// for int

/**
 * @brief The SettingConverter<int> class
 * @ingroup Settings
 */
template<>
class SettingConverter<int>{
public:
	static QString cvt_to_string(const int& val){
		return QString::number(val);
	}

	static bool cvt_from_string(QString val, int& i){
		bool ok;
		i = val.toInt(&ok);

		return ok;
	}
};

template<>
class SettingConverter<float>{
public:
	static QString cvt_to_string(const float& val){
		return QString::number(val);
	}

	static bool cvt_from_string(QString val, float& i){
		bool ok;
		i = val.toFloat(&ok);

		return ok;
	}
};


// for QStringList
template<>
/**
 * @brief The SettingConverter<QStringList> class
 * @ingroup Settings
 */
class SettingConverter<QStringList>{
public:
	static QString cvt_to_string(const QStringList& val){
		return val.join(",");
	}

	static bool cvt_from_string(QString val, QStringList& lst){
		lst = val.split(",");
		return true;
	}
};

// for QString
template<>
/**
 * @brief The SettingConverter<QString> class
 * @ingroup Settings
 */
class SettingConverter<QString>{
public:
	static QString cvt_to_string(const QString& val){
		return val;
	}

	static bool cvt_from_string(QString val, QString& b){
		b = val;
		return true;
	}
};

// for QSize
template<>
/**
 * @brief The SettingConverter<QSize> class
 * @ingroup Settings
 */
class SettingConverter<QSize>{
public:
	static QString cvt_to_string(const QSize& val){
		return QString::number(val.width()) + "," + QString::number(val.height());
	}

	static bool cvt_from_string(QString val, QSize& sz){

		bool ok;
		int width, height;

		QStringList lst = val.split(",");

		if(lst.size() < 2) return false;

		width = lst[0].toInt(&ok);

		if(!ok) return false;
		height = lst[1].toInt(&ok);
		if(!ok) return false;

		sz.setWidth(width);
		sz.setHeight(height);

		return true;
	}
};

// for QPoint
template<>
/**
 * @brief The SettingConverter<QPoint> class
 * @ingroup Settings
 */
class SettingConverter<QPoint>{
public:
	static QString cvt_to_string(const QPoint& val){
		return QString::number(val.x()) + "," + QString::number(val.y());
	}

	static bool cvt_from_string(QString val, QPoint& sz){

		bool ok;
		int x, y;

		QStringList lst = val.split(",");

		if(lst.size() < 2) return false;

		x = lst[0].toInt(&ok);

		if(!ok) return false;
		y = lst[1].toInt(&ok);
		if(!ok) return false;

		sz.setX(x);
		sz.setY(y);

		return true;
	}
};

// for QPoint
template<>
/**
 * @brief The SettingConverter<QByteArray> class
 * @ingroup Settings
 */
class SettingConverter<QByteArray>{
public:
	static QString cvt_to_string(const QByteArray& arr){
		QStringList numbers;
		for(quint8 item : arr){
			numbers << QString::number(item);
		}

		return numbers.join(",");
	}

	static bool cvt_from_string(QString str, QByteArray& arr){
		QStringList numbers = str.split(",");

		for(const QString& num_str : numbers){
			quint8 num = num_str.toInt();
			arr.append((char) num);
		}

		return (numbers.size() > 0);
	}
};

// generic for lists
template<typename T>
/**
 * @brief The SettingConverter<QList<T> > class
 * @ingroup Settings
 */
class SettingConverter< QList<T> >{

public:
	static QString cvt_to_string(const QList<T>& val){

		SettingConverter<T> sc;
		QStringList lst;

		for(const T& v : val){
			lst << sc.cvt_to_string(v);
		}

		return lst.join(",");
	}


	static bool cvt_from_string(const QString& val, QList<T>& ret){

		SettingConverter<T> sc;
		ret.clear();
		QStringList lst = val.split(",");

		for(const QString& l : lst){

			T v;
			sc.cvt_from_string(l, v);
			ret << v;
		}

		return true;
	}
};

template<typename A, typename B>
/**
 * @brief The SettingConverter<QPair<A, B> > class
 * @ingroup Settings
 */
class SettingConverter< QPair<A,B> >{
public:
	static QString cvt_to_string(const QPair<A,B>& val){
		A a = val.first;
		B b = val.second;
		SettingConverter<A> sc_a;
		SettingConverter<B> sc_b;
		return sc_a.cvt_to_string(val.first) + "," + sc_b.cvt_to_string(b);
	}

	static bool cvt_from_string(const QString& val, QPair<A,B>& ret){

		SettingConverter<A> sc_a;
		SettingConverter<B> sc_b;

		QStringList lst = val.split(",");
		QString a, b;
		bool success = true;
		if(lst.size() > 0){
			a = lst[0];
		}

		if(lst.size() > 1){
			b = lst[1];
		}
		else {
			success = false;
		}

		sc_a.cvt_from_string (a, ret.first);
		sc_b.cvt_from_string (b, ret.second);

		return success;
	}
};

#endif // SETTINGCONVERTER_H
