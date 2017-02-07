
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




#include "MTP_CopyFiles.h"

#include "Helper/DirectoryReader/DirectoryReader.h"
#include <QFileInfo>

MTP_CopyFiles::MTP_CopyFiles(const MetaDataList& v_md, MTP_FolderPtr folder, QObject *parent) :
	QThread(parent),
	_v_md(v_md),
	_folder(folder)
{

}

MTP_CopyFiles::MTP_CopyFiles(const QStringList& paths, MTP_FolderPtr folder, QObject* parent) :
	QThread(parent),
	_folder(folder)
{

	DirectoryReader dr;
	_v_md = dr.get_md_from_filelist(paths);
}

void MTP_CopyFiles::run(){

	int i=0;

	for(const MetaData& md : _v_md){

		int progress;

		MTP_TrackPtr track = MTP_TrackPtr(new MTP_Track(md));

		_folder->send_track(track);

		progress = (i * 100) / _v_md.size();

		emit sig_progress( progress );
		i++;
	}

	emit sig_progress(-1);
}
