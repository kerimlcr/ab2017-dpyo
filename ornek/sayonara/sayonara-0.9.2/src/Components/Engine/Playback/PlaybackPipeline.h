/* PlaybackPipeline.h */

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



#ifndef GSTPLAYBACKPIPELINE_H_
#define GSTPLAYBACKPIPELINE_H_

#include "AbstractPipeline.h"
#include "ChangeablePipeline.h"
#include "CrossFader.h"


#include <gst/app/gstappsink.h>
#include <QTimer>

struct StreamRecorderData;
class Engine;

class PlaybackPipeline :
		public AbstractPipeline,
		public CrossFader,
		public ChangeablePipeline
{
	Q_OBJECT


public:

	PlaybackPipeline(Engine* engine, QObject *parent=nullptr);
	virtual ~PlaybackPipeline();

	bool init(GstState state=GST_STATE_NULL) override;
	bool set_uri(gchar* uri) override;

	void set_n_sound_receiver(int num_sound_receiver);

	void set_current_volume(double volume) override;
	double get_current_volume() const override;

	GstElement* get_source() const override;
	GstElement* get_pipeline() const override;


public slots:

	void play() override;
	void pause() override;
	void stop() override;


	void set_eq_band(const QString& band_name, double val);
	void set_speed(float speed, double pitch, bool preserve_pitch);
	void set_streamrecorder_path(const QString& session_path);
	void change_pitch(int a_frequency);

	gint64 seek_rel(double percent, gint64 ref_ns);
	gint64 seek_abs(gint64 ns );


private:

	int					_vol;
	bool				_speed_active;
	float				_speed_val;
	QString				_sr_path;
	StreamRecorderData* _sr_data=nullptr;


	GstElement*			_audio_src=nullptr;
	GstElement*			_audio_convert=nullptr;
	GstElement*			_tee=nullptr;

	GstElement*			_eq_queue=nullptr;
	GstElement*			_equalizer=nullptr;
	GstElement*			_speed=nullptr;
	GstElement*			_volume=nullptr;
	GstElement*			_pitch=nullptr;

	GstElement*			_audio_sink=nullptr;

	GstElement*			_spectrum_queue=nullptr;
	GstElement*			_spectrum=nullptr;
	GstElement*			_spectrum_sink=nullptr;

	GstElement*			_level_queue=nullptr;
	GstElement*			_level=nullptr;
	GstElement*			_level_sink=nullptr;

	GstElement*			_lame_queue=nullptr;
	GstElement*			_lame_converter=nullptr;
	GstElement*			_lame_resampler=nullptr;
	GstElement*			_lame=nullptr;
	GstElement*			_lame_app_sink=nullptr;

	GstElement*			_file_queue=nullptr;
	GstElement*			_file_converter=nullptr;
	GstElement*			_file_sink=nullptr;
	GstElement*			_file_resampler=nullptr;
	GstElement*			_file_lame=nullptr;

	gulong				_level_probe, _spectrum_probe, _lame_probe, _file_probe;
	bool				_show_level, _show_spectrum, _run_broadcast, _run_sr;

    bool _seek(gint64 ns);


	bool create_elements() override;
	bool add_and_link_elements() override;
	bool configure_elements() override;
	quint64 get_about_to_finish_time() const override;

	void init_equalizer();


protected slots:

	void _sl_vol_changed();
	void _sl_show_level_changed();
	void _sl_show_spectrum_changed();
	void _sl_mute_changed();

	void _sl_speed_active_changed();
	void _sl_speed_changed();
};


#endif
