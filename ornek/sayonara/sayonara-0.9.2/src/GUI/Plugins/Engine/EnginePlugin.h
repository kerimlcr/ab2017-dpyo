/* EnginePlugin.h */

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



#ifndef ENGINEPLUGIN_H
#define ENGINEPLUGIN_H

#include "GUI_StyleSettings.h"

#include "Interfaces/PlayerPlugin/PlayerPlugin.h"

#include "EngineColorStyleChooser.h"

#include <QAction>
#include <QColor>
#include <QTimer>
#include <QPushButton>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QShowEvent>
	
class EngineHandler;
class GUI_PlayerPlugin;
class EnginePlugin : public PlayerPluginInterface {

	Q_OBJECT

protected:

	QPushButton* _btn_config=nullptr;
	QPushButton* _btn_prev=nullptr;
	QPushButton* _btn_next=nullptr;
	QPushButton* _btn_close=nullptr;

	EngineHandler*				_engine=nullptr;
	GUI_StyleSettings*			_ui_style_settings=nullptr;
	EngineColorStyleChooser*    _ecsc=nullptr;
    ColorStyle                  _cur_style;
    int                         _cur_style_idx;

	QTimer*     _timer=nullptr;
    int         _timer_stopped;

	void init_buttons(bool small);

	//virtual void showEvent(QShowEvent* e) override;
	virtual void closeEvent(QCloseEvent* e) override;
	virtual void resizeEvent(QResizeEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void enterEvent(QEvent* e) override;
	virtual void leaveEvent(QEvent* e) override;


protected slots:

    virtual void config_clicked();
    virtual void next_clicked();
    virtual void prev_clicked();

	virtual void timed_out()=0;

	virtual void played() override;
	virtual void paused() override;
	virtual void stopped() override;


public slots:
	virtual void sl_update_style()=0;
	virtual void update();
	virtual void init_ui() override;


public:
	EnginePlugin(QWidget* parent=nullptr);
    virtual ~EnginePlugin();

	virtual bool is_title_shown() const override;

};


#endif // ENGINEPLUGIN_H


