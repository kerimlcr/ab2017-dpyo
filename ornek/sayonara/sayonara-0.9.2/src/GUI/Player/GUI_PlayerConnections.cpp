/* GUI_PlayerConnections.cpp */

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

#include "GUI_Player.h"

#include "Components/TagEdit/MetaDataChangeNotifier.h"
#include "Components/Engine/EngineHandler.h"

#include "GUI/Helper/Shortcuts/ShortcutHandler.h"
#ifdef WITH_MTP
	#include "GUI/MTP/GUI_MTP.h"
#endif

void GUI_Player::setup_connections() {

	connect(btn_play,	&QPushButton::clicked,	this, &GUI_Player::play_clicked);
	connect(btn_fw,		&QPushButton::clicked,	this, &GUI_Player::next_clicked);
	connect(btn_bw,		&QPushButton::clicked,	this, &GUI_Player::prev_clicked);
	connect(btn_stop,	&QPushButton::clicked,	this, &GUI_Player::stop_clicked);
	connect(btn_mute,	&QPushButton::released,	this, &GUI_Player::mute_button_clicked);
	connect(btn_rec,	&QPushButton::toggled, this, &GUI_Player::rec_clicked);

	connect(_play_manager, &PlayManager::sig_playstate_changed, this, &GUI_Player::playstate_changed);
	connect(_play_manager, &PlayManager::sig_track_changed, this, &GUI_Player::track_changed);
	connect(_play_manager, &PlayManager::sig_position_changed_ms, this,	&GUI_Player::set_cur_pos_ms);
	connect(_play_manager, &PlayManager::sig_buffer, this, &GUI_Player::buffering);
	connect(_play_manager, &PlayManager::sig_volume_changed, this, &GUI_Player::volume_changed);
	connect(_play_manager, &PlayManager::sig_mute_changed, this, &GUI_Player::mute_changed);

	// engine
	EngineHandler* engine = EngineHandler::getInstance();
	connect(engine, &EngineHandler::sig_md_changed,	this, &GUI_Player::md_changed);
	connect(engine, &EngineHandler::sig_dur_changed, this, &GUI_Player::dur_changed);
	connect(engine, &EngineHandler::sig_br_changed,	this, &GUI_Player::br_changed);
	connect(engine, &EngineHandler::sig_cover_changed, this, &GUI_Player::cover_changed);

	// file
	connect(action_OpenFile, &QAction::triggered, this, &GUI_Player::open_files_clicked);
	connect(action_OpenFolder, &QAction::triggered, this, &GUI_Player::open_dir_clicked);
	connect(action_Close, &QAction::triggered, this, &GUI_Player::really_close);

#ifdef WITH_MTP
	connect(action_devices, &QAction::triggered, this, [=](){

		if(!_mtp){
			_mtp = new GUI_MTP(this);
		}

		_mtp->show();
	});
#endif

	// view
	connect(action_viewLibrary, &QAction::toggled, this, &GUI_Player::show_library);
	connect(action_Dark, &QAction::toggled, this, &GUI_Player::skin_toggled);
	connect(action_Fullscreen, &QAction::toggled, this, &GUI_Player::show_fullscreen_toggled);

	connect(splitter, &QSplitter::splitterMoved, this, &GUI_Player::main_splitter_moved);


	// about
	connect(action_about, &QAction::triggered, this, &GUI_Player::about);
	connect(action_help, &QAction::triggered, this, &GUI_Player::help);

	connect(sli_volume, &SearchSlider::sig_slider_moved, this, &GUI_Player::volume_slider_moved);
	connect(sli_progress, &SearchSlider::sig_slider_moved, this, &GUI_Player::seek);
	connect(sli_progress, &SearchSlider::sig_slider_hovered, this, &GUI_Player::set_progress_tooltip);


	MetaDataChangeNotifier* md_change_notifier = MetaDataChangeNotifier::getInstance();
	connect(md_change_notifier, &MetaDataChangeNotifier::sig_metadata_changed, this, &GUI_Player::id3_tags_changed);

	ShortcutHandler* sch = ShortcutHandler::getInstance();

	Shortcut sc1 = sch->add(Shortcut(this, "play_pause", tr("Play/Pause"), "Space"));
	Shortcut sc2 = sch->add(Shortcut(this, "stop", tr("Stop"), "Ctrl + Space"));
	Shortcut sc3 = sch->add(Shortcut(this, "next", tr("Next track"), "Ctrl + Right"));
	Shortcut sc4 = sch->add(Shortcut(this, "prev", tr("Previous track"), "Ctrl + Left"));
	Shortcut sc5 = sch->add(Shortcut(this, "vol_down", tr("Volume down"), "Ctrl + -"));
	Shortcut sc6 = sch->add(Shortcut(this, "vol_up", tr("Volume up"), "Ctrl++"));
	Shortcut sc7 = sch->add(Shortcut(this, "seek_fwd", tr("Seek forward"), "Alt+Right"));
	Shortcut sc8 = sch->add(Shortcut(this, "seek_bwd", tr("Seek backward"), "Alt+Left"));
	Shortcut sc9 = sch->add(Shortcut(this, "seek fwd_fast", tr("Seek forward (fast)"), "Shift+Right"));
	Shortcut sc10 = sch->add(Shortcut(this, "seek_bwd_fast", tr("Seek backward (fast)"), "Shift+Left"));

	sc1.create_qt_shortcut(this, _play_manager, SLOT(play_pause()));
	sc2.create_qt_shortcut(this, _play_manager, SLOT(stop()));
	sc3.create_qt_shortcut(this, _play_manager, SLOT(next()));
	sc4.create_qt_shortcut(this, _play_manager, SLOT(previous()));
	sc5.create_qt_shortcut(this, _play_manager, SLOT(volume_down()));
	sc6.create_qt_shortcut(this, _play_manager, SLOT(volume_up()));
	sc7.create_qt_shortcut(this, [=](){_play_manager->seek_rel_ms(2000);});
	sc8.create_qt_shortcut(this, [=](){_play_manager->seek_rel_ms(-2000);});
	sc9.create_qt_shortcut(this, [=](){
		qint64 ms = _play_manager->get_duration_ms() / 20;
		_play_manager->seek_rel_ms(ms);
	});

	sc10.create_qt_shortcut(this, [=](){
		qint64 ms = _play_manager->get_duration_ms() / 20;
		_play_manager->seek_rel_ms(-ms);
	});
}

QString GUI_Player::get_shortcut_text(const QString &shortcut_identifier) const
{
	if(shortcut_identifier == "play_pause"){
		return tr("Play/Pause");
	}
	if(shortcut_identifier == "stop"){
		return tr("Stop");
	}
	if(shortcut_identifier == "next"){
		return tr("Next track");
	}
	if(shortcut_identifier == "prev"){
		return tr("Previous track");
	}
	if(shortcut_identifier == "vol_down"){
		return tr("Volume down");
	}
	if(shortcut_identifier == "vol_up"){
		return tr("Volume up");
	}
	if(shortcut_identifier == "seek_fwd"){
		return tr("Seek forward");
	}
	if(shortcut_identifier == "seek_bwd"){
		return tr("Seek backward");
	}
	if(shortcut_identifier == "seek_fwd_fast"){
		return tr("Seek forward (fast)");
	}

	if(shortcut_identifier == "seek_bwd_fast"){
		return tr("Seek backward (fast)");
	}

	return "";
}



