/* GUI_TagEdit.h */

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


#ifndef GUI_TAGEDIT_H_
#define GUI_TAGEDIT_H_

#include <QLineEdit>
#include <QFocusEvent>
#include <QString>
#include <QRegExp>

#include "GUI/TagEdit/ui_GUI_TagEdit.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"
#include "Components/TagEdit/TagExpression.h"



/**
 * @brief The GUI_TagEdit class
 * @ingroup Tagging
 */
class TagEdit;
class CoverLocation;
class MetaDataList;
class MetaData;

class GUI_TagEdit :
		public SayonaraWidget,
		private Ui::GUI_TagEdit
{

	Q_OBJECT

public:
	GUI_TagEdit(QWidget* parent=nullptr);
	virtual ~GUI_TagEdit();

	/**
	 * @brief Get tag edit object
	 * @return Tag edit object
	 */
	TagEdit* get_tag_edit() const;

	/**
	 * @brief Commits changes to db/file
	 */
	void commit();

	/**
	 * @brief calls undo_all, and closes the entire dialog
	 */
	void cancel();

	void show_button_commit(bool b);
	void show_button_cancel(bool b);


signals:
	void sig_ok_clicked(const MetaDataList&);
	void sig_undo_clicked(int idx);
	void sig_undo_all_clicked();
	void sig_cancelled();



private:
	TagEdit*		_tag_edit=nullptr;
	TagExpression	_tag_expression;
	QMap<int, QString> _cover_path_map;

	int				_cur_idx;


	/**
	 * @brief _tag_str_map, key = tag, val = replaced string
	 */
	QMap<Tag, ReplacedString> _tag_str_map;



private:

	bool is_cover_replacement_active() const;
	void update_cover(int idx, const QString& cover_path);
	void set_cover(const MetaData& md);
	void show_replacement_field(bool b);


	/**
	 * @brief replaces text with tag or vice versa
	 * @param t Tag of interest
	 * @param activate if true, replace text by tag, else replace tag by original text
	 * @return true on success, false if nothing is selected
	 */
	bool replace_selected_tag_text(Tag t, bool activate);


	/**
	 * @brief apply the entire tag expression for one specific track
	 * @param idx track index
	 */
	void apply_tag(int idx);


	/**
	 * @brief fills track information for current index (_cur_idx)
	 */
	void track_idx_changed();


	/**
	 * @brief resets the ui, sets the _cur_idx to -1
	 */
	void reset();


	/**
	 * @brief writes changes to the tag edit logic, does not write to db or file, also see ok_button_clicked()
	 * @param idx track index
	 */
	void write_changes(int idx);


	/**
	 * @brief checks, if current index is valid
	 * @param idx index of interest
	 * @return true if index is inside bounds, false else
	 */
	bool check_idx(int idx) const;




private slots:
	/**
	 * @brief calls write_changes and track_idx_changed with new _cur_idx
	 */
	void next_button_clicked();


	/**
	 * @brief calls write_changes and track_idx_changed with new _cur_idx
	 */
	void prev_button_clicked();


	/**
	 * @brief calls apply_tag
	 */
	void apply_tag_clicked();
	void apply_tag_all_clicked();


	/**
	 * @brief (un)sets _album_all
	 */
	void album_all_changed(bool b);


	/**
	 * @brief (un)sets _artist_all
	 */
	void artist_all_changed(bool b);


	/**
	 * @brief (un)sets _genre_all
	 */
	void genre_all_changed(bool b);


	/**
	 * @brief (un)sets _year_all
	 */
	void year_all_changed(bool b);


	/**
	 * @brief (un)sets _discnumber_all
	 */
	void discnumber_all_changed(bool b);


	/**
	 * @brief (un)sets _rating_all
	 */
	void rating_all_changed(bool b);

	/**
	 * @brief (un)sets _cover_all
	 */
	void cover_all_changed(bool b);


	/**
	 * @brief calls replace_selected_tag_text with TAG_TITLE
	 * @param b active or not
	 */
	void btn_title_checked(bool b);


	/**
	 * @brief calls replace_selected_tag_text with TAG_ARTIST
	 * @param b active or not
	 */
	void btn_artist_checked(bool b);


	/**
	 * @brief calls replace_selected_tag_text with TAG_ALBUM
	 * @param b active or not
	 */
	void btn_album_checked(bool b);


	/**
	 * @brief calls replace_selected_tag_text with TAG_TRACK_NUM
	 * @param b active or not
	 */
	void btn_track_nr_checked(bool b);


	/**
	 * @brief calls replace_selected_tag_text with TAG_DISC
	 * @param b active or not
	 */
	void btn_disc_nr_checked(bool b);


	/**
	 * @brief calls replace_selected_tag_text with TAG_YEAR
	 * @param b active or not
	 */
	void btn_year_checked(bool b);


	/**
	 * @brief calls webpage with help
	 */
	void btn_tag_help_clicked();

	/**
	 * @brief tries to apply the tag
	 */
	void tag_text_changed(const QString&);


	/**
	 * @brief sets red if not valid
	 * @param valid if tag is valid or not
	 */
	void set_tag_colors(bool valid);



	/**
	 * @brief Undo on current track
	 */
	void undo_clicked();


	/**
	 * @brief Undo on all tracks
	 */
	void undo_all_clicked();


	/**
	 * @brief Shows progress bar if val > 0
	 * @param val value of progress bar
	 */
	void progress_changed(int val);


	/**
	 * @brief update gui, if metadata was changed
	 */
	void metadata_changed(const MetaDataList&);



	/**
	 * @brief triggered, when player language has been changed
	 */
	void language_changed() override;

	/**
	 * @brief private slot for notifying the MetaDataChangeNotifier
	 */
	void commit_finished();


	void rb_dont_replace_toggled(bool b);

};

#endif
