#ifndef UPDATEDATESTHREAD_H
#define UPDATEDATESTHREAD_H

#include <QThread>

class UpdateDatesThread : public QThread
{
public:
	UpdateDatesThread(QObject* parent=nullptr);
	~UpdateDatesThread();

protected:
	void run() override;
};

#endif // UPDATEDATESTHREAD_H
