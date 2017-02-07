/* StyleTypes.h */

/* Copyright (C) 2013  Lucio Carreras
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



#ifndef STYLETYPES_H
#define STYLETYPES_H

#include <QColor>
#include <QVector>
#include <QString>
#include <QHash>

struct ColorList {
    QString name;
	QList<QColor> colors;
};

struct ColorStyle {

    QString name;

    // list size is number or rectangles
    // int is the step index
    QVector< QHash<int, QColor> > style;
    ColorList col_list;

    int rect_height;
    int rect_width;

    int n_rects;
    int n_fading_steps;
    int hor_spacing;
    int ver_spacing;
};

struct RawColorStyle{

    ColorList col_list;

    int n_bins_spectrum;

    int rect_height_spectrum;
    int rect_width_level;
    int rect_height_level;
    int n_rects_level;

    int n_fading_steps_spectrum;
    int n_fading_steps_level;

    int hor_spacing_level;
    int ver_spacing_level;

    int hor_spacing_spectrum;
    int ver_spacing_spectrum;

    RawColorStyle(){
        col_list.name = "";

		n_rects_level = 0;
        n_bins_spectrum = 20;
        rect_height_spectrum = 2;
        rect_width_level = 3;
        rect_height_level = 6;
        n_fading_steps_level = 20;
        n_fading_steps_spectrum = 20;
        hor_spacing_level = 2;
        hor_spacing_spectrum = 2;
        ver_spacing_level = 2;
        ver_spacing_spectrum = 1;
    }

    QString toString() const {

        QString ret;
        ret += col_list.name + "";
        ret += ", n_bins_sp: " + QString::number(n_bins_spectrum);
        ret += ", rect_h_sp: " + QString::number(rect_height_spectrum);
        ret += ", fad_s_sp: " +  QString::number(n_fading_steps_spectrum);
        ret += ", rect_w_lv: " + QString::number(rect_width_level);
        ret += ", rect_h_lv: " + QString::number(rect_height_level);
        ret += ", fad_s_lv: " + QString::number(n_fading_steps_level);
        ret += ", hor_s_lv: " + QString::number(hor_spacing_level);
        ret += ", ver_s_lv: " + QString::number(ver_spacing_level);
        ret += ", hor_s_sp: " + QString::number(hor_spacing_spectrum);
        ret += ", ver_s_sp: " + QString::number(ver_spacing_spectrum);

        return ret;
    }

};


#endif // STYLETYPES_H
