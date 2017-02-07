/* LyricServer.h */

/* Copyright (C) 2012  Lucio Carreras
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


#ifndef LYRICSERVER_H_
#define LYRICSERVER_H_

#include <QString>
#include <QMap>
#include "Helper/Logger/Logger.h"

/**
 * @brief The ServerTemplate struct
 * @ingroup Lyrics
 */
struct ServerTemplate {

	QString display_str;
	QString server_address;
	QMap<QString, QString> replacements;
	QString call_policy;
	QMap<QString, QString> start_end_tag;
	bool include_start_tag;
	bool include_end_tag;
	bool is_numeric;
	bool to_lower;
	QString error;

	void addReplacement(QString rep, QString rep_with){
		replacements[rep] = rep_with;
	}

#define STR_TRUE QString("true")
#define STR_FALSE QString("false")

	void print_xml() const
	{

		sp_log(Log::Info) << "<ServerTemplate>";
		sp_log(Log::Info) << "  <name>\"" << display_str << "\"</name>";
		sp_log(Log::Info) << "  <server_address>\"" << server_address << "\"</server_address>";
		sp_log(Log::Info) << "  <call_policy>\"" << call_policy << "\"</call_policy>";
		/*sp_log(Log::Info) << "  <start_tag>\"" << start_tag << "\"</start_tag>";
		sp_log(Log::Info) << "  <end_tag>\"" << end_tag << "\"</end_tag>";*/
		sp_log(Log::Info) << "  <include_start_tag>" << (include_start_tag ? STR_TRUE : STR_FALSE) << "</include_start_tag>";
		sp_log(Log::Info) << "  <include_end_tag>" << (include_end_tag ? STR_TRUE : STR_FALSE) << "</include_end_tag>";
		sp_log(Log::Info) << "  <is_numeric>" << (is_numeric ? STR_TRUE : STR_FALSE) << "</is_numeric>";
		sp_log(Log::Info) << "  <to_lower>" << (to_lower ? STR_TRUE : STR_FALSE) << "</to_lower>";
		sp_log(Log::Info) << "  <error>\"" << error << "\"</error>";

		for(QString key : replacements.keys()){
			sp_log(Log::Info) << "  <replacement>";
			sp_log(Log::Info) << "    <from>\"" << key << "\"</from>";
			sp_log(Log::Info) << "    <to>\"" << replacements[key] << "\"</to>";
			sp_log(Log::Info) << "  </replacement>";
		}

		sp_log(Log::Info) << "</ServerTemplate>";
	}

	void print_json() const
	{
		sp_log(Log::Info) << "  {";
		sp_log(Log::Info) << "    \"ServerName\": \"" + display_str + "\",";
		sp_log(Log::Info) << "    \"ServerAddress\": \"" + server_address + "\",";
		sp_log(Log::Info) << "    \"CallPolicy\": \"" + call_policy + "\",";
		sp_log(Log::Info) << "    \"IncludeStartTag\": " + QString::number(include_start_tag) + ",";
		sp_log(Log::Info) << "    \"IncludeEndTag\": " + QString::number(include_end_tag) + ",";
		sp_log(Log::Info) << "    \"IsNumeric\": " + QString::number(is_numeric) + ",";
		sp_log(Log::Info) << "    \"ToLower\": " + QString::number(to_lower) + ",";
		sp_log(Log::Info) << "    \"Error\": \"" + error + "\",";

		sp_log(Log::Info) << "    \"Replacements\": [";

		for(const QString& str : replacements.keys()){
			sp_log(Log::Info) << "      {";
			sp_log(Log::Info) << "        \"OrgString\": \"" + str + "\",";
			sp_log(Log::Info) << "        \"RepString\": \"" + replacements[str] + "\"";
			sp_log(Log::Info) << "      },";
		}

		sp_log(Log::Info) << "    ]";

		sp_log(Log::Info) << "    \"BorderTags\": [";
		for(const QString& str : start_end_tag.keys()){
			QString key = str;
			key.replace("\"", "\\\"");
			QString value = start_end_tag[str];
			value.replace("\"", "\\\"");

			sp_log(Log::Info) << "      {";
			sp_log(Log::Info) << "        \"StartTag\": \"" + key + "\",";
			sp_log(Log::Info) << "        \"EndTag\": \"" + value + "\"";
			sp_log(Log::Info) << "      },";
		}

		sp_log(Log::Info) << "    ]";
		sp_log(Log::Info) << "  }";
	}

};

#endif /* LYRICSERVER_H_ */
