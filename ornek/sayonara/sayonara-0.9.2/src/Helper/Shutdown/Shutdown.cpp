/* Shutdown.cpp */

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


/* Inspired by qshutdown
 * Original-Maintainer: Christian Metscher <hakaishi@web.de>
 * Homepage: https://launchpad.net/~hakaishi
 */


#include "Helper/Helper.h"
#include "Helper/Shutdown/Shutdown.h"
#include "Database/DatabaseConnector.h"
#include "Components/PlayManager/PlayManager.h"
#include "Interfaces/Notification/NotificationHandler.h"
#include "Helper/Logger/Logger.h"


#ifdef WITH_SHUTDOWN

Shutdown::Shutdown(QObject* parent) :
	QObject(parent)
{
	_play_manager = PlayManager::getInstance();
	_is_running = false;

	_timer = new QTimer(this);
	_timer_countdown = new QTimer(this);

	_timer->setInterval(100);
	_timer_countdown->setInterval(50);

	_msecs2go = 0;

	connect(_timer, &QTimer::timeout, this, &Shutdown::timeout);
	connect(_timer_countdown, &QTimer::timeout, this, &Shutdown::countdown_timeout);
	connect(_play_manager, &PlayManager::sig_playlist_finished, this, &Shutdown::playlist_finished);
}


Shutdown::~Shutdown(){
	_timer->stop();
	_timer->deleteLater();
	_timer_countdown->stop();
	_timer_countdown->deleteLater();
}


void Shutdown::shutdown_after_end(){
	_is_running = true;

	NotificationHandler::getInstance()->notify(tr("Shutdown"),
											   tr("Computer will shutdown after playlist has finished"),
											   Helper::get_share_path() + "logo.png");
}


bool Shutdown::is_running() const
{
	return _is_running;
}


void Shutdown::shutdown(quint64 ms){

	if(ms == 0){
		timeout();
		return;
	}

	_is_running = true;
	_msecs2go = ms;
	_timer->start((int) ms);
	_timer_countdown->start(1000);
	emit sig_started(ms);

	NotificationHandler::getInstance()->notify(tr("Shutdown"),
											   tr("Computer will shutdown in %1 minutes").arg(Helper::cvt_ms_to_string(ms, false, true, false)),
											   Helper::get_share_path() + "logo.png");
}


void Shutdown::stop(){
	sp_log(Log::Info) << "Shutdown cancelled";
	_is_running = false;
	_timer->stop();
	_timer_countdown->stop();
	_msecs2go = 0;

}


void Shutdown::countdown_timeout(){

	_msecs2go -= 1000;
	_timer_countdown->start(1000);

	emit sig_time_to_go(_msecs2go);
	sp_log(Log::Info) << "Time to go: " << _msecs2go;

	if(_msecs2go % 60000 == 0){
		NotificationHandler::getInstance()->notify(tr("Shutdown"),
												   tr("Computer will shutdown in %1 minutes").arg(Helper::cvt_ms_to_string(_msecs2go, false, true, false)),
												   Helper::get_share_path() + "logo.png");
	}
}


void Shutdown::timeout()
{
	
	_is_running = false;
	DatabaseConnector::getInstance()->store_settings();


#ifdef Q_OS_WIN
	//ExitWindowsEx(	

#else

	QDBusMessage response;

	QDBusInterface free_desktop_login(
				"org.freedesktop.login1",
				"/org/freedesktop/login1",
				"org.freedesktop.login1.Manager",
				 QDBusConnection::systemBus()
	);

	QDBusInterface free_desktop_console_kit(
				"org.freedesktop.ConsoleKit",
				"/org/freedesktop/ConsoleKit/Manager",
				"org.freedesktop.ConsoleKit.Manager",
				QDBusConnection::systemBus()
	);

	QDBusInterface gnome_session_manager(
				"org.gnome.SessionManager",
				"/org/gnome/SessionManager",
				"org.gnome.SessionManager",
				QDBusConnection::sessionBus()
	);

	QDBusInterface mate_session_manager(
				"org.mate.SessionManager",
				"/org/mate/SessionManager",
				"org.mate.SessionManager",
				QDBusConnection::sessionBus()
	);


	QDBusInterface kde_session_manager(
				"org.kde.ksmserver",
				"/KSMServer",
				"org.kde.KSMServerInterface",
				QDBusConnection::sessionBus()
	);


	if(QProcess::startDetached("/usr/bin/systemctl poweroff")){
		return;
	}

	bool g_pwr1 = QProcess::startDetached("gnome-power-cmd.sh shutdown");
	bool g_pwr2 = QProcess::startDetached("gnome-power-cmd shutdown");

	if(g_pwr1 || g_pwr2){
		return;
	}

	response = free_desktop_login.call("PowerOff", true);

	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = gnome_session_manager.call("RequestShutdown");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = gnome_session_manager.call("Shutdown");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = kde_session_manager.call("logout", 0, 2, 2);
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = kde_session_manager.call("Shutdown");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = mate_session_manager.call("RequestShutdown");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = mate_session_manager.call("Shutdown");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	response = free_desktop_console_kit.call("Stop");
	if(response.type() != QDBusMessage::ErrorMessage){
		return;
	}

	if(QProcess::startDetached("sudo shutdown -P now")){
		return;
	}

	if(QProcess::startDetached("sudo shutdown -h -P now")){
		return;
	}

	sp_log(Log::Warning) << "Sorry, power off is not possible";

#endif
}


void Shutdown::playlist_finished(){
	if( _is_running ){
		timeout();
	}
}

#endif
