/* LocalCoverSearcher.cpp */

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



#include "LocalCoverSearcher.h"
#include "Helper/FileHelper.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>

QStringList LocalCoverSearcher::get_local_cover_paths_from_filename(const QString& filepath){

	QString file, dir;
	Helper::File::split_filename(filepath, dir, file);
	return get_local_cover_paths_from_dirname(dir);

}

QStringList LocalCoverSearcher::get_local_cover_paths_from_dirname(const QString& filepath){

	QStringList ret;
	QStringList hash[3];
	QStringList entries;
	QStringList filters;
	filters << "*.jpg"
			<< "*.png";

	QDir dir(filepath);
	entries = dir.entryList(filters);
	if(entries.isEmpty()){
		return ret;
	}

	for(const QString& entry : entries){

		int prio = 2;

		if(entry.contains("cover", Qt::CaseInsensitive) ||
			entry.contains("albumart", Qt::CaseInsensitive) ||
			entry.contains("front", Qt::CaseInsensitive))
		{
			if(entry.contains("large", Qt::CaseInsensitive)){
				prio = 0;
			}

			else if(!entry.contains("small", Qt::CaseInsensitive)){
				prio = 1;
			}
		}

		hash[prio] << filepath + "/" + entry;
	}

	for(int i=0; i<3; i++){
		ret << hash[i];
	}

	return ret;
}


QStringList LocalCoverSearcher::get_local_cover_paths(const QString &filepath){


	QStringList ret;


	QFileInfo fi(filepath);

	if(fi.isDir()){
		ret =  get_local_cover_paths_from_dirname(filepath);
	}

	else if(fi.isFile()){
		ret =  get_local_cover_paths_from_filename(filepath);
	}


	for(QString& str : ret){
		QFileInfo sfi(str);
		str = sfi.absoluteFilePath();
	}

	return ret;
}
