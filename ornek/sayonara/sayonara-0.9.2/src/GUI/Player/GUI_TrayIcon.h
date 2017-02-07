/* GUI_TrayIcon.h */

/* Copyright (C) 2012-2016 Lucio Carreras  gleugner
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


#ifndef GUI_TRAYICON_H
#define GUI_TRAYICON_H

#include "Helper/Settings/SayonaraClass.h"

#include "Interfaces/Notification/NotificationHandler.h"
#include "Components/PlayManager/PlayManager.h"

#include <QSystemTrayIcon>
#include <QAction>
#include <QTimer>

class MetaData;
/**
  * Small class to be used as tray icon
  */
class GUI_TrayIcon :
		public QSystemTrayIcon,
		public NotificationInterface,
		private SayonaraClass

{
    Q_OBJECT

public:

    GUI_TrayIcon(QObject *parent=nullptr);
    virtual ~GUI_TrayIcon();

	bool event ( QEvent * e ) override;


   void set_enable_play(bool);
   void set_enable_stop(bool);
   void set_enable_mute(bool);
   void set_enable_fwd(bool);
   void set_enable_bwd(bool);

   void notify(const MetaData& md) override;
   void notify(const QString &title, const QString &message, const QString &image_path) override;


public slots:

	void skin_changed();

signals:

    /**
      * this event is fired, if we have a mouse wheel event
      * @param delta bigger then 0 when mouse wheel has moved forward smaller when moved backwards
      */
	void sig_wheel_changed(int delta);
	void sig_hide_clicked();
    void sig_close_clicked();
	void sig_show_clicked();


private slots:
	void play_clicked();
	void stop_clicked();
	void fwd_clicked();
	void bwd_clicked();
	void show_clicked();
	void close_clicked();
	void mute_clicked();
	void cur_song_clicked();

	void playstate_changed(PlayManager::PlayState state);

	void mute_changed(bool muted);
	void _sl_show_tray_icon();



protected:
	virtual void  language_changed();

private:
	// some shared actions
	PlayManager* _play_manager=nullptr;

	QAction*	_show_action=nullptr;
	QAction*	_close_action=nullptr;
	QAction*	_play_action=nullptr;
	QAction*	_stop_action=nullptr;
	QAction*	_mute_action=nullptr;
	QAction*	_fwd_action=nullptr;
	QAction*	_bwd_action=nullptr;
	QAction*	_cur_song_action=nullptr;

	QMenu*		_context_menu=nullptr;
	QTimer*		_timer=nullptr;

private:
	void init_context_menu();

};


#endif
