/* LibraryImporter.h */

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



#ifndef LIBRARYIMPORTER_H
#define LIBRARYIMPORTER_H


#include "Helper/Tagging/Tagging.h"
#include "Helper/Settings/SayonaraClass.h"
#include "ImportCache.h"

class DatabaseConnector;
class CachingThread;
class CopyThread;

class LibraryImporter : public QObject, protected SayonaraClass
{
	Q_OBJECT

	SINGLETON_QOBJECT(LibraryImporter)


public:

	enum class ImportStatus : quint8 {
			 Cancelled,
			 Rollback,
			 Caching,
			 NoTracks,
			 Sleeping,
			 Importing,
			 Imported
	};

signals:
	void sig_got_metadata(const MetaDataList&);
	void sig_status_changed(LibraryImporter::ImportStatus);
	void sig_got_library_dirs(const QStringList& library_dirs);
	void sig_progress(int);

	void sig_imported();
	void sig_triggered();


public:
	void import_files(const QStringList& files);
	void import_dir(const QString& dirname);
	void accept_import(const QString& target_dir);
	void cancel_import();
	LibraryImporter::ImportStatus get_status() const;


private slots:

	void caching_thread_finished();
	void copy_thread_finished();

	void emit_status(LibraryImporter::ImportStatus status);

	void metadata_changed(const MetaDataList& old_md, const MetaDataList& new_md);

private:
	CachingThread*			_cache_thread=nullptr;
	CopyThread*				_copy_thread=nullptr;

	ImportCache				_import_cache;

	DatabaseConnector*		_db=nullptr;

	ImportStatus			_status;

	QString                 _lib_path;
	QString					_src_dir;
};

#endif // LIBRARYIMPORTER_H
