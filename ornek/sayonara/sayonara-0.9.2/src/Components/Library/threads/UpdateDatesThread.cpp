#include "UpdateDatesThread.h"
#include "Database/DatabaseConnector.h"

UpdateDatesThread::UpdateDatesThread(QObject* parent) :
    QThread(parent)
{

}

UpdateDatesThread::~UpdateDatesThread() {}

void UpdateDatesThread::run()
{
    DatabaseConnector* db = DatabaseConnector::getInstance();
    if(db->updateTrackDates()){
	db->store_setting("version", 11);
    }
}
