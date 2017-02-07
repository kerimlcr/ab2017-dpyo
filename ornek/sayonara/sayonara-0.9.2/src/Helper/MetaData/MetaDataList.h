	/* MetaDataList.h */

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

#ifndef METADATALIST_H
#define METADATALIST_H

#include <QList>
#include "MetaData.h"
#include "Helper/globals.h"

#include <functional>

/**
 * @brief The MetaDataList class
 * @ingroup MetaDataHelper
 */

class MetaDataInfo;
class AlbumInfo;
class ArtistInfo;

namespace SP {
	template<typename T>
	class Set;
}

class MetaDataList : public QList<MetaData> {

private:
	int _cur_played_track;

public:

	enum class Interpretation : quint8
	{
		None=0,
		Artists,
		Albums,
		Tracks
	};

	MetaDataList();
	MetaDataList(const MetaDataList&);

	virtual ~MetaDataList();

	void set_cur_play_track(int idx);
	void set_cur_play_track_idx_only(int idx);
	int get_cur_play_track() const;


	virtual bool contains(const MetaData& md) const;
	MetaDataList& remove_track(int idx);
	MetaDataList& remove_tracks(const SP::Set<int>& rows);
	MetaDataList& remove_tracks(int first, int last);

	MetaDataList& move_tracks(const SP::Set<int>& indexes, int tgt_idx);
	MetaDataList& copy_tracks(const SP::Set<int>& indexes, int tgt_idx);
	MetaDataList& insert_tracks(const MetaDataList& v_md, int tgt_idx);
	MetaDataList& randomize();

	MetaDataList extract_tracks(std::function<bool (const MetaData& md)> func) const;
	//MetaDataList extract_tracks(const IdxList& idx_list) const;
	MetaDataList extract_tracks(const SP::Set<int>& idx_list) const;

	IdxList findTracks(int id) const;
	IdxList findTracks(const QString&) const;

	QStringList toStringList() const;

	MetaDataList& operator <<(const MetaDataList& v_md);
	MetaDataList& operator <<(const MetaData& md);

	bool contains(qint32 id) const;

};


#endif // METADATALIST_H
