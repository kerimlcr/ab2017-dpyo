/* application.cpp */

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

#include "application.h"
#include "InstanceThread.h"

#ifdef WITH_DBUS
	#include "Components/DBus/DBusHandler.h"
#endif

#ifdef Q_OS_WIN
	#include <windows.h>
	#include "3rdParty/SomaFM/ui/SomaFMLibraryContainer.h"
	#include "3rdParty/Soundcloud/ui/GUI_SoundcloudLibrary.h"
#endif

#include "Components/Playlist/PlaylistHandler.h"
#include "Components/RemoteControl/RemoteControl.h"
#include "Components/Engine/EngineHandler.h"

#include "Interfaces/LibraryInterface/LibraryPluginHandler.h"
#include "Interfaces/PlayerPlugin/PlayerPluginHandler.h"

#include "GUI/Helper/GUI_Helper.h"

#include "GUI/Player/GUI_Player.h"
#include "GUI/Library/LocalLibraryContainer.h"
#include "GUI/DirectoryWidget/GUI_DirectoryWidget.h"

#include "GUI/Plugins/PlaylistChooser/GUI_PlaylistChooser.h"
#include "GUI/Plugins/AudioConverter/GUI_AudioConverter.h"
#include "GUI/Plugins/Engine/GUI_LevelPainter.h"
#include "GUI/Plugins/Engine/GUI_Spectrum.h"
#include "GUI/Plugins/Stream/GUI_Stream.h"
#include "GUI/Plugins/Stream/GUI_Podcasts.h"
#include "GUI/Plugins/Equalizer/GUI_Equalizer.h"
#include "GUI/Plugins/Bookmarks/GUI_Bookmarks.h"
#include "GUI/Plugins/Speed/GUI_Speed.h"
#include "GUI/Plugins/Broadcasting/GUI_Broadcast.h"
#include "GUI/Plugins/Crossfader/GUI_Crossfader.h"

#include "GUI/Preferences/Fonts/GUI_FontConfig.h"
#include "GUI/Preferences/Notifications/GUI_Notifications.h"
#include "GUI/Preferences/LastFM/GUI_LastFM.h"
//#include "GUI/Preferences/Startup/GUI_StartupDialog.h"
#include "GUI/Preferences/Language/GUI_LanguageChooser.h"
#include "GUI/Preferences/Broadcast/GUI_BroadcastSetup.h"
#include "GUI/Preferences/PlaylistPreferences/GUI_PlaylistPreferences.h"
#include "GUI/Preferences/StreamRecorder/GUI_StreamRecorder.h"
#include "GUI/Preferences/RemoteControl/GUI_RemoteControl.h"
#include "GUI/Preferences/LibraryPreferences/GUI_LibraryPreferences.h"
#include "GUI/Preferences/Shortcuts/GUI_Shortcuts.h"
#include "GUI/Preferences/PlayerPreferences/GUI_PlayerPreferences.h"
#include "GUI/Preferences/PreferenceDialog/GUI_PreferenceDialog.h"


#include "Helper/FileHelper.h"
#include "Helper/Helper.h"

#include "Database/DatabaseConnector.h"

static InstanceMessage instance_message=InstanceMessageNone;

#ifdef Q_OS_UNIX

	#include <csignal>

	void new_instance_handler(int sig_num)
	{
		switch(sig_num)
		{
			case SIGUSR1:
				instance_message = InstanceMessageWithFiles;
				break;
			case SIGUSR2:
				instance_message = InstanceMessageWithoutFiles;
				break;
			default:
				break;
		}
	}

#else
	void global_key_handler(){

		if(!RegisterHotKey(NULL, 1, MOD_NOREPEAT, VK_MEDIA_PLAY_PAUSE)){
			return false;
		}

		MSG msg = {0};
		while (GetMessage(&msg, NULL, 0, 0) != 0)
	    {
    	    if (msg.message == WM_HOTKEY)
	        {
	        	UINT modifiers = msg.lParam;
				UINT key = msg.wParam;
	        }
	    } 
	}
#endif

Application::Application(int & argc, char ** argv) :
	QApplication(argc, argv),
	SayonaraClass()
{
	_timer = new QTime();
	_timer->start();
	instance_message = InstanceMessageNone;

	this->setQuitOnLastWindowClosed(false);
}

void Application::check_for_crash()
{
	QString error_file = Helper::get_error_file();

	if(!QFile::exists(error_file)) return;

	QString info_text;
	QString mail;

	if(!Helper::File::read_file_into_str(error_file, mail)){
		mail = "";
		mail.prepend("mailto:luciocarreras@gmail.com?subject=Sayonara Crash&amp;body=Hi Lucio,\n\nhere is the trace for a Sayonara crash\n\n");
	}

	else{
		mail.prepend("mailto:luciocarreras@gmail.com?subject=Sayonara Crash&amp;body=Hi Lucio,\n\nhere is the trace for a Sayonara crash\n\n");
		mail.append("\n\nI hope this will not happen again...");
	}

	info_text = QString("Sayonara seems to have crashed the last time<br />") +
			"Please send " +
			Helper::create_link(error_file, false) +
			" in " + Helper::create_link(Helper::get_sayonara_path(), false) +
			" to " + Helper::create_link("luciocarreras@gmail.com", false, mail);

	GlobalMessage::getInstance()->info(info_text);

	QFile f(error_file);
	f.open(QIODevice::ReadOnly);
	if(!f.isOpen()){
		sp_log(Log::Warning) << "Cannot oopen " << error_file;
		return;
	}

	f.remove();
	f.close();

	return;
}


bool Application::init(QTranslator* translator, const QStringList& files_to_play)
{
	_plh = PlaylistHandler::getInstance();
	_db = DatabaseConnector::getInstance();

	sp_log(Log::Debug) << "Init application: " << _timer->elapsed() << "ms";

	bool success = this->installTranslator(translator);
	if(!success){
		sp_log(Log::Warning) << "Cannot install translator";
	}

	//check_for_crash();

	QString version = QString(SAYONARA_VERSION);
	_settings->set(Set::Player_Version, version);

	sp_log(Log::Debug) << "Start player: " << _timer->elapsed() << "ms";
	player = new GUI_Player(translator);
	GUI::set_main_window(player);

	connect(player, &GUI_Player::sig_player_closed, this, &QCoreApplication::quit);

	sp_log(Log::Debug) << "Init player: " << _timer->elapsed() << "ms";

#ifdef WITH_DBUS
	DBusHandler* dbus	= new DBusHandler(player, this);
	Q_UNUSED(dbus)

#endif

	RemoteControl* rmc = new RemoteControl(this);
	Q_UNUSED(rmc)

	if(_settings->get(Set::Notification_Show)){
		NotificationHandler::getInstance()->notify("Sayonara Player",
												   tr("Version ") + SAYONARA_VERSION,
												   Helper::get_share_path() + "logo.png");
	}

	sp_log(Log::Debug) << "Init plugins: " << _timer->elapsed() << "ms";
	PlayerPluginHandler* pph = new PlayerPluginHandler(this);

	pph->add_plugin(new GUI_LevelPainter());
	pph->add_plugin(new GUI_Spectrum());
	pph->add_plugin(new GUI_Equalizer());
	pph->add_plugin(new GUI_Stream());
	pph->add_plugin(new GUI_Podcasts());
	pph->add_plugin(new GUI_PlaylistChooser());
	pph->add_plugin(new GUI_AudioConverter());
	pph->add_plugin(new GUI_Bookmarks());
	pph->add_plugin(new GUI_Speed());
	pph->add_plugin(new GUI_Broadcast());
	pph->add_plugin(new GUI_Crossfader());
	pph->load_dynamic_plugins();

	sp_log(Log::Debug) << "Plugins finsihed: " << _timer->elapsed() << "ms";

	QList<LibraryContainerInterface*> library_containers;
	DirectoryLibraryContainer* directory_container = new DirectoryLibraryContainer(this);
	LocalLibraryContainer* local_library_container = new LocalLibraryContainer(this);
	LibraryPluginHandler* library_plugin_loader = new LibraryPluginHandler(this);

	library_containers << static_cast<LibraryContainerInterface*>(local_library_container);
	library_containers << static_cast<LibraryContainerInterface*>(directory_container);

#ifdef Q_OS_WIN
	SoundcloudLibraryContainer* soundcloud_container = new SoundcloudLibraryContainer(this);
	SomaFMLibraryContainer* somafm_container = new SomaFMLibraryContainer(this);
	library_containers << static_cast<LibraryContainerInterface*>(soundcloud_container);
	library_containers << static_cast<LibraryContainerInterface*>(somafm_container);
#endif
	library_plugin_loader->init(library_containers);

	sp_log(Log::Debug) << "Libraries loaded: " << _timer->elapsed() << "ms";

	GUI_PreferenceDialog* preferences = new GUI_PreferenceDialog(player);

	player->register_preference_dialog(preferences);

	preferences->register_preference_dialog(new GUI_LanguageChooser());
	preferences->register_preference_dialog(new GUI_FontConfig());
	preferences->register_preference_dialog(new GUI_PlayerPreferences());
	preferences->register_preference_dialog(new GUI_PlaylistPreferences());
	preferences->register_preference_dialog(new GUI_LibraryPreferences());
	preferences->register_preference_dialog(new GUI_Shortcuts());
	preferences->register_preference_dialog(new GUI_Notifications());
	preferences->register_preference_dialog(new GUI_LastFM());
	preferences->register_preference_dialog(new GUI_StreamRecorder());
	preferences->register_preference_dialog(new GUI_BroadcastSetup());
	preferences->register_preference_dialog(new GUI_RemoteControl());

	EngineHandler::getInstance()->init();

	sp_log(Log::Debug) << "Preference dialogs loaded: " << _timer->elapsed() << "ms";

	player->set_libraries(library_plugin_loader);
	player->set_player_plugin_handler(pph);
	player->ui_loaded();

	if(files_to_play.size() > 0) {

		QString playlist_name = _plh->request_new_playlist_name();
		_plh->create_playlist(files_to_play, playlist_name);
	}

	init_single_instance_thread();

	sp_log(Log::Debug) << "Time to start: " << _timer->elapsed() << "ms";

	return true;
}


Application::~Application()
{
	if(_instance_thread){
		_instance_thread->stop();
		while(_instance_thread->isRunning()){
			Helper::sleep_ms(100);
		}
	}

	if(player){
		delete player; player=nullptr;
	}

	if(_plh){
		_plh->save_all_playlists();
	}

	if(_db){
		_db->store_settings();
		_db->close_db();
	}

	if(_timer)
	{
		delete _timer; _timer=nullptr;
	}
}


void Application::init_single_instance_thread()
{

#ifdef Q_OS_UNIX
	signal(SIGUSR1, new_instance_handler);
	signal(SIGUSR2, new_instance_handler);
#endif

	_instance_thread = new InstanceThread(&instance_message, this);

	connect(_instance_thread, &InstanceThread::sig_player_raise, player, &GUI_Player::raise);
	connect(_instance_thread, SIGNAL(sig_create_playlist(const QStringList&, const QString&, bool)),
			_plh, SLOT(create_playlist(const QStringList&, const QString&, bool)));

	_instance_thread->start();
}

