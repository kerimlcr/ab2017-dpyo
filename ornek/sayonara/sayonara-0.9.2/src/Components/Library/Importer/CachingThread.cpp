/* ImportCachingThread.cpp */

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



#include "CachingThread.h"

#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Helper/Tagging/Tagging.h"
#include "Helper/FileHelper.h"


CachingThread::CachingThread(const QStringList& file_list, QObject *parent) :
	QThread(parent),
	SayonaraClass()
{
	_file_list = file_list;
	_cancelled = false;
}

void CachingThread::run()
{

	_cache.clear();

	read_files();
	extract_soundfiles();

	emit sig_progress( -1 );
}


void CachingThread::read_files()
{
	DirectoryReader dr;
	dr.set_filter("*");

	for(const QString& filename : _file_list){

		if(_cancelled){
			_cache.clear();
			return;
		}

		if(Helper::File::is_dir(filename)){
			QStringList dir_files;
			QDir dir(filename);

			dr.get_files_in_dir_rec(dir, dir_files);

			for(const QString& dir_file : dir_files){
				_cache.add_standard_file(dir_file, filename);
			}
		}

		else{
			_cache.add_standard_file(filename);
		}
	}
}

void CachingThread::extract_soundfiles()
{

	for(const QString& filename : _cache.get_files()){

		if(Helper::File::is_soundfile(filename)){

			bool success = false;
			MetaData md;
			md.set_filepath(filename);
			success = Tagging::getMetaDataOfFile(md);

			if(success){
				_cache.add_soundfile(md);
			}
		}
	}
}

void CachingThread::change_metadata(const MetaDataList& v_md_old, const MetaDataList& v_md_new){
	_cache.change_metadata(v_md_old, v_md_new);	
}


ImportCache CachingThread::get_cache() const
{
	return _cache;
}

void CachingThread::cancel()
{
	_cancelled = true;
}

bool CachingThread::was_canelled() const
{
	return _cancelled;
}

