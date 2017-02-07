/* GUI_SoundcloudArtistSearch.h */

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



#ifndef GUI_SOUNDCLOUDARTISTSEARCH_H
#define GUI_SOUNDCLOUDARTISTSEARCH_H

#include <QDialog>
#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"
#include "3rdParty/Soundcloud/ui_GUI_SoundcloudArtistSearch.h"
#include "3rdParty/Soundcloud/src/SoundcloudLibrary.h"
#include "3rdParty/Soundcloud/src/SoundcloudDataFetcher.h"


class GUI_SoundcloudArtistSearch : public SayonaraDialog, private Ui::GUI_SoundcloudArtistSearch
{
	Q_OBJECT

public:
	explicit GUI_SoundcloudArtistSearch(SoundcloudLibrary* library, QWidget *parent=nullptr);

private slots:
	void search_clicked();
	void clear_clicked();
	void add_clicked();
	void close_clicked();

	void artists_fetched(const ArtistList& artists);
	void artists_ext_fetched(const ArtistList& artists);
	void albums_fetched(const AlbumList& albums);
	void tracks_fetched(const MetaDataList& tracks);

	void artist_selected(int idx);

	void language_changed() override;

private:
	SoundcloudLibrary*			_library=nullptr;
	SoundcloudDataFetcher*		_fetcher=nullptr;


	MetaDataList		_v_md;
	AlbumList			_albums;
	ArtistList			_searched_artists;
	ArtistList			_chosen_artists;

	quint64				_cur_artist_sc_id;

private:
	void set_tracks_label(int n_tracks);
	void set_playlist_label(int n_playlists);

};

#endif // GUI_SOUNDCLOUDARTISTSEARCH_H
