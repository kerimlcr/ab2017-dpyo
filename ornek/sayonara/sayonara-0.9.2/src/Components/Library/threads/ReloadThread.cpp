/* ReloadThread.cpp */

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
 * ReloadThread.cpp
 *
 *  Created on: Jun 19, 2011
 *      Author: Lucio Carreras
 */

#define N_FILES_TO_STORE 500

#include "ReloadThread.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Database/DatabaseConnector.h"
#include "Helper/DirectoryReader/DirectoryReader.h"

#include <utility>

ReloadThread::ReloadThread(QObject *parent) :
	QThread(parent),
	SayonaraClass()
{

	_db = DatabaseConnector::getInstance();

	_paused = false;
	_running = false;

	_library_path = _settings->get(Set::Lib_Path);
	_quality = Library::ReloadQuality::Fast;
}


ReloadThread::~ReloadThread() {

}

bool ReloadThread::compare_md(const MetaData& md1, const MetaData& md2){

	QStringList genres1 = md1.genres;
	QStringList genres2 = md2.genres;

	genres1.removeAll("");
	genres1.removeDuplicates();

	genres2.removeAll("");
	genres2.removeDuplicates();

	return (md1.title == md2.title &&
			md1.album == md2.album &&
			md1.artist == md2.artist &&
			md1.year == md2.year &&
			md1.rating == md2.rating &&
			genres1 == genres2 &&
			md1.discnumber == md2.discnumber &&
			md1.track_num == md2.track_num  
			);
}

int ReloadThread::get_and_save_all_files(const QHash<QString, MetaData>& md_map_lib) {

	if(_library_path.size() == 0 || !QFile::exists(_library_path)) {
		return 0;
	}

	QDir dir(_library_path);

	MetaDataList v_md_to_store;
	QStringList files = get_files_recursive (dir);

	int n_files = files.size();
	int cur_idx_files=0;

	for(const QString& filepath : files){

		bool file_was_read = false;
		MetaData md(filepath);
		const MetaData& md_lib = md_map_lib[filepath];

		int percent = (cur_idx_files++ * 100) / n_files;
		emit sig_reloading_library(tr("Reloading library"), percent);

		if(md_lib.id >= 0){

			if(_quality == Library::ReloadQuality::Fast){
				continue;
			}

			file_was_read = Tagging::getMetaDataOfFile(md, Tagging::Quality::Dirty);

			if(!file_was_read){		
				continue;
			}

			if( md_lib.length_ms > 1000 && md_lib.length_ms < 3600000 && compare_md(md, md_lib)){
				continue;
			}
		}

		sp_log(Log::Debug) << "Have to reload " << filepath;

		file_was_read = Tagging::getMetaDataOfFile(md, Tagging::Quality::Quality);

		if(file_was_read){
			v_md_to_store << md;

			if(v_md_to_store.size() >= N_FILES_TO_STORE){
				_db->storeMetadata(v_md_to_store);
				v_md_to_store.clear();
			}
		}
	}

	if(!v_md_to_store.isEmpty()){
		_db->storeMetadata(v_md_to_store);
		v_md_to_store.clear();
	}

	_db->createIndexes();
	_db->clean_up();

	return v_md_to_store.size();
}


QStringList ReloadThread::get_files_recursive(QDir base_dir) {

	QStringList ret;
	QString message = tr("Reading files from file system") + "... ";
	emit sig_reloading_library(message, 0);

	QStringList soundfile_exts = Helper::get_soundfile_extensions();
	QStringList sub_dirs;
	QStringList sub_files;

	sub_dirs = base_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for(const QString& dir : sub_dirs) {

		bool success = base_dir.cd(dir);

		if(!success){
			continue;
		}

		ret << get_files_recursive(base_dir);

		base_dir.cdUp();
    }

	sub_files = base_dir.entryList(soundfile_exts, QDir::Files);
	if(sub_files.isEmpty()){
		return ret;
	}


	ret << process_sub_files(base_dir, sub_files);

	return ret;
}



QStringList ReloadThread::process_sub_files(const QDir& base_dir, const QStringList& sub_files){

	QStringList lst;
	for(const QString& filename : sub_files) {

		QString abs_path = base_dir.absoluteFilePath(filename);
		QFileInfo info(abs_path);

		if(!info.exists()){
			sp_log(Log::Warning) << "File " << abs_path << " does not exist. Skipping...";
			continue;
		}

		if(!info.isFile()){
			sp_log(Log::Warning) << "Error: File " << abs_path << " is not a file. Skipping...";
			continue;
		}

		lst << abs_path;
	}

	return lst;
}



void ReloadThread::pause() {
    _paused = true;
}

void ReloadThread::goon() {
	_paused = false;
}

bool ReloadThread::is_running() const
{
	return _running;
}

void ReloadThread::set_quality(Library::ReloadQuality quality)
{
	_quality = quality;
}

void ReloadThread::run() {

	if(_running){
		return;
	}

	_running = true;
    _paused = false;

	MetaDataList v_md, v_to_delete;
	QHash<QString, MetaData> v_md_map;

	emit sig_reloading_library(tr("Delete orphaned tracks..."), 0);

	_db->deleteInvalidTracks();
	_db->getTracksFromDatabase(v_md);

	sp_log(Log::Debug) << "Have " << v_md.size() << " tracks";

	// find orphaned tracks in library && delete them
	for(const MetaData& md : v_md){

		if(!Helper::File::check_file(md.filepath())) {
			v_to_delete << std::move(md);
		}

		else{
			v_md_map[md.filepath()] = md;
		}
	}

	if(!v_to_delete.isEmpty()){
		_db->deleteTracks(v_to_delete);
	}

	get_and_save_all_files(v_md_map);

    _paused = false;
	_running = false;

}


void ReloadThread::set_lib_path(const QString& library_path) {
	_library_path = library_path;
}



