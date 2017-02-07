#include "FileSystemWatcher.h"
#include "Helper/FileHelper.h"
#include "Helper/Helper.h"
#include "Helper/DirectoryReader/DirectoryReader.h"

#include <algorithm>

const int SleepingTimeSec = 15;

FileSystemWatcher::FileSystemWatcher(const QString& library_path, QObject* parent) :
	QThread(parent)
{
	_library_path = library_path;
}

void FileSystemWatcher::run()
{
	_may_run = true;
	_refresh = false;
	_waiting = false;

	_indexed_files = index_files(_library_path);

	while(_may_run)
	{
		bool changed = false;

		if(_refresh)
		{
			if(!_may_run){
				break;
			}

			_indexed_files = index_files(_library_path);
			_refresh = false;
			_waiting = false;
		}

		if(!_waiting)
		{
			if(!_may_run){
				break;
			}

			QStringList actual_files = index_files(_library_path);

			if(actual_files.size() != _indexed_files.size()){
				changed = true;
			}

			else
			{
				auto it1 = _indexed_files.begin();
				auto it2 = actual_files.begin();

				for(; it1 != _indexed_files.end(); it1++, it2++){
					if(!_may_run){
						break;
					}

					if(it1->compare(*it2) != 0){
						changed = true;
						break;
					}
				}
			}
		}

		if(changed && _may_run){
			_waiting = true;
			emit sig_changed();
		}

		for(int i=0; i<SleepingTimeSec; i++){
			if(!_may_run){
				break;
			}

			QThread::msleep(1000);
		}

		if(!_may_run){
			break;
		}
	}

	sp_log(Log::Info) << "Watcher finished";
}

QStringList FileSystemWatcher::index_files(const QString& root)
{
	DirectoryReader reader;
	QStringList filters;
	filters << Helper::get_soundfile_extensions();

	QStringList files;
	reader.get_files_in_dir_rec(QDir(root), files);

	if(!_may_run){
		return QStringList();
	}

	std::sort(files.begin(), files.end());

	return files;
}

void FileSystemWatcher::refresh()
{
	_refresh = true;
}

void FileSystemWatcher::stop()
{
	_may_run = false;
}
