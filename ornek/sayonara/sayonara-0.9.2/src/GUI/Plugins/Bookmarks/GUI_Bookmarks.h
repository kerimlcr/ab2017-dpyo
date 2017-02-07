/* GUI_Bookmarks.h */

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



#ifndef GUI_BOOKMARKS_H
#define GUI_BOOKMARKS_H


#include "GUI/Plugins/Bookmarks/ui_GUI_Bookmarks.h"
#include "Interfaces/PlayerPlugin/PlayerPlugin.h"

class Bookmarks;
class Bookmark;
/**
 * @brief The GUI_Bookmarks class
 * @ingroup Bookmarks
 */
class GUI_Bookmarks :
		public PlayerPluginInterface,
		protected Ui::GUI_Bookmarks
{
	Q_OBJECT

	friend class PlayerPluginInterface;

public:
	explicit GUI_Bookmarks(QWidget *parent=nullptr);

	QString get_name() const override;
	QString get_display_name() const override;
	

private slots:
	void combo_changed(int new_idx);
	void next_clicked();
	void prev_clicked();
	void new_clicked();
	void del_clicked();
	void del_all_clicked();
	void loop_clicked(bool);


	void prev_changed(const Bookmark& bookmark);
	void next_changed(const Bookmark& bookmark);

	void disable_prev();
	void disable_next();

	void bookmarks_changed(const QList<Bookmark>& bookmarks);


protected:
	void language_changed() override;
	void init_ui() override;


private:
	Bookmarks*	_bookmarks=nullptr;
};

#endif // GUI_BOOKMARKS_H
