
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

#include "GUI_Playlist.h"
#include "View/PlaylistView.h"
#include "TabWidget/PlaylistMenuEntry.h"
#include "GUI/Helper/IconLoader/IconLoader.h"
#include "GUI/Helper/Message/Message.h"


void GUI_Playlist::playlist_name_changed(int idx){

	PlaylistConstPtr pl = _playlist->get_playlist_at(idx);
	if(!pl){
		return;
	}

	QString name = pl->get_name();
	check_playlist_name(pl);

	for(int i = tw_playlists->count() - 2; i>=0; i--) {
		if(i == idx){
			continue;
		}

		if(tw_playlists->tabText(i).compare(name) == 0){
			tw_playlists->removeTab(i);
		}
	}
}


void GUI_Playlist::playlist_changed(int idx) {

	PlaylistConstPtr pl = _playlist->get_playlist_at(idx);
	PlaylistView* plv = get_view_by_idx(idx);

	check_playlist_name(pl);

	if(plv){
		plv->setFocus();
	}


	if(idx != tw_playlists->currentIndex()){
		return;
	}

	set_total_time_label();
	check_playlist_menu(pl);
}


void GUI_Playlist::select_tab_left()
{
	int cur_idx = tw_playlists->currentIndex();
	if(cur_idx > 0 ){
		playlist_idx_changed(cur_idx - 1);
	}
}

void GUI_Playlist::select_tab_right()
{
	int cur_idx = tw_playlists->currentIndex();
	if(cur_idx < tw_playlists->count() - 1 ){
		playlist_idx_changed(cur_idx + 1);
	}
}

void GUI_Playlist::playlist_idx_changed(int pl_idx){

	if(!between(pl_idx, tw_playlists->count() - 1)){
		return;
	}

	PlaylistConstPtr pl = _playlist->get_playlist_at(pl_idx);
	tw_playlists->setCurrentIndex(pl_idx);

	set_total_time_label();
	check_playlist_menu(pl);
}


void GUI_Playlist::playlist_added(PlaylistPtr pl){

	PlaylistView* pl_view;
	int idx;
	QString name;

	pl_view = new PlaylistView(pl);
	pl_view->setObjectName("playlist_view" + QString::number(pl->get_idx()));

	idx = pl->get_idx();
	name = pl->get_name();

	tw_playlists->insertTab(tw_playlists->count() - 1, pl_view, name);
	_playlist->set_current_idx(idx);

	connect(pl_view, &PlaylistView::sig_double_clicked, this, &GUI_Playlist::double_clicked);
	connect(pl_view, &PlaylistView::sig_left_tab_clicked, this, &GUI_Playlist::select_tab_left);
	connect(pl_view, &PlaylistView::sig_right_tab_clicked, this, &GUI_Playlist::select_tab_right);
	connect(pl_view, &PlaylistView::sig_time_changed, this, &GUI_Playlist::playlist_time_changed);

	connect(pl.get(), &Playlist::sig_data_changed, this, &GUI_Playlist::playlist_changed);
}


void GUI_Playlist::playstate_changed(PlayManager::PlayState state){
	if(state == PlayManager::PlayState::Stopped){
		check_tab_icon();
	}
}


void GUI_Playlist::playlist_finished(){
	check_tab_icon();
}



/** GUI SLOTS **/


void GUI_Playlist::tab_close_playlist_clicked(int idx){

	QWidget* playlist_widget;
	PlaylistView* plv;
	int count = tw_playlists->count();

	if( !between(idx, count - 1)) {
		return;
	}

	playlist_widget = tw_playlists->widget(idx);

	tw_playlists->removeTab(idx);

	plv = get_current_view();
	if(plv){
		plv->setFocus();
	}

	_playlist->close_playlist(idx);
	_playlist->set_current_idx(tw_playlists->currentIndex());

	set_total_time_label();

	delete playlist_widget; playlist_widget = nullptr;
}


void GUI_Playlist::tab_save_playlist_clicked(int idx){

	PlaylistDBInterface::SaveAsAnswer success = _playlist->save_playlist(idx);

	if(success == PlaylistDBInterface::SaveAsAnswer::Success){

		QString old_string = tw_playlists->tabText(idx);

		if(old_string.startsWith("* ")){
			old_string = old_string.right(old_string.size() - 2);
		}

		tw_playlists->setTabText(idx, old_string);
	}

	show_save_message_box(success);
}


void GUI_Playlist::tab_save_playlist_as_clicked(int idx, const QString& str){

	PlaylistDBInterface::SaveAsAnswer success;

	success = _playlist->save_playlist_as(idx, str, false);

	if(success == PlaylistDBInterface::SaveAsAnswer::AlreadyThere){

		GlobalMessage::Answer answer = show_save_message_box(success);

		if(answer == GlobalMessage::Answer::No) {
			return;
		}

		success = _playlist->save_playlist_as(idx, str, true);
	}

	show_save_message_box(success);
}


void GUI_Playlist::tab_rename_clicked(int idx, const QString& str){

	PlaylistDBInterface::SaveAsAnswer success = _playlist->rename_playlist(idx, str);

	if(success == PlaylistDBInterface::SaveAsAnswer::AlreadyThere){
		Message::error(tr("Playlist name already exists"));
	}

	else{
		show_save_message_box(success);
	}
}


void GUI_Playlist::tab_delete_playlist_clicked(int idx){

	GlobalMessage::Answer answer = Message::question_yn(tr("Really"), tr("Delete"));

	if(answer == GlobalMessage::Answer::No) {
		return;
	}

	_playlist->delete_playlist(idx);
}



void GUI_Playlist::check_tab_icon(){

	int active_idx;

	active_idx = _playlist->get_active_idx_of_cur_track();
	PlaylistView* plv = get_view_by_idx(active_idx);

	for(int i=0; i<tw_playlists->count(); i++){
		tw_playlists->setIconSize(QSize(16, 16));
		tw_playlists->setTabIcon(i, QIcon());
	}

	if(!plv){
		return;
	}

	PlayManager::PlayState state = PlayManager::getInstance()->get_play_state();
	if(state == PlayManager::PlayState::Stopped){
		return;
	}

	if(plv->get_num_rows() == 0){
		return;
	}

	IconLoader* icon_loader = IconLoader::getInstance();
	QIcon icon = icon_loader->get_icon("media-playback-start", "play_bordered");

	tw_playlists->tabBar()->setTabIcon(active_idx, icon);
}


void GUI_Playlist::check_playlist_menu(PlaylistConstPtr pl){

	PlaylistMenuEntries entries = PlaylistMenuEntry::None;

	bool temporary = pl->is_temporary();
	bool was_changed = pl->was_changed();
	bool storable = pl->is_storable();

	bool save_enabled =		(!temporary && storable);
	bool save_as_enabled =	(storable);
	bool delete_enabled =	(!temporary && storable);
	bool reset_enabled =	(!temporary && storable && was_changed);
	bool close_enabled =	(tw_playlists->count() > 2);
	bool rename_enabled =	(storable);
	bool clear_enabled =	(!pl->is_empty());

	entries |= PlaylistMenuEntry::OpenFile;
	entries |= PlaylistMenuEntry::OpenDir;

	if(save_enabled){
		entries |= PlaylistMenuEntry::Save;
	}
	if(save_as_enabled){
		entries |= PlaylistMenuEntry::SaveAs;
	}
	if(delete_enabled){
		entries |= PlaylistMenuEntry::Delete;
	}
	if(reset_enabled){
		entries |= PlaylistMenuEntry::Reset;
	}
	if(close_enabled){
		entries |= PlaylistMenuEntry::Close;
		entries |= PlaylistMenuEntry::CloseOthers;
	}
	if(rename_enabled){
		entries |= PlaylistMenuEntry::Rename;
	}
	if(clear_enabled){
		entries |= PlaylistMenuEntry::Clear;
	}

	tw_playlists->show_menu_items(entries);
}

void GUI_Playlist::check_playlist_name(PlaylistConstPtr pl)
{
	QString name = pl->get_name();

	if(!pl->is_temporary() &&
		pl->was_changed() &&
		pl->is_storable())
	{
		name.prepend("* ");
	}

	tw_playlists->setTabText(pl->get_idx(), name);
}


/** Private methods **/

GlobalMessage::Answer GUI_Playlist::show_save_message_box(PlaylistDBInterface::SaveAsAnswer answer){

	switch(answer){
		case PlaylistDBInterface::SaveAsAnswer::Error:
			Message::warning(tr("Cannot save playlist."), tr("Save as"));
			break;

		case PlaylistDBInterface::SaveAsAnswer::AlreadyThere:
			return Message::question_yn(tr("Playlist exists") + "\n" + tr("Overwrite?"), tr("Save as"));

		case PlaylistDBInterface::SaveAsAnswer::ExternTracksError:
			return Message::warning(tr("Playlists are currently only supported for library tracks."), tr("Save playlist"));

		default:
			return GlobalMessage::Answer::Undefined;
	}

	return GlobalMessage::Answer::Undefined;
}


PlaylistView* GUI_Playlist::get_view_by_idx(int idx){

	if(!between(idx, tw_playlists->count() - 1)){
		return nullptr;
	}

	PlaylistView* plv = static_cast<PlaylistView*>(tw_playlists->widget(idx));
	return plv;
}


PlaylistView* GUI_Playlist::get_current_view(){

	int idx = tw_playlists->currentIndex();
	if(!between(idx, tw_playlists->count() - 1)){
		return nullptr;
	}

	PlaylistView* plv = static_cast<PlaylistView*>(tw_playlists->widget(idx));
	return plv;
}
