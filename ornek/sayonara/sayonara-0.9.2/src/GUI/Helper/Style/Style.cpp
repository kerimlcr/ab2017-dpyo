/* Style.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * Style.cpp
 *
 *  Created on: Oct 27, 2011
 *      Author: Lucio Carreras
 */

#include "Style.h"

#include "Helper/Helper.h"
#include "Helper/Settings/Settings.h"
#include "Helper/FileHelper.h"

#include <QApplication>
#include <QFont>
#include <QStyle>

#define NEWLINE "\n";

QString Style::get_style(bool dark) {

	Settings* settings = Settings::getInstance();

	QFont std_font = QApplication::font();
	QString font_family =	settings->get(Set::Player_FontName);
	int font_size =			settings->get(Set::Player_FontSize);
	int font_size_lib =		settings->get(Set::Lib_FontSize);
	int font_size_pl =		settings->get(Set::PL_FontSize);
	bool lib_bold =			settings->get(Set::Lib_FontBold);

	if(font_family.isEmpty()){
		font_family = std_font.family();
	}

	if(font_size <= 0){
		font_size = std_font.pointSize();
	}

	if(font_size_lib <= 0){
		font_size_lib = font_size;
	}

	if(font_size_pl <= 0){
		font_size_pl = font_size;
	}

    QString style;
    QString share_path = Helper::get_share_path();

	if(!dark){
		Helper::File::read_file_into_str(share_path + "standard.css", style);
	}

    else{
		Helper::File::read_file_into_str(share_path + "dark.css", style);
        style.replace("<<SHARE_PATH>>", share_path);
    }

	style.replace("<<FONT_FAMILY>>", font_family);
	style.replace("<<FONT_SIZE>>", QString::number(font_size));
	style.replace("<<FONT_SIZE_LIB>>", QString::number(font_size_lib));
	style.replace("<<FONT_SIZE_PL>>", QString::number(font_size_pl));
	style.replace("<<FONT_WEIGHT_LIB>>", lib_bold ? "600" : "normal");

    return style;
}

