/* GUI_Target_Playlist_Dialog.h */

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

#ifndef GUI_TARGET_PLAYLIST_DIALOG_H
#define GUI_TARGET_PLAYLIST_DIALOG_H

#include "GUI/Plugins/PlaylistChooser/ui_GUI_TargetPlaylistDialog.h"
#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"

class GUI_TargetPlaylistDialog :
		public SayonaraDialog,
		private Ui::GUI_TargetPlaylistDialog
{

    Q_OBJECT

public:
	GUI_TargetPlaylistDialog(QWidget *parent=nullptr);
	virtual ~GUI_TargetPlaylistDialog();

signals:
	void sig_target_chosen(const QString& name, bool relative);
    
private slots:
    void search_button_clicked();
    void ok_button_clicked();

protected:
	void language_changed() override;
};

#endif // GUI_TARGET_PLAYLIST_DIALOG_H
