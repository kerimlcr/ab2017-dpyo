/* Bookmarks.h */

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



#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include "Bookmark.h"

#include "Components/PlayManager/PlayManager.h"

#include <QPair>
#include <QList>

class DatabaseConnector;
/**
 * @brief The Bookmarks logic class
 * @ingroup Bookmarks
 */
class Bookmarks : public QObject
{
	Q_OBJECT

signals:
	/**
	 * @brief emitted when bookmarks have changed
	 * @param bookmarks new bookmarks
	 */
	void sig_bookmarks_changed(const QList<Bookmark>& bookmarks);

	/**
	 * @brief previous bookmark has changed
	 * @param bm new bookmark. Check for Bookmark::is_valid()
	 */
	void sig_prev_changed(const Bookmark& bm);

	/**
	 * @brief next bookmark has changed
	 * @param bm new bookmark. Check for Bookmark::is_valid()
	 */
	void sig_next_changed(const Bookmark& bm);

public:
	Bookmarks(QObject *parent=nullptr);
	virtual ~Bookmarks();

	/**
	 * @brief fetch all bookmarks for current track
	 * @return all bookmarks for current track
	 */
	QList<Bookmark> get_all_bookmarks() const;


	/**
	 * @brief create a new bookmark for current track and current position
	 * @return true if successful, else false
	 */
	bool save();

	/**
	 * @brief Jump to specific bookmark
	 * @param idx bookmark index
	 * @return true if index was valid
	 */
	bool jump_to(int idx);

	/**
	 * @brief Jump to next bookmark
	 * @return true if successful, false else
	 */
	bool jump_next();

	/**
	 * @brief Jump to previous bookmark
	 * @return true if successful, false else
	 */
	bool jump_prev();

	/**
	 * @brief remove single bookmark from database for current track
	 * @param idx index
	 * @return
	 */
	bool remove(int idx);


	/**
	 * @brief Remove all bookmarks from database for current track
	 * @return true if successful, false if not
	 */
	bool remove_all();

	/**
	 * @brief tries to set the loop between the current two indices
	 * @param b switch loop on or off
	 * @return false if the two current indices are invalid or if b == false. True else
	 */
	bool set_loop(bool b);

	/**
	 * @brief get the current played track
	 * @return return current played track
	 */
	MetaData get_cur_track() const;


	int get_size() const;



private slots:
	/**
	 * @brief track position has changed
	 * @param pos new position in ms
	 */
	void pos_changed_ms(quint64 pos);

	/**
	 * @brief current track has changed
	 * @param md new MetaData object
	 */
	void track_changed(const MetaData& md);

	/**
	 * @brief current playstate has changed
	 * @param state new playstate
	 */
	void playstate_changed(PlayManager::PlayState state);


private:
	DatabaseConnector*		_db=nullptr;
	PlayManager*			_play_manager=nullptr;

	QList<Bookmark>			_bookmarks;
	MetaData				_md;

	quint32					_cur_time;
	int						_prev_idx;
	int						_next_idx;

	quint32					_loop_start;
	quint32					_loop_end;


private:

	/**
	 * @brief reset member variables
	 */
	void init_members();

	/**
	 * @brief fetch bookmarks from db and emit sig_bookmarks_changed signal
	 */
	void reload_bookmarks();

	/**
	 * @brief sort bookmarks by time
	 */
	void sort_bookmarks();
};

#endif // BOOKMARKS_H
