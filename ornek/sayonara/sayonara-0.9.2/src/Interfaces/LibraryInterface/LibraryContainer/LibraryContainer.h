/* LibraryContainer.h */

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



#ifndef LIBRARYCONTAINER_H
#define LIBRARYCONTAINER_H

#include <QIcon>
#include <QWidget>
#include <QComboBox>

#include <QList>
#include <QAction>

class Settings;
class LibraryPluginHandler;

/**
 * @brief An interface class needed when implementing a library plugin
 * @ingroup LibraryPlugins
 * @ingroup Interfaces
 */

class LibraryContainerInterface :
	public QObject
{
	Q_OBJECT

	friend class LibraryPluginHandler;

private:
	bool		_initialized;
	QAction*	_action=nullptr;
	Settings*	_settings=nullptr;

	/**
	 * @brief Called by LibraryPluginHandler
	 */
	void set_initialized();

private slots:
	void language_changed();

public:

	explicit LibraryContainerInterface(QObject* parent=nullptr);
	virtual ~LibraryContainerInterface();

	/**
	 * @brief Should return the icon displayed in the library view combobox
	 * @return QIcon instance
	 */
	virtual QIcon				get_icon() const=0;


	/**
	 * @brief Should return an untranslated name used for identifying this widget
	 * @return name
	 */
	virtual QString				get_name() const=0;

	/**
	 * @brief Should return the translated name displayed in the library view combobox
	 * @return display name
	 */
	virtual QString				get_display_name() const=0;

	/**
	 * @brief Should return the UI for the library view
	 * @return pointer to the ui
	 */
	virtual QWidget*			get_ui() const=0;

	/**
	 * @brief Should return the library chooser combo box
	 * @return pointer to the combobox
	 */
	virtual QComboBox*			get_libchooser()=0;


	/**
	 * @brief return actions menu (may be nullptr). The title does not have to be set
	 * @return the translated menu relevant for the corresponding library
	 */
	virtual QMenu*				get_menu();


	/**
	 * @brief sets the action member field used in the player menu bar. This is
	 * called by the player if the language has changed
	 * @param action the new translated action
	 */
	void						set_menu_action(QAction* action);

	/**
	 * @brief get the action in the player menubar.
	 * @return pointer to action field
	 */
	QAction*					get_menu_action() const;

	
	/**
	 * @brief Should initialize the ui. The ui constructor should be called within this function
	 */
	virtual void				init_ui()=0;


	/**
	 * @brief tests, if the ui already has been initialized
	 * @return true if ui has been initialized, false else
	 */
	bool						is_initialized() const;

};

Q_DECLARE_INTERFACE(LibraryContainerInterface, "com.sayonara-player.library")

#endif // LIBRARYCONTAINER_H
