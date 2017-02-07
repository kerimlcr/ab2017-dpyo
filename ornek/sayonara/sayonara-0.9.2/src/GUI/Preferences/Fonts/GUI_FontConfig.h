
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




#ifndef GUI_FONTCONFIG_H
#define GUI_FONTCONFIG_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"
#include "GUI/Preferences/ui_GUI_FontConfig.h"

#include <QFontDatabase>
#include <QFont>
#include <QList>


class GUI_FontConfig :
		public PreferenceWidgetInterface,
		protected Ui::GUI_FontConfig
{
	Q_OBJECT

	friend class PreferenceInterface<SayonaraWidget>;

public:
	GUI_FontConfig(QWidget* parent=nullptr);
	virtual ~GUI_FontConfig();

	QString get_action_name() const override;

	void commit() override;
	void revert() override;

protected:
	void init_ui() override;

protected slots:

	void default_clicked();
	void combo_fonts_changed(const QFont& font);

	void language_changed() override;
	void skin_changed() override;


private:
	QFontDatabase*	_font_db=nullptr;
	int				_cur_font_size;
	int				_cur_font_weight;
	bool			_is_default;

private:
	QStringList get_available_font_sizes(const QString& font_name, const QString& style=QString());
	QStringList get_available_font_sizes(const QFont& font);

	void fill_sizes(const QStringList& sizes);
};

#endif // FONTCONFIG_H
