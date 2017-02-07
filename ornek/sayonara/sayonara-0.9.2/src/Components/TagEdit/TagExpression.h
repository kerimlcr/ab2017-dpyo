/* TagExpression.h */

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



#ifndef TAGEXPRESSION_H
#define TAGEXPRESSION_H

#include <QMap>
#include <QString>
#include <QStringList>


#define TAG_NONE
#define TAG_TITLE QString("<t>")
#define TAG_ALBUM QString("<al>")
#define TAG_ARTIST QString("<ar>")
#define TAG_TRACK_NUM QString("<nr>")
#define TAG_YEAR QString("<y>")
#define TAG_DISC QString("<d>")
#define TAG_IGNORE QString("<ign>")

typedef QString Tag;
typedef QString ReplacedString;

/**
 * @brief The TagExpression class
 * @ingroup Tagging
 */
class TagExpression {

private:

	/**
	 * @brief _cap_map contains the Tag and the text that fits the regular expression
	 */
	QMap<Tag, ReplacedString>	_cap_map;

	/**
	 * @brief _tag_regex_map keys = the tag (e.g. <t>) and the corresponding regular expression
	 */
	QMap<Tag, QString>		_tag_regex_map;
	bool					_valid;

	/**
	 * @brief prepends a "\\" before special characters
	 * @param str string to be modified
	 * @return new string with escaped characters
	 */
	QString escape_special_chars(const QString& str) const;


	/**
	 * @brief calcs the regular expression string
	 * @param splitted_tag_str a stringlist gennerated by split_tag_string
	 * @return the regular expression string
	 */
	QString calc_regex_string(const QStringList& splitted_tag_str) const;

	/**
	 * @brief splits the tag string into normal string and tags e.g. foo<t>bar -> (foo, <t>, bar)
	 * @param tag_str
	 * @return a stringlist containing normal strings and tags
	 */
	QStringList split_tag_string(const QString& tag_str) const;


public:

	TagExpression();
	TagExpression(const QString& tag_str, const QString& filename);
	virtual ~TagExpression();


	/**
	 * @brief fills the _cap_map
	 * @param tag_str the tag string entered in UI
	 * @param filepath the filepath
	 * @return true if regular expressions can be applied to filepath, false else
	 */
	bool update_tag(const QString& tag_str, const QString& filepath);

	/**
	 * @brief checks, if one specific tag can be applied to the string
	 * @param tag e.g. <t>
	 * @param str usually the filepath
	 * @return true on success, false else
	 */
	bool check_tag(const Tag& tag, const QString& str);

	QMap<Tag, ReplacedString> get_tag_val_map() const;
};

#endif // TAGEXPRESSION_H
