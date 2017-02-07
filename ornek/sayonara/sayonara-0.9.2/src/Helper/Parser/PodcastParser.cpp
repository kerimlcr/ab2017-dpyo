/* PodcastParser.cpp */

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


#include <QDomDocument>

#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Parser/PodcastParser.h"

#include <algorithm>

int find_year(QString str) {

    int idx = str.indexOf(QRegExp("[0-9]{4,4}"));

    if(idx >= 0) {
        return str.mid(idx, 4).toInt();
    }

    return 0;
}

int parse_length_s(const QString& str)
{
	QStringList lst = str.split(":");
	int h=0;
	int m=0;
	int s=0;

	if(lst.size() == 3)
	{
		h = lst[0].toInt();
		m = lst[1].toInt();
		s = lst[2].split(".").first().toInt();
	}

	if(lst.size() == 2)
	{
		m = lst[0].toInt();
		s = lst[1].split(".").first().toInt();
	}

	if(lst.size() == 1)
	{
		s = lst[0].split(".").first().toInt();
	}

	return (h * 3600 + m * 60 + s);
}





bool  PodcastParser::parse_podcast_xml_file_content(const QString& content, MetaDataList& v_md) {

    v_md.clear();

    QDomDocument doc("PodcastFile");
    doc.setContent(content);

    QDomElement docElement = doc.documentElement();
	QDomNode first_channel_node = docElement.firstChildElement("channel");

	if(!first_channel_node.hasChildNodes()) return false;

    QString author;
    QStringList categories;
    QString album;
	QString fallback_url;
	QString cover_url;

	for(int c = 0; c<first_channel_node.childNodes().size(); c++) {

		QDomNode channel_content_node = first_channel_node.childNodes().at(c);
		QString channel_content_nodename = channel_content_node.nodeName();
		QDomElement channel_content_element = channel_content_node.toElement();

		if(!channel_content_nodename.compare("title", Qt::CaseInsensitive)) {
			album = channel_content_element.text();
        }

		else if(!channel_content_nodename.compare("itunes:author", Qt::CaseInsensitive)) {
			author = channel_content_element.text();
        }

		else if(!channel_content_nodename.compare("itunes:category", Qt::CaseInsensitive)) {

			QStringList genres = channel_content_element.text().split(QRegExp(",|/|;|\\."));
            for(int i=0; i<genres.size(); i++) {
                genres[i] = genres[i].trimmed();
            }

            categories.append(genres);
        }

		else if(!channel_content_nodename.compare("image", Qt::CaseInsensitive)) {

			QDomNodeList image_content_nodes = channel_content_node.childNodes();

			for(int icn=0; icn<image_content_nodes.size(); icn++) {
				QDomNode image_content_node = image_content_nodes.at(icn);
				QString image_content_nodename = image_content_node.nodeName();
				QDomElement image_content_element = image_content_node.toElement();

				if(!image_content_nodename.compare("url", Qt::CaseInsensitive)) {
					if(!image_content_element.text().isEmpty()){
						cover_url = image_content_element.text();
					}
					break;
                }
            }
		}

		else if(!channel_content_nodename.compare("itunes:image", Qt::CaseInsensitive)) {
			cover_url = channel_content_element.attribute("href");
		}

        // item
		else if(!channel_content_nodename.compare("item", Qt::CaseInsensitive)) {

			if(!channel_content_node.hasChildNodes()) continue;

            MetaData md;
			int n_chapters=0;
            md.genres.push_back("Podcasts");
            md.genres.append(categories);

            md.album = QString::fromUtf8(album.toStdString().c_str());
            md.artist = QString::fromUtf8(author.toStdString().c_str());

			QDomNodeList item_content_nodes = channel_content_node.childNodes();

			for(int i=0; i<item_content_nodes.size(); i++)
            {
				QDomNode item_node = item_content_nodes.at(i);
				QString item_nodename = item_node.nodeName();
				QDomElement item_element = item_node.toElement();
				QString item_element_text = item_element.text();

				if(!item_nodename.compare("title", Qt::CaseInsensitive)) {
					md.title = QString::fromUtf8(item_element_text.toStdString().c_str());
					md.add_custom_field("1title", "Title", md.title);
                }

				else if(!item_nodename.compare("description", Qt::CaseInsensitive)){
					QString text = QString::fromUtf8(item_element_text.toStdString().c_str());
					md.add_custom_field("2desciption", "Description", text);
				}

				else if(!item_nodename.compare("enclosure", Qt::CaseInsensitive)){

					QDomNamedNodeMap map = item_element.attributes();
					for(int j=0; j<map.count(); j++){
						QDomAttr attr = map.item(j).toAttr();
						if(attr.name().compare("url", Qt::CaseInsensitive) == 0){
							md.set_filepath(attr.value());
						}
					}
				}

				else if(!item_nodename.compare("link", Qt::CaseInsensitive)) {
					fallback_url = item_element.text();
                }

				else if(!item_nodename.compare("enclosure", Qt::CaseInsensitive)) {
					md.filesize = item_element.attribute("length").toLong();
                }

				else if(!item_nodename.compare("author") && md.artist.size() == 0) {
					md.artist = QString::fromUtf8(item_element_text.toStdString().c_str());
                }

				else if(!item_nodename.compare("itunes:author", Qt::CaseInsensitive)) {
					md.artist = QString::fromUtf8(item_element_text.toStdString().c_str());
                }

				else if(!item_nodename.compare("itunes:duration", Qt::CaseInsensitive)) {

					QStringList lst = item_element_text.split(":");
                    int len = 0;
					for(int j=lst.size() -1; j>=0; j--) {
						if(j == lst.size() -1)
							len += lst[j].toInt();
						else if(j == lst.size() -2) {
							len += lst[j].toInt() * 60;
                        }
						else if(j == lst.size() -3) {
							len += lst[j].toInt() * 3600;
                        }
                    }

                    md.length_ms = len * 1000;
				} // curation

				else if(!item_nodename.compare("pubDate", Qt::CaseInsensitive) ||
						!item_nodename.compare("dc:date", Qt::CaseInsensitive)) {
					md.year = find_year(item_element_text);
                }

				else if(!item_nodename.compare("psc:chapters", Qt::CaseInsensitive)){

					QDomNodeList chapter_nodes = item_node.childNodes();

					for(int cn=0; cn<chapter_nodes.size(); cn++)
					{
						QDomNode chapter_node = chapter_nodes.at(cn);
						QDomElement chapter_element = chapter_node.toElement();
						QDomNamedNodeMap chapter_attributes = chapter_element.attributes();
						QString chapter_info, chapter_key;
						QString title, length_str;

						for(int cha=0; cha<chapter_attributes.count(); cha++){
							QDomAttr attr = chapter_attributes.item(cha).toAttr();
							
							if(attr.name().compare("start", Qt::CaseInsensitive) == 0){
								int length = parse_length_s(attr.value());
								length_str = QString::number(length);
							}

							else if(attr.name().compare("title", Qt::CaseInsensitive) == 0){
								title = attr.value();
							}
						}

						if(title.isEmpty() || length_str.isEmpty()){
							continue;
						}

						n_chapters++;

						chapter_info = length_str + ":" + title;
						chapter_key = QString("Chapter") + QString::number(n_chapters);

						md.add_custom_field(chapter_key, chapter_key, chapter_info);

					} // chapter

				} //chapters

			} // item_content_nodes

			if(md.filepath().isEmpty()){
				md.set_filepath(fallback_url);
			}

			md.cover_download_url = cover_url;

			if( !md.filepath().isEmpty() ){
				v_md << std::move(md);
			}
        } // item
    }

    return (v_md.size() > 0);
}


