
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


#include "Helper/FileHelper.h"

#include "SearchableFileTreeView.h"
#include "GUI/Helper/SearchableWidget/MiniSearcher.h"

#include "Helper/Settings/Settings.h"
#include "Helper/LibrarySearchMode.h"

#include <QDirIterator>
#include <algorithm>

AbstractSearchFileTreeModel::AbstractSearchFileTreeModel(QObject* parent) :
	QFileSystemModel(parent)
{
	_cur_idx = -1;
}


QModelIndex AbstractSearchFileTreeModel::getFirstRowIndexOf(QString substr)
{
	_cur_idx = -1;
	_found_strings.clear();

	Settings* settings = Settings::getInstance();
	LibraryHelper::SearchModeMask mask = settings->get(Set::Lib_SearchMode);
	substr = LibraryHelper::convert_search_string(substr, mask);

	QDirIterator it(this->rootPath(), QDirIterator::Subdirectories);
	QString str;

	while (it.hasNext()) {
		it.next();

		QString filename = it.fileName();
		filename = LibraryHelper::convert_search_string(filename, mask);

		if (filename.contains(substr)) {
			str = it.filePath();

			if(it.fileInfo().isFile()){
				QString parent_folder = Helper::File::get_parent_directory(str);
				str = parent_folder;
			}

			_found_strings << str;
		}
	}


	if(_found_strings.size() > 0){

		std::sort(_found_strings.begin(), _found_strings.end(), [](const QString& str1 , const QString& str2){
			return (str1 < str2);
		});

		_found_strings.removeDuplicates();

		str = _found_strings[0];
		_cur_idx = 0;
	}

	return index(str);
}



QModelIndex AbstractSearchFileTreeModel::getNextRowIndexOf(QString substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(substr)
	Q_UNUSED(cur_row)
	Q_UNUSED(parent)

	QString str;

	if(_cur_idx < 0 || _found_strings.isEmpty() ){
		return QModelIndex();
	}

	_cur_idx = (_cur_idx + 1) % _found_strings.size();

	str = _found_strings[_cur_idx];

	return index(str);
}


QModelIndex AbstractSearchFileTreeModel::getPrevRowIndexOf(QString substr, int cur_row, const QModelIndex& parent)
{

	Q_UNUSED(substr)
	Q_UNUSED(cur_row)
	Q_UNUSED(parent)

	QString str;

	if(_cur_idx < 0 ){
		return QModelIndex();
	}

	if(_cur_idx == 0){
		str = _found_strings[_cur_idx];

		return index(str);
	}

	_cur_idx--;
	str = _found_strings[_cur_idx];

	return index(str);
}

QMap<QChar, QString> AbstractSearchFileTreeModel::getExtraTriggers()
{
	return QMap<QChar, QString>();
}

