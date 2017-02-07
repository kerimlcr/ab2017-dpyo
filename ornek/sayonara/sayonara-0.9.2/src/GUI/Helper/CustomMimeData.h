/* CustomMimeData.h */

/* Copyright (C) 2013  Lucio Carreras
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



#ifndef _CUSTOMMIMEDATA_H
#define _CUSTOMMIMEDATA_H

#include "Helper/MetaData/MetaDataList.h"
#include <QMimeData>
#include <QUrl>

/**
 * @brief Mimedata class for drag and dropping metadata
 * @ingroup GUIHelper
 */
class CustomMimeData : public QMimeData {


private:
    MetaDataList    _v_md;
	QString			_cover_url;

public:
    /**
     * @brief Constructor
     */
    CustomMimeData();

    virtual ~CustomMimeData();

    /**
     * @brief Set metadata you want to drag and drop
     * @param v_md metadata that should be sent
     */
    void setMetaData(const MetaDataList& v_md);

    /**
     * @brief get metadata from drag and drop
     * @param v_md reference to metadata
     * @return size of metadata
     */
	MetaDataList getMetaData() const;

    /**
     * @brief check, if the custom mimedata has metadata
     * @return true if yes, false else
     */
    bool hasMetaData() const;

	void setCoverUrl(const QString& path);
	QString getCoverUrl() const;
	bool hasCoverUrl() const;
};


#endif
