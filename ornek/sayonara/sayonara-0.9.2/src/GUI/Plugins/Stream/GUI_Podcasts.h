/* GUI_Podcasts.h */

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



#ifndef GUI_PODCASTS_H
#define GUI_PODCASTS_H


#include "GUI/Plugins/Stream/ui_GUI_Podcasts.h"
#include "Components/StreamPlugins/Streams/StreamHandlerPodcasts.h"
#include "GUI_AbstractStream.h"

#include <QMap>

class GUI_Podcasts :
		public GUI_AbstractStream,
		public Ui::GUI_Podcasts
{
    Q_OBJECT

protected:
	void language_changed() override;
	QString get_title_fallback_name() const override;

public:
	GUI_Podcasts(QWidget *parent=nullptr);
    virtual ~GUI_Podcasts();


	QString get_name() const override;
	QString get_display_name() const override;


};

#endif // GUI_PODCASTS_H
