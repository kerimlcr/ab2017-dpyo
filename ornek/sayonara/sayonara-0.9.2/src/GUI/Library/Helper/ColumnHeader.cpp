
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


#include "ColumnHeader.h"
#include <algorithm>

ColumnHeader::ColumnHeader(bool switchable, SortOrder sort_asc, SortOrder sort_desc){
	_action = new QAction(this);
	_switchable = switchable;
	_sort_asc = sort_asc;
	_sort_desc = sort_desc;

	_size_type = SizeType::Undefined;
	_action->setChecked(true);
	_action->setCheckable(_switchable);

}

ColumnHeader::ColumnHeader(bool switchable, SortOrder sort_asc, SortOrder sort_desc, int preferred_size_abs) :
	ColumnHeader(switchable, sort_asc, sort_desc)
{
	_preferred_size_abs = preferred_size_abs;
	_preferred_size_rel = 0;

	_size_type = SizeType::Abs;

	_action->setCheckable(_switchable);
}

ColumnHeader::ColumnHeader(bool switchable, SortOrder sort_asc, SortOrder sort_desc, double preferred_size_rel, int min_size) :
	ColumnHeader(switchable, sort_asc, sort_desc)
{
	_preferred_size_abs = min_size;
	_preferred_size_rel = preferred_size_rel;
	_size_type = SizeType::Rel;
}

void ColumnHeader::set_preferred_size_abs(int preferred_size){
	_preferred_size_abs = preferred_size;
	_preferred_size_rel = 0;
	_size_type = SizeType::Abs;
}

void ColumnHeader::set_preferred_size_rel(double preferred_size){
	_preferred_size_rel = preferred_size;
	_preferred_size_abs = 0;
	_size_type = SizeType::Rel;
}

bool ColumnHeader::is_switchable() const {
	return _switchable;
}

int ColumnHeader::get_preferred_size_abs() const {
	return _preferred_size_abs;
}

double ColumnHeader::get_preferred_size_rel() const {
	return _preferred_size_rel;
}

SortOrder ColumnHeader::get_asc_sortorder() const {
	return _sort_asc;
}

SortOrder ColumnHeader::get_desc_sortorder() const {
	return _sort_desc;
}

QAction* ColumnHeader::get_action() {
	_action->setText( this->get_title() );
	return _action;
}

bool ColumnHeader::is_visible() const
{
	if(!_switchable){
		return true;
	}

	bool is_checked = _action->isChecked();
	return is_checked;
}

bool ColumnHeader::is_hidden() const
{
	return (!is_visible());
}

void ColumnHeader::retranslate() {
	_action->setText(this->get_title());
}

ColumnHeader::SizeType ColumnHeader::get_size_type() const {
	return _size_type;
}


int ColumnHeaderList::get_shown_columns() const
{
	int count = std::count_if(this->begin(), this->end(), [](ColumnHeader* header){
		return header->is_visible();
	});

	return count;
}

int ColumnHeaderList::get_nth_shown_col(int n) const
{
	if(n < 0 || n > this->size()){
		return -1;
	}

	for(int i=0; i<this->size(); i++){
		ColumnHeader* header = this->at(i);
		if(header->is_visible()){
			n--;
		}

		if(n < 0){
			return i;
		}
	}

	return -1;
}


