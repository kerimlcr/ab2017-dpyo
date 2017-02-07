/* PlayManager.h */

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

#ifndef PLAY_MANAGER_H
#define PLAY_MANAGER_H

#include "Helper/globals.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/Settings/SayonaraClass.h"

#include <algorithm>

template<typename T, int N_ITEMS>
class RingBuffer {

	private:
		int _cur_idx;
		int _n_items;
		T _data[N_ITEMS];

	public:
		RingBuffer(){
			clear();
		}

		void clear(){
			_cur_idx = 0;
			_n_items = 0;
		}

		void insert(const T& item){
			_data[_cur_idx] = item;
			_cur_idx = (_cur_idx + 1) % N_ITEMS;
			_n_items = std::min(N_ITEMS, _n_items + 1);
		}

		bool has_item(const T& item) const {
			for(int i=0; i<_n_items; i++){
				if(_data[i] == item){
					return true;
				}
			}

			return false;
		}

		int count() const
		{
			return _n_items;
		}

		bool is_empty() const
		{
			return (_n_items == 0);
		}
};

/**
 * @brief Global handler for current playback state (Singleton)
 * @ingroup Components
 */
class PlayManager : public QObject, protected SayonaraClass
{

	Q_OBJECT

	SINGLETON_QOBJECT(PlayManager)

	public:

		/**
		 * @brief Current Playing state
		 */
		enum class PlayState : quint8 {
					   Playing=0,
					   Paused,
					   Stopped
		};


signals:

	/**
	 * @brief emitted when a streamed track has finished
	 * @param old_md the last played track
	 */
	void sig_www_track_finished(const MetaData& old_md);

	/**
	 * @brief emitted, when PlayManager::PlayState was changed
	 */
	void sig_playstate_changed(PlayManager::PlayState);

	/**
	 * @brief next track was triggered
	 */
	void sig_next();

	/**
	 * @brief previous track was triggered
	 */
	void sig_previous();

	/**
	 * @brief stop was triggered
	 */
	void sig_stopped();

	/**
	 * @brief relative seeking was triggered
	 * @param percent relative position in track
	 */
	void sig_seeked_rel(double percent);

	/**
	 * @brief relative seeking was triggered
	 * @param ms relative position to current position in milliseconds
	 */
	void sig_seeked_rel_ms(qint64 ms);

	/**
	 * @brief absolute seeking was triggered
	 * @param ms absolute position in milliseconds
	 */
	void sig_seeked_abs_ms(quint64 ms);

	/**
	 * @brief position in track has changed
	 * @param ms absolute position in milliseconds
	 */
	void sig_position_changed_ms(quint64 ms);

	/**
	 * @brief track has changed
	 * @param md new MetaData
	 */
	void sig_track_changed(const MetaData& md);

	/**
	 * @brief track has changed
	 * @param idx index in playlist
	 */
	void sig_track_idx_changed(int idx);

	/**
	 * @brief playlist has changed
	 * @param len new size of playlist
	 */
	void sig_playlist_changed(int len);

	/**
	 * @brief duration of track has changed
	 * @param ms duration of track in milliseconds
	 */
	void sig_duration_changed(quint64 ms);

	/**
	 * @brief playlist has finished
	 */
	void sig_playlist_finished();

	/**
	 * @brief recording is requested
	 * @param b
	 *  true, when a new recording session should begin,
	 *  false if a recording session should stop
	 */
	void sig_record(bool b);

	/**
	 * @brief emitted when currently in buffering state
	 * @param b true if buffering, false else
	 */
	void sig_buffer(int b);

	/**
	 * @brief emitted when volume has changed
	 * @param vol value between 0 and 100
	 */
	void sig_volume_changed(int vol);


	/**
	 * @brief emitted when mute state has changed
	 * @param b true if muted, false else
	 */
	void sig_mute_changed(bool b);

	void sig_md_changed(const MetaData& md);


	void sig_duration_changed(qint64 ms);


public slots:
	/**
	 * @brief Start playing if there's a track
	 */
	void play();

	/**
	 * @brief toggle play/pause
	 */
	void play_pause();

	/**
	 * @brief pause track, if currently playing
	 */
	void pause();

	/**
	 * @brief change to previous track
	 */
	void previous();

	/**
	 * @brief change to next track
	 */
	void next();

	/**
	 * @brief stop playback
	 */
	void stop();

	/**
	 * @brief request recording (see also sig_record(bool b))
	 * @param b
	 *  true, when a new recording session should begin,
	 *  false if a recording session should stop
	 */
	void record(bool b);

	/**
	 * @brief seek relative
	 * @param percent relative position within track
	 */
	void seek_rel(double percent);

	/**
	 * @brief seek absolute
	 * @param ms absolute position in milliseconds
	 */
	void seek_abs_ms(quint64 ms);

	/**
	 * @brief seek_rel_ms
	 * @param ms relative position to current position in milliseconds
	 */
	void seek_rel_ms(qint64 ms);

	/**
	 * @brief set current position of track
	 * This method does not seek.
	 * Just tells the playmanager where the current position is
	 * @param ms position in milliseconds. 	 
	 */
	void set_position_ms(quint64 ms);

	/**
	 * @brief change current track
	 * @param md new MetaData object
	 */
	void change_track(const MetaData& md, int playlist_idx);


	/**
	 * @brief change duration of track
	 * @param duration_ms new duration in milliseconds
	 */
	void duration_changed(quint64 duration_ms);

	/**
	 * @brief notify, that track is ready for playback
	 */
	void set_track_ready();

	/**
	 * @brief notifiy, that track is in buffering state currently
	 * @param progress
	 */
	void buffering(int progress);

	/**
	 * @brief increase volume by 5
	 */
	void volume_up();

	/**
	 * @brief decrease volume by 5
	 */
	void volume_down();

	/**
	 * @brief set volume
	 * @param vol value between [0,100], will be cropped if not within boundaries
	 */
	void set_volume(int vol);

	/**
	 * @brief mute/unmute
	 * @param b
	 */
	void set_mute(bool b);


	void change_metadata(const MetaData& md);



	void change_duration(qint64 ms);

public:
	/**
	 * @brief get current play state
	 * @return PlayState enum
	 */
	PlayState	get_play_state() const;

	/**
	 * @brief get current position in milliseconds
	 * @return current position in milliseconds
	 */
	quint64		get_cur_position_ms() const;

	/**
	 * @brief get position in milliseconds where track will start
	 * @return position in milliseconds where track will start
	 */
	quint64		get_init_position_ms() const;

	/**
	 * @brief get duration of track
	 * @return duration in milliseconds
	 */
	quint64		get_duration_ms() const;

	/**
	 * @brief get current track
	 * @return MetaData object of current track
	 */
	MetaData	get_cur_track() const;

	/**
	 * @brief get current volume
	 * @return value between 0 and 100
	 */
	int			get_volume() const;


	/**
	 * @brief query mute status
	 * @return true if muted, false else
	 */
	bool		get_mute() const;


private:
	RingBuffer<QString, 3>	_ring_buffer;
	quint64					_position_ms;
	int						_cur_idx;
	quint64					_initial_position_ms;
	PlayState				_playstate;
	MetaData				_md;

};



#endif

