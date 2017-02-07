/* GUI_SoundCloudLibrary.h */

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



#ifndef GUI_SOUNDCLOUDLIBRARY_H
#define GUI_SOUNDCLOUDLIBRARY_H

#include "3rdParty/Soundcloud/ui_GUI_SoundcloudLibrary.h"

#include "GUI/Library/GUI_AbstractLibrary.h"

#include "3rdParty/Soundcloud/src/SoundcloudLibrary.h"
#include "3rdParty/Soundcloud/ui/GUI_SoundcloudArtistSearch.h"

#include <QtGlobal>

class GUI_SoundCloudLibrary :
		public GUI_AbstractLibrary,
		protected Ui::GUI_SoundcloudLibrary
{
	friend class GUI_AbstractLibrary;
	Q_OBJECT


public:
	explicit GUI_SoundCloudLibrary(SoundcloudLibrary* library, QWidget *parent=nullptr);

	QComboBox*	get_libchooser() const;
	QMenu*		get_menu() const;


protected:
	virtual Library::TrackDeletionMode show_delete_dialog(int n_tracks) override;
	virtual void init_shortcuts() override;

protected slots:
	void btn_add_clicked();


private:
	GUI_SoundcloudArtistSearch* _artist_search=nullptr;
	QMenu*						_library_menu=nullptr;

};



#endif // GUI_SOUNDCLOUDLIBRARY_H
