/* Main.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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

/*
 * Main.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Lucio Carreras
 */

#include "Application/application.h"
#include "Helper/Helper.h"
#include "Helper/Parser/CommandLineParser.h"
#include "Helper/Settings/SettingRegistry.h"

#include "GUI/Helper/GUI_Helper.h"

#include <QSharedMemory>
#include <QTranslator>
#include <QFontDatabase>
#include <QtGlobal>
#include <algorithm>

#ifdef Q_OS_LINUX
	#include <execinfo.h>		// backtrace
	#include <csignal>			// kill/signal
	#include <sys/types.h>		// kill
	#include <cstring>			// memcpy
	#include <unistd.h>			// STDERR_FILENO
#else

	#include <glib-2.0/glib.h>
	#undef signals
	#include <gio/gio.h>
	
#endif

int check_for_another_instance(qint64 own_pid) {

#ifdef Q_OS_LINUX

	QDir dir("/proc");
	dir.cd(".");
	QStringList lst = dir.entryList(QDir::Dirs);

	for(const QString& dirname : lst) {
		bool ok;
		int tmp_pid = dirname.toInt(&ok);
		if(!ok) continue;

		dir.cd(dirname);

		QFile f(dir.absolutePath() + QDir::separator() + "cmdline");
		f.open(QIODevice::ReadOnly);
		if(!f.isOpen()) {
			dir.cd("..");
			continue;
		}

		QString str = f.readLine();
		f.close();

		if(str.contains("sayonara", Qt::CaseInsensitive)) {
			if(own_pid != tmp_pid){
				return tmp_pid;
			}
		}

		dir.cd("..");
	}

#endif
	return 0;
}


void notify_old_instance(const QStringList& files_to_play, int pid){

	QSharedMemory memory("SayonaraMemory");

	if(!files_to_play.isEmpty()){

		QString filename = files_to_play[0] + "\n";
		QByteArray arr = filename.toUtf8();

		memory.attach(QSharedMemory::ReadWrite);

		if(memory.create(arr.size())){
			memory.lock();
			char* ptr = (char*) memory.data();
			int size = std::min(memory.size(), arr.size());

			memcpy(ptr,
					arr.data(),
					size);

			memory.unlock();
		}

		kill(pid, SIGUSR1);
	}

	else{
		kill(pid, SIGUSR2);
	}

	Helper::sleep_ms(500);

	if(memory.isAttached()){
		memory.detach();
	}

	sp_log(Log::Info) << "Another instance is already running";
}

#ifdef Q_OS_WIN
void init_gio(){
	QString gio_path = Helper::File::clean_filename(QApplication::applicationDirPath()) + QDir::separator() + "gio-modules";
	QString gst_plugin_path = Helper::File::clean_filename(QApplication::applicationDirPath()) + QDir::separator() + "gstreamer-1.0/";

	Helper::set_environment("GST_PLUGIN_PATH", gst_plugin_path);
	Helper::set_environment("GST_PLUGIN_SYSTEM_PATH", gst_plugin_path);

	g_io_extension_point_register("gio-tls-backend");
	g_io_modules_load_all_in_directory(gio_path.toLocal8Bit().data());

	sp_log(Log::Debug) << "Done " << gio_path;
}
#endif


void segfault_handler(int sig){

	Q_UNUSED(sig)

#ifdef Q_OS_LINUX

	void* array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 20);
	backtrace_symbols_fd(array, size, STDERR_FILENO);

#endif

}


int main(int argc, char *argv[]) {

	Application app(argc, argv);

	QTranslator translator;
	QString language;

#ifdef Q_OS_WIN
	init_gio();
#endif


#ifdef Q_OS_LINUX

	signal(SIGSEGV, segfault_handler);

#endif

	CommandLineData cmd_data = CommandLineParser::parse(argc, argv);
	if(cmd_data.abort){
		return 0;
	}

#ifdef Q_OS_LINUX

	int pid=0;
	if( !cmd_data.multiple_instances ){
		pid = check_for_another_instance(QCoreApplication::applicationPid());
	}

	if(pid > 0) {
		notify_old_instance(cmd_data.files_to_play, pid);
		return 0;
	}

#else
	Q_UNUSED(single_instance)
#endif


	/* Tell the settings manager which settings are necessary */
	if( !SettingRegistry::getInstance()->init() ){
		sp_log(Log::Error) << "Cannot initialize settings";
		return 1;
	}

	Q_INIT_RESOURCE(Icons);

#ifdef Q_OS_WIN
	Q_INIT_RESOURCE(IconsWindows);
#endif

	if(!QFile::exists( Helper::get_sayonara_path() )) {
		QDir().mkdir( Helper::get_sayonara_path() );
	}

	language = Settings::getInstance()->get(Set::Player_Language);

	translator.load(language, Helper::get_share_path() + "translations");

	if(!app.init(&translator, cmd_data.files_to_play)) {
		return 1;
	}

	app.setApplicationName("Sayonara");
	app.setWindowIcon(GUI::get_icon("logo"));

	app.exec();

	return 0;
}
