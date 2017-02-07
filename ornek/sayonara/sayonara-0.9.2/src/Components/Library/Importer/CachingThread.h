/* ImportFolderThread.h */

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



#ifndef IMPORTFOLDERTHREAD_H
#define IMPORTFOLDERTHREAD_H


#include "ImportCache.h"
#include "Helper/Settings/SayonaraClass.h"

#include <QThread>


class CachingThread :
		public QThread,
		private SayonaraClass
{
	Q_OBJECT

private:

signals:
	void			sig_progress(int);


public:
	CachingThread(const QStringList& file_list, QObject *parent=nullptr);


	void			cancel();
	bool			was_canelled() const;
	ImportCache		get_cache() const;
	void			change_metadata(const MetaDataList& v_md_old, const MetaDataList& v_md_new);


private:

	QStringList		_file_list;
	bool			_cancelled;

	ImportCache		_cache;


private:
	void run() override;

	void read_files();
	void extract_soundfiles();

};

#endif // IMPORTFOLDERTHREAD_H
