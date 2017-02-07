/* GUI_PreferenceDialog.h */

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



#ifndef GUI_PreferenceDialog_H
#define GUI_PreferenceDialog_H

#include <QDialog>
#include <QList>
#include "GUI/Preferences/ui_GUI_PreferenceDialog.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"
#include "Interfaces/PreferenceDialog/PreferenceDialogInterface.h"

#include <QSpacerItem>

class PreferenceWidgetInterface;

/**
 * @brief The Preference Dialog. Register new Preference dialogs with the register_preference_dialog() method.
 * @ingroup Preferences
 */
class GUI_PreferenceDialog :
		public PreferenceDialogInterface,
		private Ui::GUI_PreferenceDialog
{

	friend class PreferenceDialogInterface;
	friend class PreferenceInterface<SayonaraDialog>;

	Q_OBJECT

public:
	explicit GUI_PreferenceDialog(QWidget *parent = 0);
	~GUI_PreferenceDialog();

	QString get_action_name() const override;
	void init_ui() override;

	void register_preference_dialog(PreferenceWidgetInterface* dialog);


protected slots:
	void language_changed() override;

	void commit_and_close();
	void commit() override;
	void revert() override;

	void row_changed(int row);

protected:
	void hide_all();

private:
	QList<PreferenceWidgetInterface*> _dialogs;
};

#endif // GUI_PreferenceDialog_H
