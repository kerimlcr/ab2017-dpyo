/* GUI_TargetPlaylistDialog.cpp */

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



#include "GUI_TargetPlaylistDialog.h"
#include <QFileDialog>

GUI_TargetPlaylistDialog::GUI_TargetPlaylistDialog(QWidget *parent) :
	SayonaraDialog(parent),
	Ui::GUI_TargetPlaylistDialog()
{
	setupUi(this);

	connect(btn_choose, &QPushButton::clicked, this, &GUI_TargetPlaylistDialog::search_button_clicked);
	connect(btn_ok, &QPushButton::clicked, this, &GUI_TargetPlaylistDialog::ok_button_clicked);
}

GUI_TargetPlaylistDialog::~GUI_TargetPlaylistDialog() {

}


void GUI_TargetPlaylistDialog::language_changed() {
	retranslateUi(this);
}



void GUI_TargetPlaylistDialog::search_button_clicked() {

	QString lib_path = _settings->get(Set::Lib_Path);

    QString target_filename = QFileDialog::getSaveFileName(this,
                                                           tr("Choose target file"),
															lib_path,
                                                           "*.m3u");

    if(!target_filename.endsWith("m3u", Qt::CaseInsensitive)) target_filename.append(".m3u");
	le_path->setText(target_filename);
}


void GUI_TargetPlaylistDialog::ok_button_clicked() {
	QString target_filename = le_path->text();
	bool checked = cb_relative->isChecked();

    if(target_filename.size() > 0) {
        emit sig_target_chosen(target_filename, checked);
        close();
    }

}
