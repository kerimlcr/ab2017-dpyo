/* TagLineEdit.h */

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



#ifndef TAGLINEEDIT_H
#define TAGLINEEDIT_H

#include <QLineEdit>
#include <QString>
#include <QFocusEvent>
/**
 * @brief Holds start and size of a selection
 * @ingroup Tagging
 */
struct TextSelection{

	int selection_start;
	int selection_size;

	/**
	 * @brief set selection_start to -1 and selection_size to 0
	 */
	void reset(){
		selection_start = -1;
		selection_size = 0;
	}

	TextSelection(){
		reset();
	}
};

/**
 * @brief The TagLineEdit class
 * @ingroup Tagging
 */
class TagLineEdit : public QLineEdit {

	Q_OBJECT

public:
	TagLineEdit(QWidget* parent=nullptr);

protected:

	TextSelection 	_text_selection;

protected:
	/**
	 * @brief resets the TextSelection
	 * @param e
	 */
	void focusInEvent(QFocusEvent* e) override;

	/**
	 * @brief if selectected_text is empty, TextSelection is resetted. Else the new TextSelection is set.
	 * @param e
	 */
	void focusOutEvent(QFocusEvent* e) override;

public:
	/**
	 * @brief Retrieve the current TextSelection
	 * @return The current TextSelection object
	 */
	TextSelection get_text_selection() const;
};


#endif // TAGLINEEDIT_H
