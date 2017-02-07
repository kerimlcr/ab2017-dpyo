/* CopyFolderThread.cpp */

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



#include "CopyThread.h"
#include "Helper/FileHelper.h"


CopyThread::CopyThread(const QString& target_dir, const ImportCache& cache, QObject *parent) :
	QThread(parent),
	SayonaraClass()
{
	_cache = cache;
	_target_dir = target_dir;
	clear();
}


void CopyThread::clear()
{
	_v_md.clear();
	_copied_files = 0;
	_lst_copied_files.clear();
	_mode = Mode::Copy;
	_percent = 0;
	_cancelled = false;
}


void CopyThread::emit_percent(int i, int n)
{
	int percent = (i * 100000) / n;
	_percent = percent / 1000;

	emit sig_progress(_percent);
}


void CopyThread::copy()
{
	clear();

	QStringList files = _cache.get_files();

	for(const QString& filename : files){

		if(_cancelled){
			return;
		}

		bool success;
		QString target_filename, target_dir;

		target_filename = _cache.get_target_filename(filename, _target_dir);
		if(target_filename.isEmpty()){
			continue;
		}

		target_dir = Helper::File::get_parent_directory(target_filename);

		success = Helper::File::create_directories(target_dir);
		if(!success){
			continue;
		}

		sp_log(Log::Debug) << "copy " << filename << " to \n\t" << target_filename;

		QFile f(filename);
		success = f.copy(target_filename);

		if(!success){
			continue;
		}

		MetaData md = _cache.get_metadata(filename);

		if(!md.filepath().isEmpty()){
			sp_log(Log::Debug) << "Set new filename: " << target_filename;
			md.set_filepath(target_filename);
			_v_md << md;
		}

		_lst_copied_files << target_filename;
		_copied_files++;

		emit_percent(_copied_files, files.size());
	}
}

void CopyThread::rollback()
{
	int n_operations = _lst_copied_files.size();
    int n_ops_todo = n_operations;
    int percent;

	QString lib_dir = _settings->get(Set::Lib_Path);
	QDir dir(lib_dir);

	for(const QString& f : _lst_copied_files) {
        QFile file(f);
        file.remove();
        percent = ((n_ops_todo--) * (_percent * 1000)) / (n_operations);

        emit sig_progress(percent/ 1000);
    }

    _percent = 0;
    _copied_files = 0;
    _lst_copied_files.clear();
}


void CopyThread::run()
{
    _cancelled = false;
	if(_mode == Mode::Copy){
		copy();
	}

	else if(_mode == Mode::Rollback){
		rollback();
	}
}


void CopyThread::cancel()
{
	_cancelled = true;
}

MetaDataList CopyThread::get_copied_metadata() const
{
	return _v_md;
}

bool CopyThread::was_cancelled() const
{
    return _cancelled;
}


int CopyThread::get_n_copied_files() const
{
    return _copied_files;
}


void CopyThread::set_mode(CopyThread::Mode mode)
{
    _mode = mode;
}

CopyThread::Mode CopyThread::get_mode() const
{
	return _mode;
}

