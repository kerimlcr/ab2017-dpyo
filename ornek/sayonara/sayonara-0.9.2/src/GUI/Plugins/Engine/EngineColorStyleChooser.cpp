/* EngineColorStyleChooser.cpp */

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



#include "EngineColorStyleChooser.h"
#include "Database/DatabaseConnector.h"

#include <algorithm>

QList<float> borders_4, borders_3, borders_2;

EngineColorStyleChooser::EngineColorStyleChooser(int widget_width, int widget_height)
{

	borders_4 << 0 << 0.33f  << 0.66f << 1.0f;
	borders_3 << 0 << 0.50f  << 1.0f;
	borders_2 << 0 << 1.0f;


    reload(widget_width, widget_height);
}




void EngineColorStyleChooser::
create_colorstyle(ColorStyle &style, const ColorList &clist_active, int n_rects, int n_fading_steps) {

    style.style.clear();

		QHash<int, QColor> map_col_active;

        // compute color of each rect
        for(int i=0; i<n_rects; i++) {
            insertColorOfRect(i, n_rects, clist_active, map_col_active);
        }

        // compute active to inactive color
		QList<float> borders;
        borders << 0.0 << 1.0;

        // run through rect
        for(int idx_rect=0; idx_rect < n_rects; idx_rect++) {

            QColor col_active = map_col_active.value(idx_rect);

            // fadeout
			QHash<int, QColor> fading_map;

            ColorList col_list;
			col_list.colors << QColor(0,0,0,50) << col_active.darker();

            // run through step
			for(int idx_step=0; idx_step<=n_fading_steps; idx_step++){
				insertColorOfRect(idx_step, n_fading_steps + 1, col_list, fading_map);
			}

            fading_map[-1] = col_active;

            style.style << fading_map;
        }

        style.name = clist_active.name;
}


void EngineColorStyleChooser::insertColorOfRect(int bin, int n_bins, const ColorList& colorlist, QHash<int, QColor>& map) {

	QColor col;
	QList<float> borders;

	if(colorlist.colors.size() == 4) {
		borders = borders_4;
	}

	else if(colorlist.colors.size() == 3){
		borders = borders_3;
	}

	else if(colorlist.colors.size() == 2){
		borders = borders_2;
	}

    float x = (bin * 1.0f) / n_bins;
    int i=0;
    int r, g, b, a;

    if(bin == 0) {
        map[bin] = colorlist.colors[0];
        return;
    }

    while(x > borders[i]) {
        i++;
    }

    float dx = (borders[i] - borders[i-1]);

    float dy = colorlist.colors[i].red() - colorlist.colors[i-1].red();
    r = (int) (( dy * (x-borders[i-1]) ) / dx + colorlist.colors[i-1].red());

    dy = colorlist.colors[i].green() - colorlist.colors[i-1].green();
    g = (int) (( dy * (x-borders[i-1])) / dx + colorlist.colors[i-1].green());

    dy = colorlist.colors[i].blue() - colorlist.colors[i-1].blue();
    b = (int) ((dy * (x-borders[i-1])) / dx + colorlist.colors[i-1].blue());

    dy = colorlist.colors[i].alpha() - colorlist.colors[i-1].alpha();
    a = (int) ((dy * (x-borders[i-1])) / dx + colorlist.colors[i-1].alpha());

    col.setRed(r);
    col.setGreen(g);
    col.setBlue(b);
    col.setAlpha(a);

    map[bin] = col;
}


// scheme_fading_rect_color[r]: get access to the rect j in fading scheme i
// scheme_fading_rect_color[r][c]: get access to the c-th color of rect j in fading scheme i
ColorStyle EngineColorStyleChooser::get_color_scheme_spectrum(int i) {

	i = std::max(i, 0);
	i = std::min(_styles_spectrum.size() -1, i);

    return _styles_spectrum[i];
}

ColorStyle EngineColorStyleChooser::get_color_scheme_level(int i) {

	i = std::max(i, 0);
	i = std::min(_styles_level.size() -1, i);

    return _styles_level[i];
}



int EngineColorStyleChooser::get_num_color_schemes() {
    return _styles_spectrum.size();
}


void EngineColorStyleChooser::reload(int widget_width, int widget_height) {

	QList< RawColorStyle > colors_active = DatabaseConnector::getInstance()->get_raw_color_styles();

    _styles_spectrum.clear();
    _styles_level.clear();

    if(colors_active.size() == 0) {

        RawColorStyle fallback1, fallback2;

        fallback1.col_list.colors << QColor(0, 216, 0)  << QColor(216, 216, 0) << QColor(216, 0, 0) << QColor(216, 0, 0);;
        fallback1.col_list.name = "Fancy";
        fallback1.hor_spacing_level = 2;
        fallback1.hor_spacing_level = 2;
        fallback1.ver_spacing_spectrum = 1;
        fallback1.hor_spacing_spectrum = 1;
        fallback1.n_bins_spectrum = 50;
        fallback1.n_fading_steps_level = 20;
        fallback1.n_fading_steps_spectrum = 20;
        fallback1.rect_height_spectrum = 2;
        fallback1.rect_width_level = 5;
        fallback1.rect_height_level = 6;



        fallback2.col_list.colors << QColor(27, 32, 47)  << QColor(134, 134, 134) << QColor(216, 216, 216) << QColor(255, 255, 255);
        fallback2.col_list.name = "B/W";
        fallback2.hor_spacing_level = 2;
        fallback2.hor_spacing_level = 2;
        fallback2.ver_spacing_spectrum = 1;
        fallback2.hor_spacing_spectrum = 1;
        fallback2.n_bins_spectrum = 50;
        fallback2.n_fading_steps_level = 20;
        fallback2.n_fading_steps_spectrum = 20;
        fallback2.rect_height_spectrum = 2;
        fallback2.rect_width_level = 3;
        fallback2.rect_height_level = 3;

        colors_active << fallback1 << fallback2;

        DatabaseConnector::getInstance()->insert_raw_color_style_to_db(fallback1);
        DatabaseConnector::getInstance()->insert_raw_color_style_to_db(fallback2);
    }


	for(const RawColorStyle& rcs : colors_active) {
        ColorStyle style_spectrum;
        ColorStyle style_level;

        style_level.name = rcs.col_list.name;
        style_level.n_fading_steps = rcs.n_fading_steps_level;
        style_level.hor_spacing = rcs.hor_spacing_level;
        style_level.ver_spacing = rcs.ver_spacing_level;
        style_level.col_list =  rcs.col_list;
        style_level.rect_width = rcs.rect_width_level;
        style_level.rect_height = rcs.rect_height_level;
        style_level.n_rects = widget_width / (style_level.rect_width + style_level.hor_spacing);


        style_spectrum.name = rcs.col_list.name;
        style_spectrum.n_fading_steps = rcs.n_fading_steps_spectrum;
        style_spectrum.hor_spacing = rcs.hor_spacing_spectrum;
        style_spectrum.ver_spacing = rcs.ver_spacing_spectrum;
        style_spectrum.col_list = rcs.col_list;
        style_spectrum.rect_height = rcs.rect_height_spectrum;
        style_spectrum.n_rects = widget_height / (style_spectrum.rect_height + style_spectrum.ver_spacing);


        create_colorstyle(style_spectrum, rcs.col_list, style_spectrum.n_rects, rcs.n_fading_steps_spectrum);
        create_colorstyle(style_level, rcs.col_list, style_level.n_rects, rcs.n_fading_steps_level);

        _styles_spectrum << style_spectrum;
        _styles_level << style_level;
    }
}
