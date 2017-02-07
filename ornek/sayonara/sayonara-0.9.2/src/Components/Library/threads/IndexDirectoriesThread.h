#ifndef INDEXDIRECTORIESTHREAD_H
#define INDEXDIRECTORIESTHREAD_H


#include "Helper/MetaData/MetaDataList.h"
#include <QStringList>
#include <QThread>

class IndexDirectoriesThread : public QThread
{
public:
	IndexDirectoriesThread(const MetaDataList& v_md);
	QStringList get_directories() const;

protected:
	void run() override;

private:
	MetaDataList	_v_md;
	QStringList		_directories;
};

#endif // INDEXDIRECTORIESTHREAD_H
