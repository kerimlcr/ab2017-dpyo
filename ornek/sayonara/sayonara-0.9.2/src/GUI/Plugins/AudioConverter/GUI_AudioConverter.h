/* GUI_AudioConverter.h */

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



#ifndef GUI_AUDIOCONVERTER_H
#define GUI_AUDIOCONVERTER_H

#include "Helper/globals.h"
#include "GUI/Plugins/AudioConverter/ui_GUI_AudioConvert.h"
#include "Interfaces/PlayerPlugin/PlayerPlugin.h"

#include "Helper/Playlist/PlaylistMode.h"

#include <QAction>
#include <QCloseEvent>

class EngineHandler;
class GUI_AudioConverter :
		public PlayerPluginInterface,
		private Ui::GUI_AudioConvert
{
	Q_OBJECT

	friend class PlayerPluginInterface;

public:
	explicit GUI_AudioConverter(QWidget *parent=nullptr);

	QString	get_name() const override;
	QString	get_display_name() const override;
	

private slots:
	void rb_cbr_toggled(bool b);
	void rb_vbr_toggled(bool b);
	void quality_changed(int index);
	void cb_active_toggled(bool b);
	void mp3_enc_found();

protected:

	void language_changed() override;
	virtual void init_ui() override;


protected slots:
	void stopped() override;

	
private:

	EngineHandler*		_engine=nullptr;

	PlaylistMode		_pl_mode;
	LameBitrate			_bitrate;
	bool				_mp3_enc_available;

	void fill_cbr();
	void fill_vbr();

	void pl_mode_backup();
	void pl_mode_restore();
};

#endif // GUI_AUDIOCONVERTER_H
