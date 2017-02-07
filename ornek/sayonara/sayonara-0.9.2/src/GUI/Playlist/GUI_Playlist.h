/* GUI_Playlist.h */

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
 * GUI_Playlist.h
 *
 *  Created on: Apr 6, 2011
 *      Author: Lucio Carreras
 */

#ifndef GUI_PLAYLIST_H_
#define GUI_PLAYLIST_H_

#include "GUI/Playlist/ui_GUI_Playlist.h"
#include "Components/Playlist/PlaylistHandler.h"
#include "GUI/Helper/Message/GlobalMessage.h"

#include <QTextEdit>
#include <QFocusEvent>
#include <QKeyEvent>


class PlaylistView;
class GUI_Playlist : public SayonaraWidget, private Ui::Playlist_Window
{
	Q_OBJECT

public:
	GUI_Playlist(QWidget *parent=nullptr);
	~GUI_Playlist();

private:
	PlayManager*			_play_manager=nullptr;
	PlaylistHandler*		_playlist=nullptr;
	Playlist::Type			_playlist_type;

private:
	PlaylistView* get_view_by_idx(int idx);
	PlaylistView* get_current_view();

	void init_shortcuts();
	void set_total_time_label();

	/** Overridden events **/
	void resizeEvent(QResizeEvent *e) override;
	void language_changed() override;
	void skin_changed() override;

	void dragEnterEvent(QDragEnterEvent* event) override;
	void dragLeaveEvent(QDragLeaveEvent* event) override;
	void dropEvent(QDropEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void changeEvent(QEvent* e) override;

	GlobalMessage::Answer show_save_message_box(PlaylistDBInterface::SaveAsAnswer answer);

private slots:

	void load_old_playlists();

	// triggered from playlist
	void playlist_track_changed(int row, int pl_idx); // GUI_Playlist.cpp
	void playlist_fill(PlaylistPtr pl); // GUI_Playlist.cpp
	void playlist_added(PlaylistPtr pl); // GUI_PlaylistTabs.cpp
	void playlist_name_changed(int pl_idx);// GUI_PlaylistTabs.cpp
	void playlist_changed(int pl_idx);
	void playlist_idx_changed(int pld_idx);

	// triggered by GUI
	void tab_close_playlist_clicked(int pl_idx); // GUI_PlaylistTabs.cpp
	void tab_save_playlist_clicked(int pl_idx); // GUI_PlaylistTabs.cpp
	void tab_save_playlist_as_clicked(int pl_idx, const QString& str); // GUI_PlaylistTabs.cpp
	void tab_rename_clicked(int pl_idx, const QString& str);
	void tab_delete_playlist_clicked(int pl_idx); // GUI_PlaylistTabs.cpp
	void tab_metadata_dropped(int pl_idx, const MetaDataList& v_md);
	void open_file_clicked(int pl_idx);
	void open_dir_clicked(int pl_idx);

	void check_tab_icon();
	void check_playlist_menu(PlaylistConstPtr pl);
	void check_playlist_name(PlaylistConstPtr pl);

	void double_clicked(int row);

	void add_playlist_button_pressed();

	void clear_button_pressed(int pl_idx);

	void select_tab_left();
	void select_tab_right();


	// called by playmanager
	void playstate_changed(PlayManager::PlayState state);
	void playlist_finished();
	void playlist_time_changed();


	void _sl_show_numbers_changed();
	void _sl_library_path_changed();
};





#endif /* GUI_PLAYLIST_H_ */
