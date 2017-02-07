/* DBusMPRIS.h */

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



#ifndef DBUS_MPRIS_H
#define DBUS_MPRIS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QDBusObjectPath>

#include "Helper/Settings/SayonaraClass.h"
#include "Components/PlayManager/PlayManager.h"

class SayonaraMainWindow;
class DBusAdaptor : public QObject {

	Q_OBJECT

protected:

	PlayManager* _play_manager=nullptr;

	QString		_object_path;
	QString		_service_name;
	QString		_dbus_service;
	QString		_dbus_interface;


	explicit DBusAdaptor(QObject *parent=nullptr);
	virtual ~DBusAdaptor();

	void create_message(QString name, QVariant val);
};



namespace DBusMPRIS {


class MediaPlayer2 : public DBusAdaptor, protected SayonaraClass
{

	Q_OBJECT

	private:
		QStringList _supported_uri_schemes;
		QStringList _supported_mime_types;


	public:

		explicit MediaPlayer2(SayonaraMainWindow* player, QObject *parent=nullptr);
		virtual ~MediaPlayer2();

		Q_PROPERTY(bool			CanQuit				READ CanQuit)
		bool					CanQuit();

		Q_PROPERTY(bool			CanRaise			READ CanRaise)
		bool					CanRaise();

		Q_PROPERTY(bool			HasTrackList		READ HasTrackList)
		bool					HasTrackList();


		Q_PROPERTY(QString		Identity			READ Identity)
		QString					Identity();

		Q_PROPERTY(QString		DesktopEntry		READ DesktopEntry)
		QString					DesktopEntry();

		Q_PROPERTY(QStringList	SupportedUriSchemes	READ SupportedUriSchemes)
		QStringList				SupportedUriSchemes();


		Q_PROPERTY(QStringList	SupportedMimeTypes	READ SupportedMimeTypes)
		QStringList				SupportedMimeTypes();


		Q_PROPERTY(bool			CanSetFullscreen	READ CanSetFullscreen)
		bool					CanSetFullscreen();

		Q_PROPERTY(bool			Fullscreen			READ Fullscreen				WRITE SetFullscreen)
		bool					Fullscreen();
		void					SetFullscreen(bool b);

		void					Raise();
		void					Quit();


	private:

		SayonaraMainWindow*	_player=nullptr;
		int				_len_playlist;
		int				_cur_idx;

		bool			_can_next;
		bool			_can_previous;
		double			_volume;
		QString			_playback_status;
		MetaData		_md;
		qint64			_pos;

		bool			_initialized;
		void			init();


	public:
		Q_PROPERTY(QString		PlaybackStatus		READ	PlaybackStatus)
		QString					PlaybackStatus();


		Q_PROPERTY(QString		LoopStatus			READ	LoopStatus	WRITE	SetLoopStatus)
		QString					LoopStatus();
		void					SetLoopStatus(QString status);



		Q_PROPERTY(double		Rate				READ	Rate		WRITE	SetRate)
		double					Rate();
		void					SetRate(double rate);



		Q_PROPERTY(bool			Shuffle				READ	Shuffle		WRITE	SetShuffle)
		bool					Shuffle();
		void					SetShuffle(bool shuffle);


		Q_PROPERTY(QVariantMap	Metadata			READ	Metadata)
		QVariantMap				Metadata();


		Q_PROPERTY(double		Volume				READ	Volume		WRITE	SetVolume)
		double					Volume();
		void					SetVolume(double volume);


		Q_PROPERTY(qint64	Position			READ	Position)
		qint64				Position();
		void				SetPosition(const QDBusObjectPath& track_id, qint64 position);



		Q_PROPERTY(double		MinimumRate			READ	MinimumRate)
		double					MinimumRate();


		Q_PROPERTY(double		MaximumRate			READ	MaximumRate)
		double					MaximumRate();


		Q_PROPERTY(bool			CanGoNext			READ	CanGoNext)
		bool					CanGoNext();


		Q_PROPERTY(bool			CanGoPrevious		READ	CanGoPrevious)
		bool					CanGoPrevious();


		Q_PROPERTY(bool			CanPlay				READ	CanPlay)
		bool					CanPlay();


		Q_PROPERTY(bool			CanPause			READ	CanPause)
		bool					CanPause();


		Q_PROPERTY(bool			CanSeek				READ	CanSeek)
		bool					CanSeek();


		Q_PROPERTY(bool			CanControl			READ	CanControl)
		bool					CanControl();


		void					Next();
		void					Previous();
		void					Pause();
		void					PlayPause();
		void					Stop();
		void					Play();
		void					Seek(qint64 offset);

		void					OpenUri(const QString& uri);


	public slots:

		void					position_changed(quint64 pos_ms);
		void					volume_changed(int volume);
		void					track_idx_changed(int idx);
		void					playlist_len_changed(int len);
		void					track_changed(const MetaData& md);
		void					playstate_changed(PlayManager::PlayState);

	signals:
		void					Seeked(qint64 position);
		void					sig_raise();

};

} // end namespace DBusMPRIS

#endif // DBUS_MPRIS_H
