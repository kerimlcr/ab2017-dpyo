/* ArtistInfo.h */

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



#ifndef ARTISTINFO_H
#define ARTISTINFO_H

#include "MetaDataInfo.h"

/**
 * @brief The ArtistInfo class
 * @ingroup MetaDataHelper
 */
class MetaDataList;
class ArtistInfo : public MetaDataInfo {

	Q_OBJECT

private:
	void set_cover_location() override;
	void set_subheader() override;
	void set_header() override;

	void calc_similar_artists(Artist& artist);

public:
	ArtistInfo(const MetaDataList& v_md);
	virtual ~ArtistInfo();

	QString get_cover_album() const override;
	QString get_additional_info_as_string() const override;
};

#endif // ARTISTINFO_H

