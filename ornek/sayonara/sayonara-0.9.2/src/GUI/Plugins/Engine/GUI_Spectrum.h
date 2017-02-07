/* GUI_Spectrum.h */

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



#ifndef GUI_SPECTRUM_H
#define GUI_SPECTRUM_H

#include "GUI/Plugins/Engine/ui_GUI_Spectrum.h"
#include "Components/Engine/Playback/SoundOutReceiver.h"

#include "EnginePlugin.h"

#include <QList>

class GUI_Spectrum :
		public EnginePlugin,
		public SpectrumReceiver,
		private Ui::GUI_Spectrum
{
    Q_OBJECT

	friend class PlayerPluginInterface;

public:
	explicit GUI_Spectrum(QWidget *parent=nullptr);
	virtual QString get_name() const override;
	virtual QString get_display_name() const override;

protected:
	void paintEvent(QPaintEvent* e) override;
	void showEvent(QShowEvent*) override;
	void closeEvent(QCloseEvent*) override;
	void language_changed() override;
	void init_ui() override;


protected slots:
	void timed_out() override;


public slots:
	void set_spectrum(const QList<float>&) override;
	void sl_update_style() override;

private:

	QList<float> _spec;
	int** _steps=nullptr;

    void resize_steps(int bins, int rects);

};

#endif // GUI_SPECTRUM_H
