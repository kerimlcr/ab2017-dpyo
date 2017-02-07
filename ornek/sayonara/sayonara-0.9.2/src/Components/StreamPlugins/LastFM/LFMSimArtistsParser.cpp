/* LFMSimArtistsParser.cpp */

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




#include "LFMSimArtistsParser.h"
#include "Helper/Logger/Logger.h"

#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>

LFMSimArtistsParser::LFMSimArtistsParser(const QString& artist_name, const QByteArray &arr){

	_artist_name = artist_name;

	_data = arr;

	parse_document();
}

LFMSimArtistsParser::LFMSimArtistsParser(const QString& artist_name, const QString& filename)
{
	_artist_name = artist_name;

	QFile f(filename);
	if(f.open(QFile::ReadOnly)){
		_data = f.readAll();
		f.close();
	}

	parse_document();
}



void LFMSimArtistsParser::parse_document()
{
	bool success;
	QDomDocument doc("similar_artists");
	success = doc.setContent(_data);

	if(!success){
		_artist_match = ArtistMatch("");
		sp_log(Log::Warning) << "Cannot parse similar artists document";
		return;
	}

	_artist_match = ArtistMatch(_artist_name);

	QDomElement docElement = doc.documentElement();
	QDomNode similar_artists = docElement.firstChild();			// similarartists

	if(!similar_artists.hasChildNodes()) {
		return;
	}

	QString artist_name, mbid;
	double match = -1.0;

	QDomNodeList child_nodes = similar_artists.childNodes();
	for(int idx_artist=0; idx_artist < child_nodes.size(); idx_artist++) {

		QDomNode artist = child_nodes.item(idx_artist);
		QString node_name = artist.nodeName();

		if(node_name.compare("artist", Qt::CaseInsensitive) != 0) {
			continue;
		}

		if(!artist.hasChildNodes()) {
			continue;
		}

		QDomNodeList artist_child_nodes = artist.childNodes();
		for(int idx_content = 0; idx_content <artist_child_nodes.size(); idx_content++) {
			QDomNode content = artist_child_nodes.item(idx_content);
			QString node_name = content.nodeName().toLower();

			if(node_name.compare("name") == 0) {
				QDomElement e = content.toElement();
				if(!e.isNull()) {
					artist_name = e.text();
				}
			}

			else if(node_name.compare("match") == 0) {
				QDomElement e = content.toElement();
				if(!e.isNull()) {
					match = e.text().toDouble();
				}
			}

			else if(node_name.compare("mbid") == 0){
				QDomElement e = content.toElement();
				if(!e.isNull()) {
					mbid = e.text();
				}
			}

			if(!artist_name.isEmpty() && match > 0 && !mbid.isEmpty()) {
				ArtistMatch::ArtistDesc artist_desc(artist_name, mbid);
				_artist_match.add(artist_desc, match);
				artist_name = "";
				match = -1.0;
				mbid = "";
				break;
			}
		}
	}
}



ArtistMatch LFMSimArtistsParser::get_artist_match() const
{
	return _artist_match;
}
