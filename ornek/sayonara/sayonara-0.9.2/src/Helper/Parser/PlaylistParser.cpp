/* PlaylistParser.cpp */

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

#include "M3UParser.h"
#include "PLSParser.h"
#include "ASXParser.h"

#include "Helper/Parser/PlaylistParser.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaDataList.h"

#include "Database/DatabaseConnector.h"

#include <QUrl>

int PlaylistParser::parse_playlist(const QString& local_filename, MetaDataList& v_md) {

	if(Helper::File::is_www(local_filename)){
		return 0;
	}

	MetaDataList v_md_tmp;
	MetaDataList v_md_to_delete;
	AbstractPlaylistParser* playlist_parser;

	if(local_filename.toLower().endsWith("m3u")) {
		playlist_parser = new M3UParser(local_filename);
	}

	else if(local_filename.toLower().endsWith("pls")) {
		playlist_parser = new PLSParser(local_filename);
	}

	else if(local_filename.toLower().endsWith("ram")) {
		playlist_parser = new M3UParser(local_filename);
	}

	else if(local_filename.toLower().endsWith("asx")) {
		playlist_parser = new ASXParser(local_filename);
	}

	else{
		playlist_parser = new M3UParser(local_filename);
		v_md_tmp = playlist_parser->get_md();

		if(v_md_tmp.isEmpty()){
			delete playlist_parser; playlist_parser = nullptr;
			playlist_parser = new PLSParser(local_filename);
			v_md_tmp = playlist_parser->get_md();
		}

		if(v_md_tmp.isEmpty()){
			delete playlist_parser; playlist_parser = nullptr;
			playlist_parser = new ASXParser(local_filename);
		}
	}

	v_md_tmp = playlist_parser->get_md();

	for(const MetaData& md : v_md_tmp) {

		if( Helper::File::check_file(md.filepath()) ){
			v_md << md;
		}

		else{
			v_md_to_delete << md;
		}
	}

	if(!v_md_to_delete.isEmpty()){
		DatabaseConnector::getInstance()->deleteTracks(v_md_to_delete);
	}

	if(playlist_parser){
		delete playlist_parser; playlist_parser = nullptr;
	}

	return v_md.size();
}


void PlaylistParser::save_playlist(QString filename, const MetaDataList& v_md, bool relative) {

	if(!filename.endsWith("m3u", Qt::CaseInsensitive)) {
		filename.append(".m3u");
	}

	bool success;
	QString dir_str = filename.left(filename.lastIndexOf(QDir::separator()));
	QDir dir(dir_str);
	dir.cd(dir_str);

	QFile file(filename);
	success = file.open(QIODevice::WriteOnly);
	if(!success){
		return;
	}

	file.write( QByteArray("#EXTM3U\n") );

	qint64 lines = 0;
	for(const MetaData& md : v_md) {

		QString str;
		if(relative) {
			str = dir.relativeFilePath(md.filepath());
		}

		else{
			str = md.filepath();
		}

		QString ext_data = "#EXTINF: " + QString::number(md.length_ms / 1000)  + ", " + md.artist + " - " + md.title + "\n";
		lines += file.write(ext_data.toLocal8Bit());
		lines += file.write(str.toLocal8Bit());
		lines += file.write( QByteArray("\n") );
	}

	file.close();
}
