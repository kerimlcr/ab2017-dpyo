/* GUI_Equalizer.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * GUI_Equalizer.h
 *
 *  Created on: May 18, 2011
 *      Author: Lucio Carreras
 */

#ifndef GUI_EQUALIZER_H_
#define GUI_EQUALIZER_H_

#include "GUI/Plugins/Equalizer/ui_GUI_Equalizer.h"
#include "Interfaces/PlayerPlugin/PlayerPlugin.h"

#include <QList>
#include <QMenu>
#include <QAction>

class EngineHandler;
/**
 * @brief The GUI_Equalizer class
 * @ingroup Equalizer
 */
class GUI_Equalizer :
		public PlayerPluginInterface,
		private Ui::GUI_Equalizer
{

	Q_OBJECT

	friend class PlayerPluginInterface;

public:
	GUI_Equalizer(QWidget* parent=nullptr);
	virtual ~GUI_Equalizer();

	QString get_name() const override;
	QString get_display_name() const override;


private slots:
	void sli_changed(int idx, int value);
	void sli_pressed();
	void sli_released();

	void preset_changed(int);
	void cb_gauss_toggled(bool);

	void btn_default_clicked();
	void btn_save_clicked();
	void btn_delete_clicked();
	void btn_undo_clicked();

	void text_changed(const QString&);

public slots:
	void fill_eq_presets();


protected:
	void language_changed() override;
	void init_ui() override;

private:

	EngineHandler*		_engine=nullptr;
	QList<EQ_Setting>	_presets;
	QList<EqSlider*>	_sliders;

	int					_old_val[10];
	int					_active_idx;

	int find_combo_text(QString txt);

};

#endif /* GUI_EQUALIZER_H_ */
