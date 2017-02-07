/* CustomMimeData.cpp */

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



#include "CustomMimeData.h"

CustomMimeData::CustomMimeData() :
	QMimeData()
{
}

CustomMimeData::~CustomMimeData(){}


void CustomMimeData::setMetaData(const MetaDataList& v_md)
{
    _v_md = v_md;
}

MetaDataList CustomMimeData::getMetaData() const
{
	return _v_md;
}

bool CustomMimeData::hasMetaData() const
{
	return (_v_md.size() > 0);
}

void CustomMimeData::setCoverUrl(const QString& url)
{
	_cover_url = url;
}

QString CustomMimeData::getCoverUrl() const 
{
	return _cover_url;
}

bool CustomMimeData::hasCoverUrl() const
{
	return (!_cover_url.isEmpty());
}
