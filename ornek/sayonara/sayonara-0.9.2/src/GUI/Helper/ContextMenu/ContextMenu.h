/* ContextMenu.h */

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



#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QAction>
#include <QMenu>
#include <QTimer>

#include "Helper/Settings/SayonaraClass.h"

class IconLoader;

/**
 * @brief Combination of ContextMenu::Entry values
 * @ingroup GUIHelper
 */
typedef int ContextMenuEntries;

/**
 * @brief A context menu with some standard actions
 * @ingroup GUIHelper
 */
class ContextMenu :
		public QMenu,
		protected SayonaraClass
{
	Q_OBJECT

public:

	/**
	 * @brief The Entry enum
	 */
	enum Entry {
		EntryNone	=0,
		EntryNew	=(1<<0),
		EntryUndo	=(1<<1),
		EntrySave	=(1<<2),
		EntrySaveAs	=(1<<3),
		EntryRename	=(1<<4),
		EntryDelete	=(1<<5),
		EntryOpen	=(1<<6),
		EntryDefault=(1<<7)
	};

signals:
	void sig_new();
	void sig_undo();
	void sig_save();
	void sig_save_as();
	void sig_rename();
	void sig_delete();
	void sig_open();
	void sig_default();


private:
	QAction*	_action_new=nullptr;
	QAction*	_action_open=nullptr;
	QAction*	_action_undo=nullptr;
	QAction*	_action_save=nullptr;
	QAction*	_action_save_as=nullptr;
	QAction*	_action_rename=nullptr;
	QAction*	_action_delete=nullptr;
	QAction*	_action_default=nullptr;


	QList<QAction*>		_actions;
	QTimer*				_timer=nullptr;
	IconLoader*			_icon_loader=nullptr;

	/**
	 * @brief show_action
	 * @param b
	 * @param action
	 */
	void show_action(bool b, QAction* action);


public:
	explicit ContextMenu(QWidget *parent=nullptr);

	/**
	 * @brief register a custom action
	 * @param action the action. You have to set up the connection manually
	 */
	void register_action(QAction* action);

	/**
	 * @brief query, if there are visible actions
	 * @return true, if at least one action is visible. false else
	 */
	bool has_actions();

	/**
	 * @brief get all visible entries
	 * @return ContextMenuEntry mask
	 */
	ContextMenuEntries get_entries() const;


protected:
	void showEvent(QShowEvent* e) override;


public slots:
	/**
	 * @brief show actions defined by ContextMenuEntry mask. Hide other actions
	 * @param mask of ContextMenu::Entry
	 */
	void show_actions(ContextMenuEntries entries);

	/**
	 * @brief show/hide specific action
	 * @param entry the entry of interes
	 * @param visible show/hide
	 */
	void show_action(ContextMenu::Entry entry, bool visible);

	/**
	 * @brief show all actions
	 */
	void show_all();


private slots:
	/**
	 * @brief enable actions
	 */
	void timed_out();
	void language_changed();
	void skin_changed();
};



#endif // CONTEXTMENU_H
