/* Equalizer_presets.h */

/* Copyright (C) 2011  Lucio Carreras
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


#ifndef _EQUALIZER_PRESETS_
#define _EQUALIZER_PRESETS_


#include <QList>
#include <QString>

/**
 * @brief The EQ_Setting class. Container for Equalizer configurations
 * @ingroup Equalizer
 * @ingroup GUIHelper
 */
class EQ_Setting
{

private:
	QList<int>		_values;
	QString			_name;

public:
	EQ_Setting(const QString& name=QString());
	EQ_Setting(const EQ_Setting& s);
	virtual ~EQ_Setting();
	
	/**
	 * @brief Compares the case insensitive string representation of two settings
	 * @param s other preset
	 * @return
	 */
	bool operator==(const EQ_Setting& s) const;

	/**
	 * @brief get name of setting
	 * @return
	 */
	QString name() const;

	/**
	 * @brief set name of setting
	 * @param name
	 */
	void set_name(const QString& name);

	/**
	 * @brief get database values for setting
	 * @return
	 */
	QList<int> values() const;

	/**
	 * @brief get specific value for a band idx. if idx is not valid, 0 is returned
	 * @param idx band index
	 * @return database value if idx is valid, 0 else
	 */
	int value(int idx) const;

	/**
	 * @brief set specific value for band
	 * @param idx band index
	 * @param val database formatted value
	 */
	void set_value(int idx, int val);

	/**
	 * @brief set all values for a specific index.
	 * If there are more than 10 values, list is stripped.
	 * If there are less, the list is filled with zeros
	 * @param values
	 */
	void set_values(const QList<int> values);

	/**
	 * @brief append a value.
	 * If there are already more than 10 values, nothing happens
	 * @param val
	 */
	void append_value(int val);

	/**
	 * @brief checks, if preset is default preset
	 * @return true if preset is default preset, false else
	 */
	bool is_default() const;

	/**
	 * @brief checks, if the preset name belongs to a default preset
	 * @return true if preset is default preset, false else
	 */
	bool is_default_name() const;

	/**
	 * @brief get default settings
	 * @return list of default settings
	 */
	static QList<EQ_Setting> get_defaults();

	/**
	 * @brief get default values for a specific preset.
	 * If the preset does not have default values, an empty list is returned
	 * @param name preset name
	 * @return value list if name belongs to a default preset. Empty list else
	 */
	static QList<int> get_default_values(const QString& name);

	/**
	 * @brief static convenience function for is_default_name()
	 * @param name preset name
	 * @return
	 */
	static bool is_default_name(const QString& name);

	/**
	 * @brief converts a string to a EQ_Setting.
	 * If not possible a default constructed EQ_Setting is returned
	 * @param str
	 * @return
	 */
	static EQ_Setting fromString(const QString& str);

	/**
	 * @brief converts EQ_Setting to string
	 * @return
	 */
	QString toString() const;
};

#endif
