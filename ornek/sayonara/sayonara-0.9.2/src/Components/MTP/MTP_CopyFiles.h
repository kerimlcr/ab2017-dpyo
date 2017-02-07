
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




#ifndef MTPCOPYFILES_H
#define MTPCOPYFILES_H

#include <QThread>
#include "Helper/MetaData/MetaDataList.h"
#include "MTP_Folder.h"
#include "MTP_Track.h"

/**
 * @brief The MTP_CopyFiles class
 * @ingroup MTP
 */

class MTP_CopyFiles : public QThread
{
	Q_OBJECT

public:
	explicit MTP_CopyFiles(const MetaDataList& v_md, MTP_FolderPtr folder, QObject *parent = 0);
	explicit MTP_CopyFiles(const QStringList& files, MTP_FolderPtr folder, QObject *parent = 0);

private:

	MetaDataList		_v_md;
	MTP_FolderPtr		_folder;

protected:
	virtual void run() override;

signals:
	void sig_progress(int);

};

#endif // MTPCOPYFILES_H
