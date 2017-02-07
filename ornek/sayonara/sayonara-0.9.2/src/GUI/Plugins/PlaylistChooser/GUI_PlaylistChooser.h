/* GUI_PlaylistChooser.h */

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


#ifndef GUIPLAYLISTCHOOSER_H_
#define GUIPLAYLISTCHOOSER_H_

#include "GUI/Plugins/PlaylistChooser/ui_GUI_PlaylistChooser.h"
#include "Interfaces/PlayerPlugin/PlayerPlugin.h"
#include "Helper/Playlist/CustomPlaylist.h"

#include <QMap>
#include <QDockWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QPixmap>


class PlaylistChooser;
class GUI_TargetPlaylistDialog;

class GUI_PlaylistChooser :
		public PlayerPluginInterface,
		private Ui::GUI_PlaylistChooser {

	Q_OBJECT

	friend class PlayerPluginInterface;

signals:
	void sig_playlist_chosen(int);
	void sig_delete_playlist(int);
	void sig_save_playlist(int);
	void sig_save_playlist(QString);
	void sig_save_playlist_file(QString, bool);
	void sig_closed();
	void sig_clear_playlist();
	void sig_files_selected(const QStringList&);


public:
	GUI_PlaylistChooser(QWidget *parent=nullptr);
	virtual ~GUI_PlaylistChooser();

	QString get_name() const override;
	QString get_display_name() const override;


private slots:
	void edit_text_changed(const QString& name);
	void save_button_pressed();
	void save_as_button_pressed();
	void delete_button_pressed();
	void load_button_pressed();
	void playlist_selected(int);

	void got_save_params(const QString&, bool);


public slots:
	void all_playlists_fetched(const CustomPlaylistSkeletons&);


private:

	GUI_TargetPlaylistDialog*		_target_playlist_dialog=nullptr;
	PlaylistChooser*				_playlist_chooser=nullptr;

	QString		_last_dir;


private:
	void language_changed() override;
	void init_ui() override;
};

#endif /* GUIPLAYLISTCHOOSER_H_ */
