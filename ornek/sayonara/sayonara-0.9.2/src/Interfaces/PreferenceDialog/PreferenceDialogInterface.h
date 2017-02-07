/* PreferenceDialogInterface.h */

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



#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include "PreferenceInterface.h"

#include <QList>

/**
 * @brief Abstract Interface you should use when creating a preference dialog.
 *
 * If you wish to subclass, reimplement void get_action_name() and void init_ui().\n
 * In every function that makes use of the widgets call bool is_ui_initialized() first.\n
 * Call setup_parent(this) in init_ui() first.\n
 * If you wish to reimplement void language_changed(), call PreferenceDialogInterface::language_changed at the end.
 * @ingroup Interfaces
 * @ingroup Preferences
 */
class PreferenceDialogInterface : public PreferenceInterface<SayonaraDialog>
{
	Q_OBJECT

	friend class PreferenceInterface<SayonaraDialog>;


public:
	/**
	 * @brief Standard constructor
	 * @param parent
	 */
	PreferenceDialogInterface(QWidget* parent=nullptr);
	virtual ~PreferenceDialogInterface();
};


typedef QList<PreferenceDialogInterface*> PreferenceWidgetList;

#endif // PREFERENCEDIALOG_H
