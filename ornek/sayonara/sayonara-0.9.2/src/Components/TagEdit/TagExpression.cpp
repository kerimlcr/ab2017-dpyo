/* TagExpression.cpp */

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



#include "TagExpression.h"
#include "Helper/FileHelper.h"
#include <QRegExp>
#include <QMap>
#include "Helper/Logger/Logger.h"

TagExpression::TagExpression(){
	_valid = false;

	_tag_regex_map.insert(TAG_TITLE, QString("(.+)"));
	_tag_regex_map.insert(TAG_ALBUM, QString ("(.+)"));
	_tag_regex_map.insert(TAG_ARTIST, QString("(.+)"));
	_tag_regex_map.insert(TAG_TRACK_NUM, QString("(\\d+)"));
	_tag_regex_map.insert(TAG_YEAR, QString("(\\d{4})"));
	_tag_regex_map.insert(TAG_DISC, QString("(\\d{1,3})"));
	_tag_regex_map.insert(TAG_IGNORE, QString("(.+)"));
}

TagExpression::TagExpression(const QString& tag_str, const QString& filepath) :
	TagExpression()
{
	update_tag(tag_str, filepath);
}

TagExpression::~TagExpression(){

}


QString TagExpression::escape_special_chars(const QString& str) const
{

	QString s = str;
	QStringList str2escape;
	str2escape << "\\" << "?" << "+" << "*" << "[" << "]" << "(" << ")" << "{" << "}" << "."; //<< "-";

	for(const QString& c : str2escape){
		s.replace(c, QString("\\") + c);
	}

	return s;
}

QStringList TagExpression::split_tag_string( const QString& tag_str ) const
{

	QStringList splitted_tag_str;
	QString tag_str_tmp = escape_special_chars(tag_str);

	QMap<int, QString> tags;

	QStringList available_tags;
	available_tags  << TAG_TITLE
					<< TAG_ALBUM
					<< TAG_ARTIST
					<< TAG_TRACK_NUM
					<< TAG_YEAR
					<< TAG_DISC;


	// search for the tags in tag_str
	for(const QString& tag : available_tags){
		int idx = tag_str_tmp.indexOf(tag);
		if(idx >= 0){
			tags.insert(idx, tag);
		}
	}

	int cur_idx = 0;


	for(int idx : tags.keys()){

		int len = idx - cur_idx;

		splitted_tag_str << tag_str_tmp.mid(cur_idx, idx - cur_idx);
		splitted_tag_str << tags[idx];

		cur_idx += (tags[idx].size() + len);
	}

	splitted_tag_str << tag_str_tmp.right(tag_str_tmp.length() - cur_idx);
	splitted_tag_str.removeAll("");

	return splitted_tag_str;
}



QString TagExpression::calc_regex_string(const QStringList& splitted_str) const
{

	QString re_str;

	for(const QString& s : splitted_str){

		if(s.isEmpty()) continue;


		if( _tag_regex_map.contains(s) ){
			re_str += _tag_regex_map[s];
		}

		else{
			re_str += "(" + s + ")";
		}
	}

	return re_str;
}



bool TagExpression::update_tag(const QString& tag_str, const QString& filepath){

	_cap_map.clear();

	bool valid;
	QStringList captured_texts;
	QStringList splitted_tag_str = split_tag_string(tag_str);
	QString regex = calc_regex_string(splitted_tag_str);

	QRegExp re( regex );

	int n_tags, n_caps;

	re.indexIn( filepath );

	captured_texts = re.capturedTexts();
	captured_texts.removeAt(0);
	captured_texts.removeAll("");

	n_caps = captured_texts.size();
	n_tags = splitted_tag_str.size();

	valid = (n_caps == n_tags);

	if( !valid ){

		sp_log(Log::Warning) << regex;
		sp_log(Log::Warning) <<  n_caps << " tags found, but requested " << n_tags;
		sp_log(Log::Warning) << "Caps: ";
		sp_log(Log::Warning) << "";

		for(const QString& s : captured_texts){
			sp_log(Log::Warning) << "  " << s;
		}

		sp_log(Log::Warning) << "";
	}

	else{

		for(int i=0; i<n_caps; i++){

			Tag tag = splitted_tag_str[i];
			QString cap = captured_texts[i];

			if(i==0){
				QString dir, filename;
				Helper::File::split_filename(cap, dir, filename);
				cap = filename;
			}


			_cap_map[tag] = cap;
		}
	}

	return valid;
}

bool TagExpression::check_tag(const Tag& tag, const QString& str){


	if(!_tag_regex_map.contains(tag)) {
		return false;
	}

	QString escape_str = escape_special_chars(str);
	QRegExp re(_tag_regex_map[tag]);

	if(re.indexIn(escape_str) != 0 ) {
		return false;
	}

	return true;
}

QMap<Tag, ReplacedString> TagExpression::get_tag_val_map() const
{
	return _cap_map;
}
