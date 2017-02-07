#ifndef FILESYSTEMWATCHER_H
#define FILESYSTEMWATCHER_H

#include <QThread>
#include <QStringList>
#include "Helper/Set.h"

class FileSystemWatcher : public QThread
{
	Q_OBJECT

signals:
	void sig_changed();

public:
	FileSystemWatcher(const QString& library_path, QObject* parent);
	void refresh();
	void stop();

protected:
	void run() override;

private:
	QStringList			_indexed_files;
	QString				_library_path;
	bool				_may_run;
	bool				_refresh;
	bool				_waiting;

	QStringList index_files(const QString& root);
};

#endif // FILESYSTEMWATCHER_H
