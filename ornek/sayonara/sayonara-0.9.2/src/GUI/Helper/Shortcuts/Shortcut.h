/* Shortcut.h */

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



#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <QString>
#include <QWidget>
#include <QStringList>
#include <QShortcut>
#include <QKeySequence>
#include "Helper/Settings/SayonaraClass.h"

class ShortcutWidget;
/**
 * @brief A single shortcut managed by ShortcutHandler.
 * This class holds information about the default shortcuts,
 * the user defined shortcuts, a name attached to each shortcut
 * an identifier which is written into the database and a list
 * of the corresponding shortcuts in the Qt format
 * @ingroup Shortcuts
 */
class Shortcut : private SayonaraClass
{

private:
	QStringList			_default_shortcuts;
	QStringList			_shortcuts;
	QString				_name;
	QString				_identifier;
	QList<QShortcut*>	_qt_shortcuts;
	ShortcutWidget* _parent=nullptr;

private:
	Shortcut();

	/**
	 * @brief Converts the sequences from get_sequences() to a list of qt specific
	 * shortcuts and writes them into the _qt_shortcuts field
	 * @param parent the widget the shortcut is mapped to
	 * @return a list of shortcuts in the Qt format
	 */
	QList<QShortcut*> init_qt_shortcut(QWidget* parent);


public:
	/**
	 * @brief Shortcut
	 * @param identifier an unique identifier used to write the shortcut into the database
	 * @param name the name displayed in the Shortcut configuration dialog
	 * @param default_shortcut one default shortcut
	 */
	Shortcut(ShortcutWidget* parent, const QString& identifier, const QString& name, const QString& default_shortcut);

	/**
	 * @brief Shortcut
	 * @param identifier an unique identifier used to write the shortcut into the database
	 * @param name the name displayed in the Shortcut configuration dialog
	 * @param default_shortcuts a list of default shortcuts
	 */
	Shortcut(ShortcutWidget* parent, const QString& identifier, const QString& name, const QStringList& default_shortcuts);

	/**
	 * @brief Copy constructor
	 * @param other
	 */
	Shortcut(const Shortcut& other);

	/**
	 * @brief get a raw and invalid shortcut. This function is used instead of the default constructor
	 * @return an uninitialized shortcut
	 */
	static Shortcut getInvalid();

	/**
	 * @brief
	 * @param shortcuts map new user-readable key sequences to this shortcut
	 */
	void					change_shortcut(const QStringList& shortcuts);

	/**
	 * @brief get the human-readable name of the shortcut
	 * @return
	 */
	QString					get_name() const;

	/**
	 * @brief get a human-readable list of mapped default shortcuts
	 * @return
	 */
	QStringList				get_default() const;

	/**
	 * @brief get a list key squences mapped to this shortcut
	 * @return
	 */
	QList<QKeySequence>		get_sequences() const;

	/**
	 * @brief get a human-readable list of mapped shortcuts
	 * @return
	 */
	QStringList				get_shortcuts() const;

	/**
	 * @brief get the unique identifier
	 * @return
	 */
	QString					get_identifier() const;

	/**
	 * @brief Check if the shortcut is valid or if it was retrieved via getInvalid()
	 * @return
	 */
	bool					is_valid() const;

	template<typename T>
	/**
	 * @brief create a qt shortcut for a widget
	 * @param parent the widget the shortcut is attached to
	 * @param func a lambda function which will be triggered when shortcut is pressed
	 */
	void create_qt_shortcut(QWidget* parent, T func){

		QList<QShortcut*> shortcuts = init_qt_shortcut(parent);
		for(QShortcut* sc : shortcuts){
			parent->connect(sc, &QShortcut::activated, func);
		}
	}

	/**
	 * @brief create a qt shortcut for a widget
	 * @param parent the widget the shortcut is attached to
	 * @param the receiver object of the shortcut
	 * @param the slot which is triggered when pressing that shortcut
	 */
	void create_qt_shortcut(QWidget* parent, QObject* receiver, const char* slot);
};

#endif // SHORTCUT_H
