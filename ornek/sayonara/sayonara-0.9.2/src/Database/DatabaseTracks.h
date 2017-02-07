/* DatabaseTracks.h */

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



#ifndef DATABASETRACKS_H
#define DATABASETRACKS_H

#include "Components/Library/Sorting.h"
#include "Database/DatabaseModule.h"
#include "Helper/globals.h"
#include "Helper/Filter.h"
#include "Helper/LibrarySearchMode.h"

class MetaData;
class MetaDataList;
class CustomPlaylist;
class DatabaseTracks : private DatabaseModule
{

private:
	QString _fetch_query;

protected:
	void set_track_fetch_query(const QString& query);

protected:
	QString append_track_sort_string(QString querytext, SortOrder sort);

public:
	DatabaseTracks(QSqlDatabase db, quint8 db_id);

	virtual bool db_fetch_tracks(SayonaraQuery& q, MetaDataList& result);

	virtual bool getAllTracksByAlbum(int album, MetaDataList& result, Filter filter=Filter(), SortOrder sortorder = SortOrder::TrackArtistAsc, int discnumber=-1);
	virtual bool getAllTracksByAlbum(IDList albums, MetaDataList& result, Filter filter=Filter(), SortOrder sortorder =SortOrder::TrackArtistAsc);

	virtual bool getAllTracksByArtist(int artist, MetaDataList& result, Filter filter=Filter(), SortOrder sortorder = SortOrder::TrackArtistAsc);
	virtual bool getAllTracksByArtist(IDList artists, MetaDataList& result, Filter filter=Filter(), SortOrder sortorder =SortOrder::TrackArtistAsc);

	virtual bool getAllTracksBySearchString(Filter filter, MetaDataList& result, SortOrder sortorder = SortOrder::TrackArtistAsc);

	virtual bool insertTrackIntoDatabase (const MetaData& data,int artistID, int albumID);
	virtual bool updateTrack(const MetaData& data);
	virtual bool updateTracks(const MetaDataList& lst);

	virtual bool getTracksFromDatabase (MetaDataList& returndata, SortOrder sortorder = SortOrder::TrackArtistAsc);
	virtual MetaData getTrackById(int id);
	virtual MetaData getTrackByPath(const QString& path);
	virtual bool getMultipleTracksByPath(const QStringList& paths, MetaDataList& v_md);

	virtual bool deleteTrack(int id);
	virtual bool deleteTracks(const MetaDataList&);
	virtual bool deleteTracks(const IDList& ids);
	virtual bool deleteInvalidTracks();

	virtual QStringList getAllGenres();
	virtual void updateTrackCissearch(LibraryHelper::SearchModeMask mode);

	bool updateTrackDates();

};

#endif // DATABASETRACKS_H
