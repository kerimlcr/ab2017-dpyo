/* LibraryPluginLoader.h */

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



#ifndef LIBRARYPLUGINLOADER_H
#define LIBRARYPLUGINLOADER_H

#include <QDir>
#include <QList>
#include <QPluginLoader>
#include <QWidget>

#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"

class LibraryContainerInterface;
/**
 * @brief Library Plugin Manager
 * @ingroup LibraryPlugins
 */
class LibraryPluginHandler :
		public QObject,
		protected SayonaraClass
{

	Q_OBJECT

signals:
	void sig_idx_changed(int);

private:
	QList<LibraryContainerInterface*>	_libraries;
	QList<QPair<QString, QIcon>>		_library_entries;

	int									_cur_idx;
	QWidget*							_library_parent=nullptr;

	/**
	 * @brief Init a library. This is used at startup for the current library
	 * or when the index has changed
	 * @param idx
	 */
	void init_library(int idx);


public:
	LibraryPluginHandler(QObject* parent=nullptr);
	~LibraryPluginHandler();


	/**
	 * @brief Search for plugins and add some predefined plugins
	 * @param containers Some predefined plugins
	 */
	void init(const QList<LibraryContainerInterface*>& containers);

	/**
	 * @brief Set the parent widget for library plugins
	 * @param parent Parent widget
	 */
	void set_library_parent(QWidget* parent);


	/**
	 * @brief Get a list for all found plugins. The ui is not necessarily initialized
	 * @return list for all found library plugins
	 */
	QList<LibraryContainerInterface*> get_libraries() const;

	/**
	 * @brief Get the current selected (and shown) library
	 * @return current library, on failure nullptr
	 */
	LibraryContainerInterface* get_cur_library() const;

	/**
	 * @brief Get the current index of the library (indicated by current index
	 * in dropdown
	 * @return current library index
	 */
	int get_cur_library_idx() const;


private slots:
	/**
	 * @brief triggered, when the index on of the comboboxes has changed
	 * @param idx new library index
	 */
	void index_changed(int idx);

	/**
	 * @brief language has changed. Translate all comboboxes
	 */
	void language_changed();
};

#endif // LIBRARYPLUGINLOADER_H
