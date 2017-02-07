/* ASXParser.cpp */

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



#include "ASXParser.h"

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

ASXParser::ASXParser(const QString& filename) :
	AbstractPlaylistParser(filename)
{
}


void ASXParser::parse()  {


	QDomDocument doc("AsxFile");
	doc.setContent(_file_content);

	QDomElement docElement = doc.documentElement();
	QDomNode child_node = docElement.firstChild();
	QDomNode entry;

	QString node_name = child_node.nodeName();
	if(node_name.compare("entry", Qt::CaseInsensitive) == 0){
		entry = child_node.toElement();
	}

	if(!entry.hasChildNodes()) {
		return;
	}

	do{

		MetaData md;

		md.length_ms = 0;
		md.album = "";


		for(int i=0; i<entry.childNodes().size(); i++)
		{

			QDomNode node = entry.childNodes().at(i);
			QString node_name = node.nodeName().toLower();

			QDomElement e = node.toElement();
			if(e.isNull()){
				continue;
			}

			if(node_name.compare("ref") == 0) {

				QString file_path = parse_ref_node(node);
				md.artist = file_path;
				md.set_filepath(file_path);
			}

			else if(!node_name.compare("title")) {
				md.title = e.text();
			}

			else if(!node_name.compare("album")) {
				md.album = e.text();
			}

			else if(!node_name.compare("author")) {
				md.artist = e.text();
			}
		}

		_v_md << md;

		entry = entry.nextSibling();

	} while (!entry.isNull());
}

QString ASXParser::parse_ref_node(QDomNode node){

	QDomElement e = node.toElement();
	QDomNamedNodeMap map = e.attributes();

	for(int j=0; j<map.size(); j++){

		QDomNode item_node = map.item(j);
		QString nodename = item_node.nodeName();
		if(nodename.compare("href", Qt::CaseInsensitive) == 0){

			QString path = e.attribute(nodename);
			return  get_absolute_filename(path);
		}
	}

	return "";
}
