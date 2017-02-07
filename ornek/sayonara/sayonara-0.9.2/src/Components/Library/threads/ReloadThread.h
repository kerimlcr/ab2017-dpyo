/* ReloadThread.h */

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
 * ReloadThread.h
 *
 *  Created on: Jun 19, 2011
 *      Author: Lucio Carreras
 */

#ifndef RELOADTHREAD_H_
#define RELOADTHREAD_H_


#include <QThread>
#include <QHash>
#include <QDir>

#include "Helper/Tagging/Tagging.h"
#include "Helper/Settings/SayonaraClass.h"
#include "Helper/MetaData/MetaDataList.h"
#include "LibraryNamespaces.h"


class DatabaseConnector;
class ReloadThread :
		public QThread,
		protected SayonaraClass
{

Q_OBJECT

signals:
	void sig_reloading_library(const QString& message, int progress);
    void sig_new_block_saved();


public:


	SINGLETON_QOBJECT(ReloadThread)

	void set_lib_path(const QString& library_path);

    void pause();
    void goon();
	bool is_running() const;

	void set_quality(Library::ReloadQuality quality);


protected:
	virtual void run() override;


private:
	DatabaseConnector*		_db=nullptr;
	QString					_library_path;
	MetaDataList			_v_md;
	Library::ReloadQuality	_quality;
	bool					_paused, _running;


private:

	int				get_and_save_all_files(const QHash<QString, MetaData>& v_md_map);
	QStringList		get_files_recursive (QDir base_dir);
	QStringList		process_sub_files(const QDir& dir, const QStringList& sub_files);

	bool			compare_md(const MetaData& md1, const MetaData& md2);

};

#endif /* RELOADTHREAD_H_ */
