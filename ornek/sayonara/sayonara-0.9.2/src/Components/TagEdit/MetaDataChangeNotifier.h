/* MetaDataChangeNotifier.h */

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



#ifndef METADATACHANGENOTIFIER_H
#define METADATACHANGENOTIFIER_H

#include "Helper/globals.h"

class MetaDataList;
/**
 * @brief The MetaDataChangeNotifier class (Singleton)
 * @ingroup Tagging
 */
class MetaDataChangeNotifier : public QObject
{
	Q_OBJECT

	SINGLETON_QOBJECT(MetaDataChangeNotifier)

public:
		/**
		 * @brief change_metadata Tell the MetaDataChangeNotifier that MetaData has been changed
		 * @param v_md_old The original Metadata used for comparison
		 * @param v_md_new The actualized Metadata
		 */
	void change_metadata(const MetaDataList& v_md_old, const MetaDataList& v_md_new);

signals:
	/**
	 * @brief sig_metadata_changed Signal emitted when change_metadata was called
	 * @param v_md_oldThe original Metadata used for comparison
	 * @param v_md_new The actualized Metadata
	 */
	void sig_metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new);

};

#endif // METADATACHANGENOTIFIER_H
