/* SettingKey.h */

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



#ifndef SETTINGKEY_H
#define SETTINGKEY_H

#include "Helper/globals.h"

class QString;
class QPoint;
class QSize;
class LibSortOrder;
struct EQ_Setting;
struct RawShortcutMap;

class PlaylistMode;

/**
 * @brief The SK namespace is used to access setting keys
 * @ingroup Settings
 */
namespace SK {
enum SettingKey {

	LFM_Active=0,
	LFM_ScrobbleTimeSec,
	LFM_Login,
	LFM_Corrections,
	LFM_ShowErrors,
	LFM_SessionKey,

	Eq_Last,
	Eq_List,
	Eq_Gauss,

	Lib_Show,
	Lib_Path,
	Lib_ColsTitle,
	Lib_ColsArtist,
	Lib_ColsAlbum,
	Lib_LiveSearch,
	Lib_Sorting,
	Lib_CurPlugin,
	Lib_SplitterStateArtist,
	Lib_SplitterStateTrack,
	Lib_SplitterStateGenre,
	Lib_OldWidth,
	Lib_DC_DoNothing,
	Lib_DC_PlayIfStopped,
	Lib_DC_PlayImmediately,
	Lib_DD_DoNothing,
	Lib_DD_PlayIfStoppedAndEmpty,
	Lib_FontSize,
	Lib_FontBold,
	Lib_SearchMode,
	Lib_AutoUpdate,

	Player_Version,
	Player_Language,
	Player_FontName,
	Player_FontSize,
	Player_Style,
	Player_Size,
	Player_Pos,
	Player_Fullscreen,
	Player_Maximized,
	Player_ShownPlugin,
	Player_OneInstance,
	Player_Min2Tray,
	Player_StartInTray,
	Player_ShowTrayIcon,
	Player_NotifyNewVersion,
	Player_SplitterState,
	Player_Shortcuts,

	PL_Playlist,
	PL_LoadSavedPlaylists,
	PL_LoadTemporaryPlaylists,
	PL_LoadLastTrack,
	PL_RememberTime,
	PL_StartPlaying,
	PL_LastTrack,
	PL_LastPlaylist,
	PL_Mode,
	PL_ShowNumbers,
	PL_EntryLook,
	PL_FontSize,

	Notification_Show,
	Notification_Timeout,
	Notification_Name,

	Engine_Name,
	Engine_Vol,
	Engine_Mute,
	Engine_ConvertQuality,
	Engine_CovertTargetPath,
	Engine_ShowSpectrum,
	Engine_ShowLevel,
	Engine_CurTrackPos_s,
	Engine_CrossFaderActive,
	Engine_CrossFaderTime,
	Engine_Pitch,
	Engine_PreservePitch,
	Engine_Speed,
	Engine_SpeedActive,

	Engine_SR_Active,
	Engine_SR_Warning,
	Engine_SR_Path,
	Engine_SR_SessionPath,

	Spectrum_Style,
	Level_Style,

	Broadcast_Active,
	Broadcast_Prompt,
	Broadcast_Port,

	MP3enc_found,
	Pitch_found,
	Player_Quit,

	Remote_Active,
	Remote_Port,

	Num_Setting_Keys
};
}

template<typename T, SK::SettingKey S>
class SettingKey
{
public:
	T* p=nullptr;
	SettingKey(){}
	~SettingKey(){}
};


/*
 * typedef SettingKey<QString, SK::Lib_Path> LibPath_t; LibPath_t LibPath;
 *
 */

/**
  * @brief MACRO INST use this macro to declare a setting:\n
  * INST(boo, LFM_Active) is expanded to \n
  * typedef SettingKey<bool, SK::LFM_Active> LFM_Active_t; const LFM_Active_t LFM_Active
  * @ingroup Settings
  */
#define INST(type, settingkey) typedef SettingKey<type, SK:: settingkey> settingkey##_t; const settingkey##_t settingkey

/**
 * @brief Set namespace defines the setting: Which key and which type
 * @ingroup Settings
 */
namespace Set {

//typedef SettingKey<bool, SK::LFM_Active> LFM_Active_t; const LFM_Active_t LFM_Active
INST(bool,				LFM_Active);				/* is lastFM active? */
INST(int,				LFM_ScrobbleTimeSec);			/* time in sec when to scrobble */
INST(StringPair,			LFM_Login);				/* 2-Tupel, username, password */

INST(bool,				LFM_Corrections);			/* propose lfm corrections */
INST(bool,				LFM_ShowErrors);			/* get error message, if there are lfm problems */
INST(QString,			LFM_SessionKey);			/* lfm session key */

INST(int,				Eq_Last);				/* last equalizer index */
INST(QList<EQ_Setting>,		Eq_List);				/* All equalizers */
INST(bool,				Eq_Gauss);				/* do curve, when changing eq setting */

INST(bool,				Lib_Show);				/* show library */
INST(QString,			Lib_Path);				/* library path */
INST(BoolList,			Lib_ColsTitle);				/* shown columns tracks */
INST(BoolList,			Lib_ColsArtist);			/* shown columns artist */
INST(BoolList,			Lib_ColsAlbum);				/* shown columns albums */
INST(bool,				Lib_LiveSearch);			/* library live search */
INST(LibSortOrder,			Lib_Sorting);				/* how to sort in lib */
INST(QString,			Lib_CurPlugin);				/* Current shown library plugin */
INST(QByteArray,			Lib_SplitterStateArtist);		/* Splitter state between artists and albums */
INST(QByteArray,			Lib_SplitterStateTrack);		/* Splitter state between artists and tracks */
INST(QByteArray,			Lib_SplitterStateGenre);		/* Splitter state between tracks and genres */
INST(int,				Lib_OldWidth);				/* Old library width when hiding library */
INST(bool,				Lib_DC_DoNothing);			/* when double clicked, create playlist and do nothing*/
INST(bool,				Lib_DC_PlayIfStopped);			/* when double clicked, play if stopped */
INST(bool,				Lib_DC_PlayImmediately);		/* when double clicked, play immediately */
INST(bool,				Lib_DD_DoNothing);			/* when drag dropped, insert tracks and do nothing */
INST(bool,				Lib_DD_PlayIfStoppedAndEmpty);		/* when drag dropped, play if playlist is empty and stopped */
INST(int,				Lib_FontSize);				/* current library font size */
INST(bool,				Lib_FontBold);				/* current library font weight */
INST(int,				Lib_SearchMode);			/* Search mode in library. See */
INST(bool,				Lib_AutoUpdate);			/* Automatic update of library */

INST(QString,			Player_Version);			/* Version string of player */
INST(QString,			Player_Language);			/* language of player */
INST(int,				Player_Style);				/* dark or native: native = 0, dark = 1 */
INST(QString,			Player_FontName);			/* current font name */
INST(int,				Player_FontSize);			/* current font size */
INST(QSize,				Player_Size);				/* player size */
INST(QPoint,			Player_Pos);				/* player position */
INST(bool,				Player_Fullscreen);			/* player fullscreen */
INST(bool,				Player_Maximized);			/* player maximized */
INST(QString,			Player_ShownPlugin);			/* current shown plugin in player, empty if none */
INST(bool,				Player_OneInstance);			/* only one Sayonara instance is allowed */
INST(bool,				Player_Min2Tray);			/* minimize Sayonara to tray */
INST(bool,				Player_ShowTrayIcon);			/* Show/hide the tray icon */
INST(bool,				Player_StartInTray);			/* start in tray */
INST(bool,				Player_NotifyNewVersion);		/* check for new version on startup */
INST(QByteArray,			Player_SplitterState);			/* spliter state between playlist and library */
INST(RawShortcutMap,		Player_Shortcuts);			/* player shortcuts */

INST(QStringList,			PL_Playlist);				/* old playlist: list of integers in case of library tracks, if no library track, filepath */
INST(bool,				PL_LoadSavedPlaylists);			/* load saved playlists on startup */
INST(bool,				PL_LoadTemporaryPlaylists);		/* load temporary playlists on startup */
INST(bool,				PL_LoadLastTrack);			/* load last track on startup */
INST(bool,				PL_RememberTime);			/* remember time of last track */
INST(bool,				PL_StartPlaying);			/* start playing immediately when opening Sayonara */
INST(int,				PL_LastTrack);				/* last track idx in playlist */
INST(int,				PL_LastPlaylist);			/* last Playlist id, where LastTrack has been played */
INST(QString,			PL_EntryLook);				/* formatting of playlist entry */
INST(int,				PL_FontSize);				/* current playlist font size */

INST(PlaylistMode,			PL_Mode);				/* playlist mode: rep1, repAll, shuffle... */
INST(bool,				PL_ShowNumbers);			/* show numbers in playlist */
INST(bool,				Notification_Show);			/* show notifications */
INST(int,				Notification_Timeout);			/* notification timeout */
INST(QString,			Notification_Name);			/* type of notifications: libnotify or empty for native baloons :( */

INST(QString,			Engine_Name);				/* Deprecated: Engine name */
INST(int,				Engine_Vol);				/* Volume */
INST(bool,				Engine_Mute);				/* Muted/unmuted */
INST(int,				Engine_CurTrackPos_s);			/* position of track (used to load old position) */
INST(int,				Engine_ConvertQuality);			/* Convert quality, 1-10 for variable, > 64 for fixed bitrate */
INST(QString,			Engine_CovertTargetPath);		/* last convert path */
INST(bool,				Engine_ShowSpectrum);			/* show spectrum */
INST(bool,				Engine_ShowLevel);			/* show level */
INST(bool,				Engine_CrossFaderActive);		/* crossfader active */
INST(int,				Engine_CrossFaderTime);			/* crossfader overlap time */
INST(int, 				Engine_Pitch);				/* hertz of a */
INST(bool, 				Engine_SpeedActive);			/* is speed control active? */
INST(float,				Engine_Speed);				/* if yes, set speed */
INST(bool, 				Engine_PreservePitch);			/* if yes, should pitch be preserved? */

INST(bool,				Engine_SR_Active);			/* Streamripper active */
INST(bool,				Engine_SR_Warning);			/* streamripper warnings */
INST(QString,			Engine_SR_Path);			/* streamripper paths */
INST(bool,				Engine_SR_SessionPath);			/* streamripper session path */

INST(int,				Spectrum_Style);			/* index of spectrum style */
INST(int,				Level_Style);				/* index of level style */
INST(bool,				Broadcast_Active);			/* is broadcast active? */
INST(bool,				Broadcast_Prompt);			/* prompt when new connection arrives? */
INST(int,				Broadcast_Port);			/* broadcast port */

INST(bool,				Remote_Active);				/* Remote control activated */
INST(int,				Remote_Port);				/* Remote control port */



}

namespace SetNoDB{

INST(bool,				MP3enc_found);
INST(bool,				Pitch_found);
INST(bool,				Player_Quit);

}



#endif // SETTINGKEY_H
