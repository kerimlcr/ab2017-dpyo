#include "IndexDirectoriesThread.h"
#include "Helper/Set.h"
#include "Helper/FileHelper.h"

IndexDirectoriesThread::IndexDirectoriesThread(const MetaDataList& v_md)
{
	_v_md = v_md;
}


QStringList IndexDirectoriesThread::get_directories() const
{
	return _directories;
}

void IndexDirectoriesThread::run()
{
	_directories.clear();

	SP::Set<QString> paths;
	for(const MetaData& md : _v_md)
	{
		paths.insert(Helper::File::get_parent_directory(md.filepath()));
	}

	for(auto it=paths.begin(); it!=paths.end(); it++){
		_directories << *it;
	}
}

