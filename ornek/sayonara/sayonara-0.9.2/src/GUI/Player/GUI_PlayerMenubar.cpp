/*
 * GUI_PlayerMenubar.cpp
 *
 *  Created on: 10.10.2012
 *      Author: Lucio Carreras
 */

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

#include "GUI_Player.h"
#include "GUI/Helper/Message/Message.h"
#include "GUI/Helper/GUI_Helper.h"

#include "Helper/Helper.h"

#include "Components/Playlist/PlaylistHandler.h"
#include "Helper/WebAccess/AsyncWebAccess.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

void GUI_Player::open_files_clicked()
{
	QStringList filetypes;

	filetypes << Helper::get_soundfile_extensions();
	filetypes << Helper::get_playlistfile_extensions();

	QString filetypes_str = tr("Media files") + " (" + filetypes.join(" ") + ")";

	QStringList list =
			QFileDialog::getOpenFileNames(
					this,
					tr("Open Media files"),
					QDir::homePath(),
					filetypes_str);

	if(list.isEmpty()){
		return;
	}

	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->create_playlist(list);
}

void GUI_Player::open_dir_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this,
			tr("Open Directory"),
			QDir::homePath(),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (dir.isEmpty()){
		return;
	}

	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->create_playlist(dir);
}


void GUI_Player::show_library(bool b)
{
	QSize player_size = this->size();

	_settings->set(Set::Lib_Show, b);

	int library_width = library_widget->width();
	library_widget->setVisible(b);

	if(!b){
		if(_settings->get(Set::Lib_OldWidth) == 0){
			_settings->set(Set::Lib_OldWidth, library_width);
		}

		player_size.setWidth( player_size.width() - library_width);
	}

	else{
		library_width = _settings->get(Set::Lib_OldWidth);
		_settings->set(Set::Lib_OldWidth, 0);

		if(library_width < 100){
			library_width = 400;
		}


		player_size.setWidth( player_size.width() + library_width);
	}

	check_library_menu_action();

	if(!this->isMaximized() && !this->isFullScreen()){
		this->resize(player_size);
	}
}


void GUI_Player::show_fullscreen_toggled(bool b)
{
	// may happened because of F11 too
	action_Fullscreen->setChecked(b);

	if(b){
		showFullScreen();
	}

	else {
		showNormal();
	}

	_settings->set(Set::Player_Fullscreen, b);
}


void GUI_Player::help()
{
	Message::info(tr("Please visit the forum at") + "<br />" +
				  Helper::create_link("http://sayonara-player.com/forum", is_dark()) +
				  "<br /><br />" +
				  tr("Donate") + ": <br />" + Helper::create_link("http://sayonara-player.com", is_dark()) +
				  "<br /><br />" +
				  tr("Thank you!") + " :-)"
				  );
}

// private slot
void GUI_Player::about()
{
	QString first_translators;
	QString last_translator;
	QString translator_str = "";
	QString version = _settings->get(Set::Player_Version);

	QString link = Helper::create_link("http://sayonara-player.com");

	if(!_about_box){
		_about_box = new QMessageBox(this);
		_about_box->setParent(this);
		_about_box->setIconPixmap(GUI::get_pixmap("logo.png", QSize(150, 150), true));
		_about_box->setWindowFlags(Qt::Dialog);
		_about_box->setModal(true);
		_about_box->setStandardButtons(QMessageBox::Ok);
		_about_box->setWindowTitle(tr("About Sayonara"));
		_about_box->setText("<b><font size=\"+2\">Sayonara Player " + version + "</font></b>");

		AsyncWebAccess* awa = new AsyncWebAccess(this);
		awa->run("http://sayonara-player.com/translators");
		connect(awa,  &AsyncWebAccess::sig_finished, this, &GUI_Player::awa_translators_finished);

	}

	if(_translators.size() > 2) {

		for (int i=0; i<_translators.size() - 1; i++) {

			first_translators += "<b>" + _translators[i] + "</b>";
			if(i < _translators.size() - 2) first_translators += ", ";
        }

		last_translator = QString("<b>") + _translators[_translators.size() - 1] + "</b>";
        translator_str = QString("<br /><br /><br />") +
                tr("Special thanks to %1 and %2 for translating")
                .arg(first_translators)
                .arg(last_translator);
    }

	_about_box->setInformativeText( QString("") +
				tr("Written by Lucio Carreras") + "<br /><br />" +
                tr("License") + ": GPLv3<br /><br />" +
				"Copyright 2011-" + QString::number(QDateTime::currentDateTime().date().year()) +
				"<br /><br />" + link + translator_str
	);


	if(!_about_box->isVisible()){
		_about_box->exec();
	}
}


