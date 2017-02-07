/* EngineHandler.h */

/* Copyright (C) 2014  Lucio Carreras
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

#ifndef ENGINEHANDLER_H_
#define ENGINEHANDLER_H_

#include "AbstractEngine.h"
#include "Components/PlayManager/PlayManager.h"

#include <QList>

#define EngineHandler_change_track_md static_cast<void (EngineHandler::*) (const MetaData& md)>(&EngineHandler::change_track)


class RawSoundReceiverInterface;
class PlaybackEngine;
class ConvertEngine;

class EngineHandler : public Engine
{
	Q_OBJECT
	SINGLETON_QOBJECT(EngineHandler)

public:

	void fill_engines(const QList<Engine*>& engines);
	virtual bool init() override;

	PlaybackEngine* get_playback_engine();
	ConvertEngine* get_convert_engine();

	void register_raw_sound_receiver(RawSoundReceiverInterface* receiver);
	void unregister_raw_sound_receiver(RawSoundReceiverInterface* receiver);

	void start_convert();
	void end_convert();

	void set_equalizer(int band, int value);


public slots:
	void jump_abs_ms(quint64 ms) override;
	void jump_rel_ms(quint64 ms) override;
	void jump_rel(double where) override;

	void change_track(const MetaData&) override;
	void change_track(const QString&) override;


private slots:

	void sl_md_changed(const MetaData&);
	void sl_dur_changed(const MetaData&);
	void sl_pos_changed_ms(quint64 ms);
	void sl_pos_changed_s(quint32 s);

	void sl_track_finished();

	void sl_track_ready_changed();
	void sl_buffer_state_changed(int progress);

	void sr_record_button_pressed(bool);
	void playstate_changed(PlayManager::PlayState);

	void new_data(const uchar* data, quint64 n_bytes);

	void play() override;
	void stop() override;
	void pause() override;


private:

	PlayManager*						_play_manager=nullptr;
	Engine*								_cur_engine=nullptr;
	QList<Engine*>						_engines;
	QList<RawSoundReceiverInterface*>	_raw_sound_receiver;

	Engine* get_engine(EngineName name);
	void switch_engine(EngineName name);

	bool configure_connections(Engine* old_engine, Engine* new_engine);

};

#endif


