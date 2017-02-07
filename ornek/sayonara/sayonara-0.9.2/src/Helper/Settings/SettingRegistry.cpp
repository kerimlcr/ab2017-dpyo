/* SettingRegistry.cpp */

/* Copyright (C) 2011-2015  Lucio Carreras
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



#include "SettingRegistry.h"
#include "Database/DatabaseConnector.h"
#include "Helper/Playlist/PlaylistMode.h"
#include "Helper/EqualizerPresets.h"
#include "Helper/LibrarySearchMode.h"
#include "GUI/Helper/Shortcuts/RawShortcutMap.h"

SettingRegistry::SettingRegistry()
{
	_settings = Settings::getInstance();
}

SettingRegistry::~SettingRegistry()
{

}


bool SettingRegistry::init(){

	DatabaseConnector* db = DatabaseConnector::getInstance();
	if(!db->is_initialized()){
		return false;
	}

	BoolList shown_cols;
	for(int i=0; i<10; i++){
		shown_cols << true;
	}

	register_setting( Set::LFM_Login, "LastFM_login", StringPair("None", "None"));
	register_setting( Set::LFM_ScrobbleTimeSec, "lfm_scrobble_time", 10);
	register_setting( Set::LFM_Active, "LastFM_active", false );
	register_setting( Set::LFM_Corrections, "lfm_corrections", false );
	register_setting( Set::LFM_ShowErrors, "lfm_q.show_errors", false );
	register_setting( Set::LFM_SessionKey, "lfm_session_key", QString() );

	register_setting( Set::Eq_Last, "eq_last", 0);
	register_setting( Set::Eq_List, "EQ_list", EQ_Setting::get_defaults() );
	register_setting( Set::Eq_Gauss, "EQ_Gauss", true );

	register_setting( Set::Lib_ColsTitle, "lib_shown_cols_title", shown_cols );
	register_setting( Set::Lib_ColsArtist, "lib_shown_cols_artist", shown_cols );
	register_setting( Set::Lib_ColsAlbum, "lib_shown_cols_album", shown_cols );
	register_setting( Set::Lib_LiveSearch, "lib_live_search", true );
	register_setting( Set::Lib_Sorting, "lib_sortings", LibSortOrder() );
	register_setting( Set::Lib_Path, "library_path", QString() );
	register_setting( Set::Lib_Show, "show_library", true );
	register_setting( Set::Lib_CurPlugin ,"last_lib_plugin", "local_library");
	register_setting( Set::Lib_SplitterStateArtist ,"splitter_state_artist", QByteArray());
	register_setting( Set::Lib_SplitterStateGenre ,"splitter_state_genre", QByteArray());
	register_setting( Set::Lib_SplitterStateTrack ,"splitter_state_track", QByteArray());
	register_setting( Set::Lib_OldWidth ,"lib_old_width", 0);
	register_setting( Set::Lib_DC_DoNothing ,"lib_dc_do_nothing", true);
	register_setting( Set::Lib_DC_PlayIfStopped ,"lib_dc_play_if_stopped", false);
	register_setting( Set::Lib_DC_PlayImmediately ,"lib_dc_play_immediately", false);
	register_setting( Set::Lib_DD_DoNothing ,"lib_dd_do_nothing", true);
	register_setting( Set::Lib_DD_PlayIfStoppedAndEmpty ,"lib_dd_play_if_stopped_and_empty", false);
	register_setting( Set::Lib_SearchMode, "lib_search_mode", LibraryHelper::CaseInsensitve);
	register_setting( Set::Lib_AutoUpdate, "lib_auto_update", false);

#ifdef Q_OS_WIN
	register_setting( Set::Lib_FontBold ,"lib_font_bold", false);
	register_setting( Set::Lib_FontSize ,"lib_font_size", 8);
#else
	register_setting( Set::Lib_FontBold ,"lib_font_bold", true);
	register_setting( Set::Lib_FontSize ,"lib_font_size", -1);
#endif

	register_setting( Set::Player_Version, "player_version", QString(SAYONARA_VERSION));
	register_setting( Set::Player_Language, "player_language", "sayonara_lang_en" );
	register_setting( Set::Player_Style, "player_style", 0 );
	register_setting( Set::Player_FontName, "player_font", QString() );
	register_setting( Set::Player_FontSize, "player_font_size", 10 );
	register_setting( Set::Player_Size, "player_size", QSize(800,600) );
	register_setting( Set::Player_Pos, "player_pos", QPoint(50,50) );
	register_setting( Set::Player_Fullscreen, "player_fullscreen", false );
	register_setting( Set::Player_Maximized, "player_maximized", false );
	register_setting( Set::Player_ShownPlugin, "shown_plugin", QString() );
	register_setting( Set::Player_OneInstance, "only_one_instance", true );
	register_setting( Set::Player_Min2Tray, "min_to_tray", false );
	register_setting( Set::Player_StartInTray, "start_in_tray", false );
	register_setting( Set::Player_ShowTrayIcon, "show_tray_icon", true );
	register_setting( Set::Player_NotifyNewVersion, "notify_new_version", true );
	register_setting( Set::Player_SplitterState ,"splitter_state_player", QByteArray());
	register_setting( Set::Player_Shortcuts, "shortcuts", RawShortcutMap());

	register_setting( Set::PL_Playlist, "playlist", QStringList() );
	register_setting( Set::PL_LoadSavedPlaylists, "load_saved_playlists", false );
	register_setting( Set::PL_LoadTemporaryPlaylists, "load_temporary_playlists", false );
	register_setting( Set::PL_LoadLastTrack, "load_last_track", false );
	register_setting( Set::PL_RememberTime, "remember_time", false );
	register_setting( Set::PL_StartPlaying, "start_playing", false );
	register_setting( Set::PL_LastTrack, "last_track", -1 );
	register_setting( Set::PL_LastPlaylist, "last_playlist", -1 );
	register_setting( Set::PL_Mode, "playlist_mode", PlaylistMode() );
	register_setting( Set::PL_ShowNumbers, "show_playlist_numbers", true );
	register_setting( Set::PL_EntryLook, "playlist_look", QString("*%title%* - %artist%"));
	register_setting( Set::PL_FontSize, "playlist_font_size", -1);

	register_setting( Set::Notification_Show, "show_notifications", true );
	register_setting( Set::Notification_Timeout, "notification_timeout", 5000 );
	register_setting( Set::Notification_Name, "notification_name", "DBus" );

	register_setting( Set::Engine_Name, "sound_engine", QString() );
	register_setting( Set::Engine_CurTrackPos_s, "last_track_pos", 0 );
	register_setting( Set::Engine_Vol, "volume", 50 );
	register_setting( Set::Engine_Mute, "mute", false );
	register_setting( Set::Engine_ConvertQuality, "convert_quality", 0 );
	register_setting( Set::Engine_CovertTargetPath, "convert_target_path", QDir::homePath() );
	register_setting( Set::Engine_ShowLevel, "show_level", false);
	register_setting( Set::Engine_ShowSpectrum, "show_spectrum", false);
	register_setting( Set::Engine_SR_Active, "streamripper", false );
	register_setting( Set::Engine_SR_Warning, "streamripper_warning", true );
	register_setting( Set::Engine_SR_Path, "streamripper_path", QDir::homePath() );
	register_setting( Set::Engine_SR_SessionPath, "streamripper_session_path", true );
	register_setting( Set::Engine_CrossFaderActive, "crossfader_active", false);
	register_setting( Set::Engine_CrossFaderTime, "crossfader_time", 5000);
	register_setting( Set::Engine_Pitch, "engine_pitch", 440);
	register_setting( Set::Engine_PreservePitch, "engine_preserve_pitch", false);
	register_setting( Set::Engine_SpeedActive, "engine_speed_active", false);
	register_setting( Set::Engine_Speed, "engine_speed", 1.0f);


	register_setting( Set::Spectrum_Style, "spectrum_style", 0 );
	register_setting( Set::Level_Style, "level_style", 0 );

	register_setting( Set::Broadcast_Active, "broadcast_active", false );
	register_setting( Set::Broadcast_Prompt, "broadcast_prompt", false );
	register_setting( Set::Broadcast_Port, "broadcast_port", 54054 );

	register_setting( Set::Remote_Active, "remote_control_active", false);
	register_setting( Set::Remote_Port, "remote_control_port", 54055);

	register_setting( SetNoDB::MP3enc_found, true );
	register_setting( SetNoDB::Pitch_found, true );
	register_setting( SetNoDB::Player_Quit, false );

	bool success = _settings->check_settings();
	if (!success){
		return false;
	}

	success = db->load_settings();
	if(!success){
		return false;
	}

	return true;
}

