/* StreamHandlerPodcasts.h */

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



#ifndef STREAMHANDLERPODCASTS_H
#define STREAMHANDLERPODCASTS_H

#include "AbstractStreamHandler.h"

class StreamHandlerPodcasts : public AbstractStreamHandler
{
	Q_OBJECT
public:
	StreamHandlerPodcasts(QObject* parent=nullptr);
	~StreamHandlerPodcasts();

	bool get_all_streams(StreamMap& streams) override;
	bool add_stream(const QString& station_name, const QString& url) override;
	bool delete_stream(const QString& station_name) override;
	bool update_url(const QString& station_name, const QString& url) override;
	bool rename_stream(const QString &station_name, const QString &url) override;
};

#endif // STREAMHANDLERPODCASTS_H
