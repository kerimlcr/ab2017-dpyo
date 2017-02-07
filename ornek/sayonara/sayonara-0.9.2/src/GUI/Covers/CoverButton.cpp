/* CoverButton.cpp */

/* Copyright (C) 2011-2015  Lucio Carreras
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



#include "CoverButton.h"
#include "GUI_AlternativeCovers.h"
#include "Components/Covers/CoverLookup.h"


CoverButton::CoverButton(QWidget* parent) : 
	QPushButton(parent)
{
	_cover_forced = false;
	_found_cover_location = CoverLocation::getInvalidLocation();
	_search_cover_location = CoverLocation::getInvalidLocation();

	connect(this, &QPushButton::clicked, this, &CoverButton::cover_button_clicked);
}

CoverButton::~CoverButton(){}



void CoverButton::cover_button_clicked(){

	_found_cover_location = CoverLocation::getInvalidLocation();

	if(!_alternative_covers){
		_alternative_covers = new GUI_AlternativeCovers(this);

		connect(_alternative_covers, &GUI_AlternativeCovers::sig_cover_changed, 
				this, &CoverButton::alternative_cover_fetched );
	}

	_alternative_covers->start(_search_cover_location);
}


void CoverButton::set_cover_location(const CoverLocation& cl){

	_search_cover_location = cl;

	if(!_cover_lookup){
		_cover_lookup = new CoverLookup(this);
		connect(_cover_lookup, &CoverLookup::sig_cover_found, this, &CoverButton::set_cover_image);
	}

	_cover_forced = false;
	_cover_lookup->fetch_cover(cl);
}

void CoverButton::force_icon(const QIcon& icon){

	_cover_forced = true;
	this->setIcon(icon);

	this->setToolTip("MP3 Tag");
}


void CoverButton::alternative_cover_fetched(const CoverLocation& cl){

	_found_cover_location = cl;

	if(cl.valid()){
		emit sig_cover_replaced();
	}

	set_cover_image(cl.cover_path());
}


void CoverButton::cover_found(const CoverLocation &cl){

	_found_cover_location = cl;

	if(cl.valid()){
		emit sig_cover_found();
	}

	set_cover_image(cl.cover_path());
}


void CoverButton::set_cover_image(const QString& cover_path){

	if(_cover_forced && sender() == _cover_lookup){
		return;
	}

	QIcon icon(cover_path);
	this->setIcon(icon);

	this->setToolTip("");
}


bool CoverButton::has_valid_cover() const
{
	return _found_cover_location.valid();
}

CoverLocation CoverButton::get_found_cover() const
{
	return _found_cover_location;
}

