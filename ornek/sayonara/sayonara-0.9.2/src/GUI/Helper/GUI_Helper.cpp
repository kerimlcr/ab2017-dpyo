/* GUI_Helper.cpp */

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



/* GUI_Helper.cpp */

#include "GUI_Helper.h"
#include "Helper/Logger/Logger.h"


QIcon GUI::get_icon(const QString& icon_name){

	QString path;

	if(icon_name.endsWith(".png")){
		path = icon_name;
		// alles paletti
	}

	else if(!icon_name.endsWith(".svg.png")){

		path = icon_name + ".svg.png";
	}

	path.prepend(":/Icons/");


	QIcon icon = QIcon(path);
	if(icon.isNull()){
		sp_log(Log::Warning) << "Icon " << path << " does not exist";
	}

	return icon;
}

QPixmap GUI::get_pixmap(const QString& icon_name, QSize sz, bool keep_aspect){

	QString path = QString(":/Icons/") + icon_name;
	if(path.endsWith(".png")){
		// alles paletti
	}

	else if(!path.endsWith(".svg.png")){
		path += ".svg.png";
	}

	QPixmap pixmap(path);

	if(pixmap.isNull()){
		sp_log(Log::Warning) << "Pixmap " << path << " does not exist";
	}

	if(sz.width() == 0){
		return pixmap;
	}

	else{
		if(keep_aspect){
			return pixmap.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}

		else{
			return pixmap.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		}
	}

	return pixmap;
}


static QMainWindow* main_window=nullptr;

QMainWindow* GUI::get_main_window(){
	return main_window;
}

void GUI::set_main_window(QMainWindow* window){
	main_window = window;
}
