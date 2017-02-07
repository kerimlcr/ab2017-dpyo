/* IconLoader.cpp */

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



#include "IconLoader.h"
#include "Helper/Settings/Settings.h"
#include "Helper/DirectoryReader/DirectoryReader.h"

#include "GUI/Helper/GUI_Helper.h"

#include <QObject>
#include <QFile>
#include <QRegExp>

#ifdef Q_OS_WIN
	QString get_win_icon_name(const QString& name)
	{
		QString icon_name = QString(":/IconsWindows/") + name + ".png";
		return icon_name;
	}
#endif

IconLoader::IconLoader(){

	_theme = QIcon::themeName();
	_theme_paths = QIcon::themeSearchPaths();
	_settings = Settings::getInstance();

#ifndef Q_OS_WIN
	for(const QString& theme_path : _theme_paths){

		QString full_theme_path = theme_path + "/" + _theme;
		QString index_path = full_theme_path + "/theme.index";
		if(!QFile::exists(index_path)){
			continue;
		}

		_theme_paths += load_ancestors(index_path);
	}
#endif

	sp_log(Log::Debug) << "Theme paths " << _theme_paths;

}

IconLoader::~IconLoader(){


}

QStringList IconLoader::load_ancestors(const QString &index_theme_file){

	QFile f(index_theme_file);
	if(!f.open(QFile::ReadOnly)){
		return QStringList();
	}

	QRegExp re_inherit(".*Inhertits\\d=\\d(.*)");

	QStringList ancestor_list;

	while(f.canReadLine()){
		QByteArray arr = f.readLine();
		QString line = QString::fromLocal8Bit(arr);
		QString ancestors;

		int idx = re_inherit.indexIn(line);
		if(idx < 0){
			continue;
		}

		ancestors = re_inherit.cap(1);
		ancestor_list = ancestors.split(',');
		break;
	}

	f.close();

	for(auto it=ancestor_list.begin(); it != ancestor_list.end(); it++){
		*it = it->trimmed();
	}

	return ancestor_list;
}


void IconLoader::add_icon_names(const QStringList& icon_names){

	#ifdef Q_OS_WIN
		return;
	#endif

	DirectoryReader dir_reader;
	dir_reader.set_filter("*.png");

	for(const QString& icon_name : icon_names){

		QIcon icon = QIcon::fromTheme(icon_name);
		if( !icon.isNull() ){
			_icons[icon_name] = icon;
			//sp_log(Log::Debug) << "Could load icon from theme: " << icon_name;
			continue;
		}

		for(const QString& theme_path : _theme_paths){

			bool found = false;
			QString full_theme_path = theme_path + "/" + _theme;


			QDir full_theme_dir(full_theme_path);
			//sp_log(Log::Debug) << full_theme_dir.canonicalPath();


			QStringList files = dir_reader.find_files_rec(full_theme_dir, icon_name);
			//sp_log(Log::Debug) << "Search for " << icon_name << " in " << full_theme_path << ": " << files;

			for(const QString& file : files){

				if(file.contains("48")){
					found = true;
				}

				else if(file.contains("32")){
					_icons[icon_name] = QIcon(file);
					found = true;
				}

				else if(file.contains("24")){
					found = true;
				}

				if(found){
					//sp_log(Log::Debug) << "Found icon " << icon_name << " in " << file;
					_icons[icon_name] = QIcon(file);
					break;
				}
			}

			if(found){
				break;
			}
		}
	}
}

QIcon IconLoader::get_icon(const QString& name, const QString& dark_name)
{
	bool dark = (_settings->get(Set::Player_Style) == 1);

	if(!dark){

		if(!has_std_icon(name)){
			QStringList lst; lst << name;
			add_icon_names(lst);

			if(has_std_icon(name)){

#ifdef Q_OS_WIN
				return QIcon(get_win_icon_name(name));
#else
				return _icons[name];
#endif
			}
		}

		else{

#ifdef Q_OS_WIN
			return QIcon(get_win_icon_name(name));
#else
			return _icons[name];
#endif
		}
	}

	return GUI::get_icon(dark_name);
}

QIcon IconLoader::get_icon(const QStringList& names, const QString& dark_name){

	bool dark = (_settings->get(Set::Player_Style) == 1);

	if(!dark){
		for(const QString& name : names){
			sp_log(Log::Debug) << "Has icon " << name << " for " << QIcon::themeName() << "? " << (!QIcon::fromTheme(name).isNull());
			if(!has_std_icon(name)){
				QStringList lst; lst << name;
				add_icon_names(lst);

				if(has_std_icon(name)){

	#ifdef Q_OS_WIN
					return QIcon(get_win_icon_name(name));
	#else
					return _icons[name];
	#endif
				}
			}

			else{

	#ifdef Q_OS_WIN
				return QIcon(get_win_icon_name(name));
	#else
				return _icons[name];
	#endif
			}
		}
	}

	return GUI::get_icon(dark_name);
}


bool IconLoader::has_std_icon(const QString& name) const
{
#ifdef Q_OS_WIN
	QIcon icon = QIcon( get_win_icon_name(name) );
	if(icon.isNull())
	{
		return false;
	}

	return true;

#endif

	return _icons.contains(name);
}
