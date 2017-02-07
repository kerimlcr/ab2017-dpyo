/* GUI_AlternativeCovers.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * GUI_AlternativeCovers.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: Lucio Carreras
 */

#include "GUI_AlternativeCovers.h"
#include "AlternativeCoverItemDelegate.h"
#include "AlternativeCoverItemModel.h"

#include "Components/Covers/CoverLookup.h"
#include "Components/Covers/CoverLookupAlternative.h"

#include "Database/DatabaseConnector.h"

#include "GUI/Helper/Message/Message.h"
#include "Helper/Helper.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QRegExp>



GUI_AlternativeCovers::GUI_AlternativeCovers(QWidget* parent) :
	SayonaraDialog(parent),
	Ui::AlternativeCovers()
{

	setupUi(this);

	QString lib_path = _settings->get(Set::Lib_Path);

	if(QFile::exists(lib_path)){
		_last_path = lib_path;
	}

	else {
		_last_path = QDir::homePath();
	}

	_cur_idx = -1;
	_is_searching = false;

	_model = new AlternativeCoverItemModel(this);
	_delegate = new AlternativeCoverItemDelegate(this);

	tv_images->setModel(_model);
	tv_images->setItemDelegate(_delegate);

	connect(btn_ok, &QPushButton::clicked, this, &GUI_AlternativeCovers::ok_clicked);
	connect(btn_apply, &QPushButton::clicked, this, &GUI_AlternativeCovers::apply_clicked);
	connect(btn_search, &QPushButton::clicked, this, &GUI_AlternativeCovers::search_clicked);
	connect(tv_images, &QTableView::pressed, this, &GUI_AlternativeCovers::cover_pressed);
	connect(btn_file, &QPushButton::clicked, this, &GUI_AlternativeCovers::open_file_dialog);
	connect(btn_cancel, &QPushButton::clicked, this, &SayonaraDialog::close);
}


GUI_AlternativeCovers::~GUI_AlternativeCovers() 
{
	delete_all_files();
}


void GUI_AlternativeCovers::language_changed() {
	retranslateUi(this);
}

void GUI_AlternativeCovers::connect_and_start() {

    reset_model();
	delete_all_files();

	connect(_cl_alternative, &CoverLookupAlternative::sig_cover_found, this, &GUI_AlternativeCovers::cl_new_cover);
	connect(_cl_alternative, &CoverLookupAlternative::sig_finished, this, &GUI_AlternativeCovers::cl_finished);

	_is_searching = true;

    btn_ok->setEnabled(false);
    btn_apply->setEnabled(false);
	btn_search->setText(tr("Stop"));
	le_search->setText(_cover_location.search_term());
	lab_info->setText(_cover_location.search_term());

	_cl_alternative->start();

	show();
}

void GUI_AlternativeCovers::start(const CoverLocation& cl)
{
	if(!cl.valid()){
		return;
	}

	_cover_location = cl;
	le_search->setText( _cover_location.search_term() );

	_cl_alternative = new CoverLookupAlternative(this, _cover_location, 10);
	connect_and_start();
}

void GUI_AlternativeCovers::ok_clicked()
{
	apply_clicked();
	close();
}

void GUI_AlternativeCovers::apply_clicked() 
{
	if(_cur_idx == -1) {
		return;
	}

	RowColumn rc = _model->cvt_2_row_col(_cur_idx);

	QModelIndex idx = _model->index(rc.row, rc.col);

	if(!idx.isValid()) {
		return;
	}

	QString cover_path = _model->data(idx, Qt::UserRole).toString();
	QFile file(cover_path);

	if(!file.exists()) {
		Message::warning(tr("This cover does not exist"));
		return;
	}

	QImage img(cover_path);
	if(img.isNull()){
		return;
	}

	img.save(_cover_location.cover_path());

	emit sig_cover_changed(_cover_location);
}

void GUI_AlternativeCovers::search_clicked() 
{
	if(_is_searching && _cl_alternative){
		_cl_alternative->stop();
		return;
	}

	if(!le_search->text().isEmpty()){
		_cover_location.set_search_term(le_search->text());
	}

	else{
		le_search->setText( _cover_location.search_term() );
	}

	_cl_alternative = new CoverLookupAlternative(this, _cover_location, 10);

	connect_and_start();
}


void GUI_AlternativeCovers::cl_new_cover(const QString& cover_path) 
{
	_filelist << cover_path;

	int n_files = _filelist.size();

	RowColumn rc_last =     _model->cvt_2_row_col( n_files - 1 );
	RowColumn rc_cur_idx =  _model->cvt_2_row_col( _cur_idx );
	QModelIndex model_idx = _model->index(rc_last.row, rc_last.col);
	bool is_valid =         _model->is_valid(rc_cur_idx.row, rc_cur_idx.col);

	_model->setData(model_idx, cover_path);

	btn_ok->setEnabled(is_valid);
	btn_apply->setEnabled(is_valid);
	lab_status->setText( tr("%1 covers found").arg(n_files) ) ;
}


void GUI_AlternativeCovers::cl_finished(bool b) 
{
	Q_UNUSED(b)

	_is_searching = false;

	btn_search->setText(tr("Search"));

	_cl_alternative->deleteLater();
	_cl_alternative = nullptr;
}


void GUI_AlternativeCovers::cover_pressed(const QModelIndex& idx) 
{
	int row = idx.row();
	int col = idx.column();
	bool valid = _model->is_valid(row, col);
	_cur_idx = _model->cvt_2_idx(row, col);

	btn_ok->setEnabled(valid);
	btn_apply->setEnabled(valid);
}


void GUI_AlternativeCovers::reset_model() 
{
    _model->removeRows(0, _model->rowCount());
	_model->insertRows(0, _model->rowCount());

	for(int y=0; y<_model->rowCount(); y++) {
		for(int x=0; x<_model->columnCount(); x++) {

            QModelIndex idx = _model->index(y,x);
			QString sayonara_logo = Helper::get_share_path() + "logo.png";
			_model->setData(idx, sayonara_logo);
        }
    }

    lab_status->setText("");
}


void GUI_AlternativeCovers::open_file_dialog() 
{
	QString lib_path = _settings->get(Set::Lib_Path);

	QDir dir( lib_path );

    QStringList filters;
        filters << "*.jpg";
        filters << "*.png";
        filters << "*.gif";

	dir.setFilter(QDir::Files);
	dir.setNameFilters(filters);

	for(const QString& f : dir.entryList()) {
		QFile::remove(dir.absoluteFilePath(f));
	}

    QStringList lst = QFileDialog::getOpenFileNames(this,
                                  tr("Open image files"),
                                  _last_path,
                                  filters.join(" "));
	if(lst.isEmpty())
	{
		return;
	}

    reset_model();

	int idx = 0;
	for(const QString& path : lst) 
{
		RowColumn rc = _model->cvt_2_row_col( idx );
		QModelIndex model_idx = _model->index(rc.row, rc.col);

		_model->setData(model_idx, path);

        idx ++;
    }
}


void GUI_AlternativeCovers::delete_all_files() 
{
	for(const QString& cover_path : _filelist) {
		if(CoverLocation::isInvalidLocation(cover_path)){
			continue;
		}

		QFile f(cover_path);
		f.remove();
	}

	_filelist.clear();
}


void GUI_AlternativeCovers::closeEvent(QCloseEvent *e) 
{
	if(_cl_alternative) {
		_cl_alternative->stop();
	}

	delete_all_files();

	SayonaraDialog::closeEvent(e);
}
