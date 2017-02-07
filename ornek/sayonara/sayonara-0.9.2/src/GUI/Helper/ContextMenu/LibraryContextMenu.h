/* LibraryContextMenu.h */

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



#ifndef LIBRARYCONTEXTMENU_H
#define LIBRARYCONTEXTMENU_H

#include <QMenu>
#include <QAction>
#include <QEvent>
#include "Helper/Settings/SayonaraClass.h"

typedef int LibraryContexMenuEntries;

/**
 * @brief Context menu used for Library and playlist windows
 * @ingroup GUIHelper
 */
class LibraryContextMenu :
		public QMenu,
		protected SayonaraClass
{
    Q_OBJECT


public:
	explicit LibraryContextMenu(QWidget *parent=nullptr);
	virtual ~LibraryContextMenu();


	/**
	 * @brief This enum indicates which entries should be visible
	 */
	enum Entry {
		EntryNone=0,
		EntryInfo=(1<<0),
		EntryEdit=(1<<1),
		EntryLyrics=(1<<2),
		EntryRemove=(1<<3),
		EntryDelete=(1<<4),
		EntryPlayNext=(1<<5),
		EntryAppend=(1<<6),
		EntryRefresh=(1<<7),
		EntryClear=(1<<8),
		EntryRating=(1<<9),
		EntryLast=(1<<10)
	};

	/**
	 * @brief get all visible entries
	 * @return all visible entries
	 */
	virtual LibraryContexMenuEntries get_entries() const;

	/**
	 * @brief show a specific amount of Entries
	 * @param entries bitwise combination of Entry
	 */
	virtual void show_actions(LibraryContexMenuEntries entries);

	/**
	 * @brief show/hide a specific Entry
	 * @param The entry of interest
	 * @param visible
	 */
	virtual void show_action(Entry entry, bool visible);

	/**
	 * @brief show all possible entries
	 */
	virtual void show_all();

	/**
	 * @brief set rating for the rating entry
	 * @param rating from 0 to 5
	 */
	void set_rating(int rating);

    
signals:
    void sig_info_clicked();
    void sig_edit_clicked();
	void sig_lyrics_clicked();
    void sig_remove_clicked();
    void sig_delete_clicked();
	void sig_play_next_clicked();
    void sig_append_clicked();
	void sig_refresh_clicked();
	void sig_clear_clicked();
	void sig_rating_changed(int rating);


private:
	QAction* 			_info_action=nullptr;
	QAction* 			_lyrics_action=nullptr;
	QAction* 			_edit_action=nullptr;
	QAction* 			_remove_action=nullptr;
	QAction* 			_delete_action=nullptr;
	QAction*            _play_next_action=nullptr;
	QAction*            _append_action=nullptr;
	QAction*            _refresh_action=nullptr;
	QAction*			_clear_action=nullptr;

	QAction*			_rating_action=nullptr;
	QMenu*				_rating_menu=nullptr;


protected:
	void changeEvent(QEvent* e) override;
	QAction* init_rating_action(int rating);

	QString rating_text();

private slots:
	void skin_changed();
};


#endif // LIBRARYCONTEXTMENU_H
