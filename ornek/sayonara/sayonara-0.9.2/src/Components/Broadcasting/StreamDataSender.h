/* StreamDataSender.h */

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


#ifndef STREAMDATASENDER_H
#define STREAMDATASENDER_H

#include <QTcpSocket>

/**
 * @brief The StreamDataSender class. This class is used for sending the raw bytes.
 * @ingroup Broadcasting
 */
class StreamDataSender
{

private:
	QTcpSocket*		_socket=nullptr;
	quint64			_sent_data_bytes;

	QByteArray		_header;
	QByteArray		_icy_header;
	QByteArray		_reject_header;

	bool send_icy_metadata(const QString& stream_title);

public:
	StreamDataSender(QTcpSocket* socket);
	~StreamDataSender();

	bool send_trash();
	bool send_data(const uchar* data, quint64 size);
	bool send_icy_data(const uchar* data, quint64 size, const QString& stream_title);
	bool send_header(bool reject, bool icy);
	bool send_html5(const QString& stream_title);
	bool send_bg();
	bool send_metadata(const QString& stream_title);
	bool send_playlist(const QString& host, int port);
	bool send_favicon();

};

#endif // STREAMDATASENDER_H
