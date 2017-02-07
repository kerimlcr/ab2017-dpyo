/* ConvertEngine.h */

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



#ifndef ConvertEngine_H
#define ConvertEngine_H

#include <QMap>
#include <gst/gst.h>
#include "Components/Engine/AbstractEngine.h"

class ConvertPipeline;
class ConvertEngine : public Engine
{
	Q_OBJECT

public:
	explicit ConvertEngine(QObject *parent=nullptr);
	
	void set_track_finished(GstElement* src) override;
	bool init() override;

protected slots:
	void set_cur_position_ms(qint64 ms) override;


public slots:
	void play() override;
	void stop() override;
	void pause() override;

	void jump_abs_ms(quint64 pos_ms) override;
	void jump_rel_ms(quint64 ms) override;
	void jump_rel(double percent) override;

	void change_track(const MetaData&) override;
	void change_track(const QString&) override;

	void set_volume(int vol);


protected:
	ConvertPipeline*		_pipeline=nullptr;
	MetaData				_md_target;
	gchar*					_target_uri=nullptr;

	// methods
	bool set_uri(const MetaData& md);
};


#endif // ConvertEngine_H


