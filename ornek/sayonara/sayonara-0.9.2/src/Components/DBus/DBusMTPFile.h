/* DBusMTPFile.h */

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



#ifndef DBUS_MTP_FILE_H_
#define DBUS_MTP_FILE_H_

#include <QList>
#include <QDBusMetaType>
#include <QMetaType>
#include <QtDBus>


/*namespace DBusMTP {
	class File : public QObject {
		Q_OBJECT

	public:
		File(QObject* parent=nullptr){}
		virtual ~File(){}

		int getFileFromFileDescriptor(int deviceID, const QDBusUnixFileDescriptor &fileDescriptorSource, int transactionID, int &newFileID){return 0;}
		int getFileMetadata(int deviceID, int fileID, int &parentID, int &storageID, QString &name, qlonglong &size, qlonglong &modificationdate, QString &mimetype){return 0;}
		int getFileToFileDescriptor(int deviceID, int fileID, const QDBusUnixFileDescriptor &fileDescriptorTarget, int transactionID){return 0;}
		int getFilesAndFolders(int deviceID, int fileID, MTPFileList &listFileStructs){return 0;}
		int getThumbnail(int deviceID, int fileID, BoolList &thumbnail){return 0;}
		int setFileName(int deviceID, int fileID, const QString& new_name){return 0;}


	};
}*/

#endif
