/* LibraryItem.cpp */

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


#include "Helper/MetaData/LibraryItem.h"

#define LI_DO_COPY \
	cover_download_url = other.cover_download_url; \
	db_id = other.db_id; \
	_additional_data = other._additional_data;


CustomField::CustomField(const QString& id, const QString& display_name, const QString& value){
	_id = id;
	_display_name = display_name;
	_value = value;
}

CustomField::CustomField(const CustomField &copy){
	_id = copy._id;
	_display_name = copy._display_name;
	_value = copy._value;
}

QString CustomField::get_id() const {
	return _id;
}

QString CustomField::get_display_name() const
{
	return _display_name;
}

QString CustomField::get_value() const
{
	return _value;
}


LibraryItem::LibraryItem()  {
	db_id = -1;
}

LibraryItem::LibraryItem(const LibraryItem& other){
	LI_DO_COPY
}

LibraryItem::LibraryItem(LibraryItem&& other){
	LI_DO_COPY
}

LibraryItem& LibraryItem::operator=(const LibraryItem& other){
	LI_DO_COPY
	return *this;
}


LibraryItem::~LibraryItem() {}

void LibraryItem::add_custom_field(const CustomField& field){
	_additional_data << field;
}

void LibraryItem::add_custom_field(const QString& id, const QString& display_name, const QString& value){
	_additional_data << CustomField(id, display_name, value);
}

const QList<CustomField>& LibraryItem::get_custom_fields() const {
	return _additional_data;
}

int LibraryItem::has_custom_field(const QString& id) const {
	for(int i=0; i<_additional_data.size(); i++){
		if(_additional_data[i].get_id() == id){
			return i;
		}
	}

	return -1;
}

QString LibraryItem::get_custom_field(const QString& id) const {

	for(const CustomField& field : _additional_data){

		if(field.get_id().compare(id, Qt::CaseInsensitive) == 0){
			return field.get_value();
		}
	}

	return "";
}


QString LibraryItem::get_custom_field(int idx) const {
	if(idx < 0 || idx >= _additional_data.size()){
		return "";
	}

	return _additional_data[idx].get_value();
}



void LibraryItem::print() const {}


