/* LyricLookupThread.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * LyricLookupThread.cpp
 *
 *  Created on: May 21, 2011
 *      Author: Lucio Carreras
 */

#include "LyricLookup.h"
#include "LyricServer.h"

#include "Helper/Helper.h"
#include "Helper/WebAccess/AsyncWebAccess.h"

#include <QRegExp>
#include <QThread>
#include <algorithm>

size_t first_appearance = -1;
size_t last_appearance = -1;
QStringList lst;


LyricLookupThread::LyricLookupThread(QObject* parent) :
	QObject(parent)
{
	init_server_list();
	_cur_server = 0;
	_final_wp.clear();


	_regex_conversions.insert("$", "\\$");
	_regex_conversions.insert("*", "\\*");
	_regex_conversions.insert("+", "\\+");
	_regex_conversions.insert("?", "\\?");
	_regex_conversions.insert("[", "\\[");
	_regex_conversions.insert("]", "\\]");
	_regex_conversions.insert("(", "\\(");
	_regex_conversions.insert(")", "\\)");
	_regex_conversions.insert("{", "\\{");
	_regex_conversions.insert("}", "\\}");
	_regex_conversions.insert("^", "\\^");
	_regex_conversions.insert("|", "\\|");
	_regex_conversions.insert(".", "\\.");

}

LyricLookupThread::~LyricLookupThread() {
}

QString LyricLookupThread::convert_to_regex(const QString& str){
	QString ret = str;

	for(QString key : _regex_conversions.keys()){
		ret.replace(key, _regex_conversions.value(key));
	}

	ret.replace(" ", "\\s+");

	return ret;
}

QString LyricLookupThread::calc_url(QString artist, QString song) {

	if(_cur_server < 0 || _cur_server >= _server_list.size()){
		return "";
	}

	QMap<QString, QString> replacements = _server_list[_cur_server].replacements;

	for(QString key : replacements.keys()) {
		while(artist.indexOf(key) >= 0){
			artist.replace(key, replacements.value(key));
		}

		while(song.indexOf(key) >= 0){
			song.replace(key, replacements.value(key));
		}
	}

	QString url = _server_list[_cur_server].call_policy;
	url.replace("<SERVER>", _server_list[_cur_server].server_address);
	url.replace("<FIRST_ARTIST_LETTER>", QString(artist[0]).trimmed());
	url.replace("<ARTIST>", artist.trimmed());
	url.replace("<TITLE>", song.trimmed());

	if(_server_list[_cur_server].to_lower){
		return url.toLower();
	}

	return url;
}

QString LyricLookupThread::parse_webpage(const QByteArray& raw) {

	QString dst(raw);

	ServerTemplate t = _server_list[_cur_server];

	for(QString start_tag : t.start_end_tag.keys()) {

		QString content;
		QString end_tag;

		end_tag = t.start_end_tag.value(start_tag);

		start_tag = convert_to_regex(start_tag);
		if(start_tag.startsWith("<") && !start_tag.endsWith(">")){
			start_tag.append(".*>");
		}

		end_tag = convert_to_regex(end_tag);

		QRegExp regex;
		regex.setMinimal(true);
		regex.setPattern(start_tag + "(.+)" + end_tag);
		if(regex.indexIn(dst) != -1){
			content  = regex.cap(1);
		}

		if(content.isEmpty()){
			continue;
		}



		QRegExp re_script;
		re_script.setPattern("<script.+</script>");
		re_script.setMinimal(true);
		while(re_script.indexIn(content) != -1){
			content.replace(re_script, "");
		}

		QString word;
		if(t.is_numeric) {
			QRegExp rx("&#(\\d+);|<br />|</span>|</p>");

			QStringList tmplist;;
			int pos = 0;
			while ((pos = rx.indexIn(content, pos)) != -1) {
				QString str = rx.cap(1);

				pos += rx.matchedLength();
				if(str.size() == 0) {
					tmplist.push_back(word);
					word = "";
					tmplist.push_back("<br />");
				}

				else{
					word.append(QChar(str.toInt()));
				}
			}

			dst = "";

			for(const QString& str : tmplist) {
				dst.append(str);
			}
		}

		else{
			dst = content;
		}

		dst.replace("\n", "<br />");
		dst.replace("\\n", "<br />");

		if(dst.size() > 100){
			break;
		}
	}

	return dst;
}

void LyricLookupThread::run(const QString& artist, const QString& title, int server_idx){

	_artist = artist;
	_title = title;

	_cur_server = std::max(0, server_idx);
	_cur_server = std::min(server_idx, _server_list.size() - 1);

	if(_artist.isEmpty() && _title.isEmpty()) {
		_final_wp = "No track selected";
		return;
	}

	_final_wp.clear();

	QString url = this->calc_url(_artist, _title);

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished, this, &LyricLookupThread::content_fetched);
	awa->run(url);

}

void  LyricLookupThread::content_fetched(bool success){

	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	QString url = awa->get_url();

	if(!success){
		_final_wp = tr("Sorry, could not fetch lyrics from %1").arg(awa->get_url());
		emit sig_finished();
		return;
	}

	_final_wp = parse_webpage(awa->get_data());

	if ( _final_wp.isEmpty() ) {
		_final_wp = tr("Sorry, no lyrics found") + "<br />" + url;
		emit sig_finished();
		return;
	}

	_final_wp.push_front(_server_list[_cur_server].display_str + "<br /><br />");
	_final_wp.push_front(awa->get_url() + "<br /><br />");
	_final_wp.push_front(QString("<font size=\"5\" color=\"#F3841A\"><b>") +
						 _artist + QString(" - ") +
						 _title +
						 "</b></font><br /><br />");

	emit sig_finished();
}

void LyricLookupThread::init_server_list() {

	/*parse_xml();
	return;*/

	// motörhead
	// crosby, stills & nash
	// guns 'n' roses
	// AC/DC
	// the doors
	// the rolling stones
	// petr nalitch
	// eric burdon and the animals
	// Don't speak


	ServerTemplate wikia;
	wikia.display_str = "Wikia.com";
	wikia.server_address = QString("http://lyrics.wikia.com");
	wikia.addReplacement(" ", "_");
	wikia.addReplacement("&", "%26");
	wikia.call_policy = QString("<SERVER>/wiki/<ARTIST>:<TITLE>");
	wikia.start_end_tag.insert("<div class='lyricbox'>", "<!--");
	wikia.include_start_tag = false;
	wikia.include_end_tag = false;
	wikia.is_numeric = true;
	wikia.to_lower = false;
	wikia.error = "this page needs content";


	ServerTemplate oldieLyrics;
	oldieLyrics.display_str = "OldieLyrics.com";
	oldieLyrics.server_address = QString("http://www.oldielyrics.com/lyrics");
	oldieLyrics.addReplacement(" ", "_");
	oldieLyrics.addReplacement("(", "_");
	oldieLyrics.addReplacement(")", "_");
	oldieLyrics.addReplacement(".", "_");
	oldieLyrics.addReplacement("&", "_");
	oldieLyrics.addReplacement("'", "");
	oldieLyrics.addReplacement("__", "_");
	oldieLyrics.call_policy = QString("<SERVER>/<ARTIST>/<TITLE>.html");
	oldieLyrics.start_end_tag.insert("&#", "</div>");
	oldieLyrics.include_start_tag = true;
	oldieLyrics.include_end_tag = false;
	oldieLyrics.is_numeric = true;
	oldieLyrics.to_lower = true;
	oldieLyrics.error = QString("error 404");


	ServerTemplate lyricskeeper;
	lyricskeeper.display_str = "lyrics keeper";
	lyricskeeper.server_address = QString("http://lyrics-keeper.com");
	lyricskeeper.addReplacement("&", "");
	lyricskeeper.addReplacement(" ", "-");
	lyricskeeper.addReplacement("'", "-");
	lyricskeeper.addReplacement("--", "-");
	lyricskeeper.call_policy = QString("<SERVER>/en/<ARTIST>/<TITLE>.html");
	lyricskeeper.start_end_tag.insert("<div id=\"lyrics\">", "</div>");
	lyricskeeper.include_start_tag = false;
	lyricskeeper.include_end_tag = false;
	lyricskeeper.is_numeric = false;
	lyricskeeper.to_lower = true;
	lyricskeeper.error = QString("page cannot be found");



	ServerTemplate metrolyrics;
	metrolyrics.display_str = "MetroLyrics.com";
	metrolyrics.server_address = QString("http://www.metrolyrics.com");
	metrolyrics.addReplacement("&", "and");
	metrolyrics.addReplacement(" ", "-");
	metrolyrics.call_policy = QString("<SERVER>/<TITLE>-lyrics-<ARTIST>.html");
	//metrolyrics.start_end_tag.insert("<span class='line line-s' id='line_1'>", "</p>");
	metrolyrics.start_end_tag.insert ("<div id=\"lyrics-body-text\" class=\"js-lyric-text\">", "</div>");
	metrolyrics.include_start_tag = false;
	metrolyrics.include_end_tag = false;
	metrolyrics.is_numeric = false;
	metrolyrics.to_lower = true;
	metrolyrics.error = QString("404 page not found");


	ServerTemplate elyrics;
	elyrics.display_str = "eLyrics";
	elyrics.server_address= QString("http://www.elyrics.net/read");
	elyrics.addReplacement(" ", "-");
	elyrics.addReplacement("the ", "");
	elyrics.addReplacement("The ", "");
	elyrics.addReplacement("'", "_");
	elyrics.call_policy = QString("<SERVER>/<FIRST_ARTIST_LETTER>/<ARTIST>-lyrics/<TITLE>-lyrics.html");

	elyrics.start_end_tag.insert("lyrics</strong><br>", "</div>");
	elyrics.start_end_tag.insert("<div id='inlyr' style='font-size:14px;'>", "</div>");

	elyrics.include_start_tag = false;
	elyrics.include_end_tag = false;
	elyrics.is_numeric = false;
	elyrics.to_lower = true;
	elyrics.error = QString("Error 404");


	ServerTemplate golyr;
	golyr.display_str = "Golyr";
	golyr.server_address = "http://www.golyr.de";
	golyr.call_policy = "<SERVER>/<ARTIST>/songtext-<TITLE>";
	golyr.addReplacement("'", "-");
	golyr.addReplacement(" ", "-");
	golyr.addReplacement("(", "");
	golyr.addReplacement(")", "");
	golyr.start_end_tag.insert("<div id=\"lyrics\">", "</div> <div class=\"fads\"");
	golyr.include_end_tag = false;
	golyr.is_numeric = false;
	golyr.to_lower = false;
	golyr.error = "404 Not Found";

	ServerTemplate musixmatch;
	musixmatch.display_str = "Musixmatch";
	musixmatch.server_address = "https://www.musixmatch.com/";
	musixmatch.call_policy = "<SERVER>/lyrics/<ARTIST>/<TITLE>";
	musixmatch.addReplacement(" ", "-");
	musixmatch.addReplacement("'", "");
	musixmatch.addReplacement("/", "-");
	musixmatch.addReplacement(".", "-");
	musixmatch.addReplacement("&", "-");
	musixmatch.addReplacement("--", "-");
	musixmatch.start_end_tag.insert("<div id=\"selectable-lyrics\"", "</span><span data-reactid");
	musixmatch.start_end_tag.insert("<p class=.*content", "</p>");
	musixmatch.start_end_tag.insert("\"body\":\"", "\",\"");

	musixmatch.include_end_tag = false;
	musixmatch.is_numeric = false;
	musixmatch.to_lower = false;
	musixmatch.error = "404 Not Found";

	_server_list.push_back(wikia);
	_server_list.push_back(musixmatch);
	_server_list.push_back(metrolyrics);
	_server_list.push_back(oldieLyrics);
	_server_list.push_back(lyricskeeper);
	_server_list.push_back(elyrics);
	_server_list.push_back(golyr);



	/*sp_log(Log::Info) << "Servers: [";
	for(const ServerTemplate& t : _server_list){
		t.print_json();
		sp_log(Log::Info) << ",";
	}
	sp_log(Log::Info) << "]";*/

}

QStringList LyricLookupThread::get_servers() {
	QStringList lst;
	for(const ServerTemplate& t : _server_list) {
		lst << t.display_str;
	}

	return lst;
}

QString LyricLookupThread::get_lyric_data() {
	return _final_wp;
}



void LyricLookupThread::parse_xml(){

	QFile f( Helper::get_sayonara_path() + "/bla.xml");
	QDomDocument doc("doc_name");
	doc.setContent(&f);

	QDomNodeList lst = doc.childNodes();

	for(int i=0; i<lst.size(); i++){

		QDomNode child = lst.item(i); //ServerTemplate
		QDomNodeList entries = child.childNodes();
		ServerTemplate t;


		for(int j=0; j<entries.size(); j++){

			QDomNode entry_node = entries.at(j);
			QString node_name = entry_node.nodeName();
			QString data = entry_node.firstChild().toText().data();
			if(data.startsWith("\"")){
				data.remove(0, 1);
			}

			if(data.endsWith("\"")){
				data.remove(data.size() - 1, 1);
			}

			if(node_name.compare("name") == 0){

				t.display_str = data;
			}

			if(node_name.compare("server_address") == 0){
				t.server_address = data;
			}

			if(node_name.compare("call_policy") == 0){
				t.call_policy = data;
			}

			/*if(node_name.compare("start_tag") == 0){
				t.start_tag = data;
			}

			if(node_name.compare("end_tag") == 0){
				t.end_tag = data;
			}*/

			if(node_name.compare("include_start_tag") == 0){
				t.include_start_tag = (data == "true");
			}

			if(node_name.compare("include_end_tag") == 0){
				t.include_end_tag = (data == "true");
			}

			if(node_name.compare("is_numeric") == 0){
				t.is_numeric = (data == "true");
			}

			if(node_name.compare("to_lower") == 0){
				t.to_lower  = (data == "true");
			}

			if(node_name.compare("error") == 0){
				t.error = data;
			}

			if(node_name.compare("replacement") == 0){
				QDomNodeList repl_children = entry_node.childNodes();
				QString from, to;

				for(int repl_child_idx=0; repl_child_idx < repl_children.size(); repl_child_idx++){
					QDomNode n = repl_children.at(repl_child_idx);
					if(n.nodeName().compare("from") == 0){
						from = n.firstChild().toText().data();
					}

					else if(n.nodeName().compare("to") == 0){
						to = n.firstChild().toText().data();
					}

					t.replacements[from] = to;

				}
			}

		}

		_server_list << t;
	}

	f.close();
	f.remove();
}
