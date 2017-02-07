/* EngineColorStyleChooser.h */

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



#ifndef ENGINECOLORSTYLECHOOSER_H
#define ENGINECOLORSTYLECHOOSER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QColor>
#include "Helper/Logger/Logger.h"
#include "StyleTypes.h"


class EngineColorStyleChooser : public QObject
{
    Q_OBJECT

signals:
    void sig_use_style(int, const ColorStyle& );

public:
    EngineColorStyleChooser(int widget_width, int widget_height);
    ColorStyle get_color_scheme_spectrum(int i);
    ColorStyle get_color_scheme_level(int i);
    int get_num_color_schemes();

    void reload(int widget_width, int widget_height);



private:
	void insertColorOfRect(int bin, int n_bins, const ColorList& colors, QHash<int, QColor>& map);
    void create_colorstyle(ColorStyle &style, const ColorList &colors_active, int n_rects, int n_fading_steps);
    void init();

	QList< ColorStyle > _styles_spectrum;
	QList< ColorStyle > _styles_level;
};

#endif // ENGINECOLORSTYLECHOOSER_H
