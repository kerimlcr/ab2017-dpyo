/* globals.h */

/* Copyright (C) 2012  Lucio Carreras
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

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <QObject>
#include <QString>
#include <QTime>
#include <QTranslator>
#include "Helper/Logger/Logger.h"

#include <type_traits>

#define SINGLETON_QOBJECT(class_name) protected: \
						class_name (QObject* object=0); \
						public: \
						static class_name *getInstance() { static class_name instance; return &instance; } \
						virtual ~class_name ();


#define SINGLETON(class_name) protected: \
						class_name (); \
						public: \
						static class_name *getInstance() { static class_name instance; return &instance; } \
						virtual ~class_name ();

#define DARK_BLUE(x) QString("<font color=#0000FF>") + x + QString("</font>")
#define LIGHT_BLUE(x) QString("<font color=#8888FF>") + x + QString("</font>")

#define CAR_RET QString("<br />")
#define BOLD(x) QString("<b>") + x + QString("</b>")
#define BLACK(x) QString("<font color=#000000>") + x + QString("</font>")


// name, target, dark, string
#define LINK(n, t, d, s) if(d) s=QString("<a href=\"t\">)") + LIGHT_BLUE(n) + QString("</a>"); \
						 else  s=QString("<a href=\"t\">)") + DARK_BLUE(n) + QString("</a>");

#define SAYONARA_ORANGE_STR QString("#e8841a")
#define SAYONARA_ORANGE_COL QColor(232, 132, 26)


#define PLUGIN_NUM 5
#define PLUGIN_NONE 0
#define PLUGIN_LFM_RADIO 1
#define PLUGIN_STREAM 2
#define PLUGIN_PODCASTS 3
#define PLUGIN_EQUALIZER 4
#define PLUGIN_PLAYLIST_CHOOSER 5
#define PLUGIN_LEVEL 6
/*

#define PLUGIN_NAME_LFM_RADIO tr("Last.&fm")
#define PLUGIN_NAME_STREAM tr("&Stream")
#define PLUGIN_NAME_PODCASTS tr("P&odcasts")
#define PLUGIN_NAME_EQUALIZER tr("&Equalizer")
#define PLUGIN_NAME_PLAYLIST_CHOOSER tr("&PlaylistChooser")
*/

#define GOOGLE_IMG_ICON QString("icon")
#define GOOGLE_IMG_SMALL QString("small")
#define GOOGLE_IMG_MEDIUM QString("medium")
#define GOOGLE_IMG_LARGE QString("large")
#define GOOGLE_IMG_XLARGE QString("xlarge")
#define GOOGLE_IMG_XXLARGE QString("xxlarge")
#define GOOGLE_IMG_HUGE QString("huge")

#define GOOGLE_FT_JPG QString("jpg")
#define GOOGLE_FT_PNG QString("png")
#define GOOGLE_FT_GIF QString("gif")
#define GOOGLE_FT_BMP QString("bmp")

typedef QPair<QString, QString> StringPair;
typedef QList<int> IntList;
typedef QList<int> IDList;
typedef QList<int> IdxList;
typedef QList<bool> BoolList;
typedef qint32 ArtistID;
typedef qint32 AlbumID;
typedef qint32 TrackID;

enum LameBitrate {

	LameBitrate_64=64,
	LameBitrate_128=128,
	LameBitrate_192=192,
	LameBitrate_256=256,
	LameBitrate_320=320,

	LameBitrate_var_0=0,
	LameBitrate_var_1=1,
	LameBitrate_var_2=2,
	LameBitrate_var_3=3,
	LameBitrate_var_4=4,
	LameBitrate_var_5=5,
	LameBitrate_var_6=6,
	LameBitrate_var_7=7,
	LameBitrate_var_8=8,
	LameBitrate_var_9=9
};

#define N_BINS 70

#define MEASURE_TIME(x) QTime myTimer; \
						myTimer.start(); \
						{ \
						x \
						} \
						qDebug() << "Time: " << myTimer.elapsed();

//#define between(idx, start, size) ( idx >= start && idx < size)


template<typename TINT, typename T>
typename std::enable_if<std::is_pointer<T>::value, bool>::type
between( TINT idx, const T& cont){
	return (idx >= 0 && idx < cont->size());
}

template<typename TINT, typename T>
typename std::enable_if<std::is_class<T>::value, bool>::type
between( TINT idx, const T cont){
	return (idx >= 0 && idx < cont.size());
}

template<typename TINT>
typename std::enable_if<std::is_integral<TINT>::value, bool>::type
between( TINT idx, TINT max){
	return (idx >= 0 && idx < max);
}



/*template<typename TINT>
between(const TINT num1, const TINT num2, const TINT num3){
	return (num1 >= num2 && num1 < num3);
}*/


#endif /* GLOBALS_H_ */
