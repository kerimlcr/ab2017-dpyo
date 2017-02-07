/* GUI_AbstractLibrary.h */

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



#ifndef GUI_ABSTRACTLIBRARY_H
#define GUI_ABSTRACTLIBRARY_H

#include "Components/Library/LibraryNamespaces.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"

#include "Views/LibraryView.h"
#include "Views/LibraryViewAlbum.h"

#include "Models/LibraryItemModelAlbums.h"
#include "Models/LibraryItemModelArtists.h"
#include "Models/LibraryItemModelTracks.h"

#include "Delegates/LibraryItemDelegateAlbums.h"
#include "Delegates/LibraryItemDelegateTracks.h"

#include "Helper/Settings/SayonaraClass.h"
#include "Helper/Filter.h"

#include <QStringList>
#include <QPoint>
#include <QList>
#include <QMap>
#include <QIcon>
#include <QComboBox>
#include <QPushButton>

class ColumnHeader;
class AbstractLibrary;

class GUI_AbstractLibrary :
		public SayonaraWidget
{
	Q_OBJECT

public:
	explicit GUI_AbstractLibrary(AbstractLibrary* library,
								 QWidget *parent=nullptr);

	virtual ~GUI_AbstractLibrary();

	void set_lib_chooser(const QMap<QString, QIcon>&);
	void set_index(int idx);


protected:
	AbstractLibrary*	_library = nullptr;

	Filter				_cur_searchfilter;

	BoolList	_shown_cols_albums;
	BoolList	_shown_cols_artist;
	BoolList	_shown_cols_tracks;


	LibraryItemModelTracks* 	_track_model = nullptr;
	LibraryItemModelAlbums* 	_album_model = nullptr;
	LibraryItemModelArtists*	_artist_model = nullptr;

	LibraryItemDelegateTracks* 		_track_delegate = nullptr;
	LibraryItemDelegateAlbums* 		_album_delegate = nullptr;
	QItemDelegate*					_artist_delegate = nullptr;

	virtual void init_headers();
	virtual void init_shortcuts();
	virtual void language_changed() override;
	virtual Library::TrackDeletionMode show_delete_dialog(int n_tracks)=0;

private:
	QComboBox*			_combo_libchooser = nullptr;
	LibraryView*		_lv_artist = nullptr;
	LibraryViewAlbum*	_lv_album = nullptr;
	LibraryView*		_lv_tracks = nullptr;

	QComboBox*			_combo_search = nullptr;
	QPushButton*		_btn_clear = nullptr;
	QLineEdit*			_le_search = nullptr;
	QPushButton*		_btn_refresh = nullptr;


	void init_finished();


protected slots:
	virtual void _sl_live_search_changed();

protected slots:

	virtual void refresh();

	virtual void lib_fill_tracks(const MetaDataList&);
	virtual void lib_fill_albums(const AlbumList&);
	virtual void lib_fill_artists(const ArtistList&);
	virtual void track_info_available(const MetaDataList& v_md);

	virtual void artist_sel_changed(const SP::Set<int>&);
	virtual void album_sel_changed(const SP::Set<int>&);
	virtual void track_sel_changed(const SP::Set<int>&);

	virtual void artist_middle_clicked(const QPoint& p);
	virtual void album_middle_clicked(const QPoint& p);
	virtual void tracks_middle_clicked(const QPoint& p);

	virtual void artist_dbl_clicked(const QModelIndex &);
	virtual void album_dbl_clicked(const QModelIndex &);
	virtual void track_dbl_clicked(const QModelIndex &);

	virtual void columns_title_changed(const BoolList&);
	virtual void columns_album_changed(const BoolList&);
	virtual void columns_artist_changed(const BoolList&);

	virtual void sortorder_title_changed(SortOrder);
	virtual void sortorder_album_changed(SortOrder);
	virtual void sortorder_artist_changed(SortOrder);

	virtual void text_line_edited(const QString&);
	virtual void clear_button_pressed();
	virtual void combo_search_changed(int idx);
	virtual void return_pressed();

	virtual void delete_artist();
	virtual void delete_album();
	virtual void delete_tracks();

	virtual void refresh_artist();
	virtual void refresh_album();
	virtual void refresh_tracks();

	virtual void album_rating_changed(int);
	virtual void title_rating_changed(int);

	virtual void append();
	virtual void append_tracks();
	virtual void play_next();
	virtual void play_next_tracks();

	virtual void show_delete_answer(QString);


public slots:
	virtual void id3_tags_changed();

protected:
	template<typename T>
	void setup_parent(T* subclass){

		subclass->setupUi(subclass);

		_combo_libchooser = subclass->combo_lib_chooser;
		_lv_artist = subclass->lv_artist;
		_lv_album = subclass->lv_album;
		_lv_tracks = subclass->tb_title;
		_btn_clear = subclass->btn_clear;
		_le_search = subclass->le_search;
		_combo_search = subclass->combo_searchfilter;


		_combo_search->setItemDelegate(new ComboBoxDelegate(this));
		_combo_search->setIconSize(QSize(16, 16));
		_combo_search->view()->setIconSize(QSize(16, 16));

		init_shortcuts();
		init_finished();

	}
};

#endif // GUI_ABSTRACTLIBRARY_H
