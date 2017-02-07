/* CopyThread.h */

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



#ifndef IMPORT_COPY_THREAD_H
#define IMPORT_COPY_THREAD_H

#include <QThread>
#include <QString>
#include <QMap>
#include <QString>
#include <QStringList>

#include "ImportCache.h"
#include "Helper/Settings/SayonaraClass.h"


class CopyThread :
		public QThread,
		protected SayonaraClass
{
    Q_OBJECT

public:

	enum class Mode : quint8 {
		Copy=0,
		Rollback
	};

	CopyThread(const QString& target_dir, const ImportCache& cache, QObject *parent=nullptr);

	int get_n_copied_files() const;

	bool was_cancelled() const;
	void cancel();

	MetaDataList get_copied_metadata() const;

	void set_mode(CopyThread::Mode mode);
	CopyThread::Mode  get_mode() const;


private:

	MetaDataList	_v_md;
	QString			_target_dir;
	QStringList		_lst_copied_files;
	int				_copied_files;
	Mode			_mode;
	int				_percent;
	bool			_cancelled;

	ImportCache		_cache;


private:

	void clear();
	void run();

	/**
	 * @brief Copies tracks to file system.
	 * Example: i want to import /home/user/dir\n
	 * my music library is in /home/user/Music\n
	 * i will type "chosen" into entry field\n
	 * i expect a directory /home/user/Music/chosen/dir in my music library
	 */
	void copy();
	void rollback();
    void emit_percent(int i, int n);


signals:
    void sig_progress(int);


};


#endif
