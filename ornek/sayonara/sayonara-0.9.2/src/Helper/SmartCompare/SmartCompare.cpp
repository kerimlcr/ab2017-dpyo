/* SmartCompare.cpp */

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



#include "Helper/SmartCompare/SmartCompare.h"
#include "Helper/Logger/Logger.h"
#include "Helper/MetaData/Artist.h"

#include "Database/DatabaseConnector.h"

#include <QFile>
#include <limits>

using namespace std;

SmartCompare::SmartCompare()
{

	_vocals << 'a' << 'e' << 'i' << 'o' << 'u';
	_consonants << 'b' << 'c' << 'd' << 'f' <<
				   'g' << 'h' << 'j' << 'k' <<
				   'l' << 'm' << 'n' << 'p' <<
				   'q' << 'r' << 's' << 't' <<
				   'v' << 'w' << 'x' << 'y' << 'z';


	_numbers << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9' << '0';
}

SmartCompare::SmartCompare(const QStringList &strlist) :
	SmartCompare()
{
	_strlist = strlist;
}

SmartCompare::SmartCompare(const ArtistList &artist_list) :
	SmartCompare()
{
	for(auto artist = artist_list.begin(); artist != artist_list.end(); artist++){
		_strlist << artist->name;
	}
}

QString revert_string(const QString& str){

	QString ret = "";
	if(str.size() == 0) return "";

	for(int i=str.size()-1; i>=0; i--){
		ret.append(str[i]);
	}

	return ret;
}


float SmartCompare::compare_hashes(const Hash& str1, const Hash& str2, HashLevel level, bool reverse){

	Q_UNUSED(level)
	QString s1 = str1.hash;
	QString s2 = str2.hash;

	if(reverse){
		s1 = revert_string(str1.hash);
		s2 = revert_string(str2.hash);
	}

	int min_str_size = min(str1.org_str.size(), str2.org_str.size());
	if(min_str_size == 0) return 0;

	int min_hash_size = min(str1.hash.size(), str2.hash.size());
	int max_hash_size = max(str1.hash.size(), str2.hash.size());
	if(min_hash_size == 0) return 0;

	float similarity = 0;

	int bad_letters = 0;
	float rate_bad_letters[2];

	int right_letters = 0;
	float rate_right_letters[2];

	int consec_letters = 0;
	float rate_consec_letters[2];

	int idx1 = 0;
	int idx2 = 0;

	float rate = 1.0f;

	QMap<int, int> pairs;

	while(true){

		QChar ncl = s1.at(idx1);
		bool found = false;
		int wrong2letters=0;

		for(int i=idx2; i<s2.size(); i++){
			QChar c = s2.at(i);
			if(c == ncl){
				found = true;
				pairs.insert(idx1, idx2);

				if(i == idx2){
					consec_letters++;
					rate *= 1.5f;
				}


				right_letters++;

				idx2 = i;
				break;
			}


			if(i==idx2 && rate > 1.0f){
				rate /= 1.5f;
			}

			wrong2letters++;
		}

		if(!found){
			bad_letters++;
		}

		else {
			bad_letters += wrong2letters;
			idx2 ++;
		}

		idx1++;

		if(idx1 == s1.size()) break;
		if(idx2 == s2.size()) break;
	}


	bad_letters += s1.size() - idx1;
	bad_letters += s2.size() - idx2;

	rate_consec_letters[0] = (consec_letters * 1.0f) / (min_hash_size * 1.0f);
	rate_right_letters[0] = (right_letters * 1.0f) / (min_hash_size * 1.0f);
	rate_bad_letters[0] = (bad_letters * 1.0f) / (min_hash_size * 1.0f);

	rate_consec_letters[1] = (consec_letters * 1.0f) / (max_hash_size * 1.0f);
	rate_right_letters[1] = (right_letters * 1.0f) / (max_hash_size * 1.0f);
	rate_bad_letters[1] = (bad_letters * 1.0f) / (max_hash_size * 1.0f);


	similarity = 0;
	for(int i=0; i<2; i++){
		similarity += (1.0f - rate_bad_letters[i]) * 5.0f;
		similarity += rate_right_letters[i] * 1.0f;
		similarity += rate_consec_letters[i] * 10.0f * rate;
	}

	similarity /= (32.0f);
	//similarity *= similarity;

//	similarity = pow(similarity, str1.penalty);

	/*if(bad_letters < right_letters || bad_letters < consec_letters){
		sp_log(Log::Debug) << "";
		str1.print();
		str2.print();
		sp_log(Log::Debug) << "Idx1 = " << idx1 << "/" << s1.size() << ", idx2 = " << idx2 << "/" << s2.size();
		sp_log(Log::Debug) << "Bad Letters: " << bad_letters;
		sp_log(Log::Debug) << "Right letters: " <<  right_letters;
		sp_log(Log::Debug) << "Conseq letter: " << consec_letters;
		sp_log(Log::Debug) << "";
	}

	else return 0;*/

	return similarity;
}



float SmartCompare::compare(const QString& str1, const QString& str2, HashLevel level, bool revert){

	Hash hash1 = create_hash(str1, level);
	Hash hash2 = create_hash(str2, level);

	if(str1.compare(str2) == 0) return 1000.0f;

	float similarity = compare_hashes(hash1, hash2, level, revert);
	float similarity2 = compare_hashes(hash2, hash1, level, revert);

	return max(similarity,similarity2);
}

Hash SmartCompare::create_hash(const QString& str, HashLevel level){

	Hash hash;

	hash.org_str = str;
	hash.hash = str.toLower();

	if(level >= HashLevel::zero){
		hash.penalty = 1.0f;

		hash.hash = remove_special_chars(hash.hash);
	}

	if(level > HashLevel::zero){
		hash.penalty = 5.0f;
		hash.hash = remove_vocals(hash.hash);

	}

	else if(level > HashLevel::medium){
		hash.penalty = 8.0f;
		hash.hash = sort_letters(hash.hash);
	}

	return hash;
}



void SmartCompare::equalize(QMap<QString, float>& map, float min, float max){

	float stretch = 1.0f / (max - min);
	QStringList to_remove;

	for(const QString& key : map.keys()){
		float val = map.value(key);

		val = (val - min) * stretch;
		if(val > 0.5f){
			map[key] = val;
		}

		else{
			to_remove << key;
		}
	}

	for(const QString& str : to_remove){
		map.remove(str);
	}
}

QMap<QString, float> SmartCompare::get_similar_strings( const QString& str){

	HashLevel level = HashLevel::zero;
	if(_cache.contains(str)){
		return _cache[str];
	}

	bool reverse = false;

	while(true){

		float max = std::numeric_limits<float>::min();
		float min = std::numeric_limits<float>::max();

		QMap<QString, float> map;

		for(const QString& tmp_str : _strlist){

			float similarity = compare(tmp_str, str, level, false);
			similarity = std::max(similarity, compare(tmp_str, str, level, true));

			if(similarity > max){
				max = similarity;
			}

			if(similarity < min){
				min = similarity;
			}

			if(similarity > 5.0f){
				map.insert(tmp_str, similarity);
			}
		}

		if( max <  5.0f && reverse == false){
			reverse = true;
			continue;
		}


		else if(max < 1.0f && level < HashLevel::strong){
			if(level == HashLevel::zero) level = HashLevel::medium;
			if(level == HashLevel::medium) level = HashLevel::strong;
			reverse = false;

			continue;
		}

		else {
			//sp_log(Log::Debug) << "LEvel : " << level;
			//equalize(map, min, max);
			_cache[str] = map;
			return map;
		}
	}
}

QString SmartCompare::remove_special_chars(const QString& str){

	QString ret = str;

	for(const QChar& c : ret){
		if(_vocals.contains(c)) continue;
		if(_consonants.contains(c)) continue;
		if(_numbers.contains(c)) continue;

		ret = ret.remove(c);
	}

	return ret;
}

QString SmartCompare::remove_vocals(const QString& str){

	QString ret = str;
	for(const QString& vocal :_vocals){
		ret.remove(vocal);
	}

	return ret;
}

QString SmartCompare::sort_letters(const QString& str){

	QString ret = "";
	QMap<QChar, int> cons_count;
	QMap<QChar, int> vocal_count;
	QMap<QChar, int> number_count;

	for(const QChar& c : str){

		if(_vocals.contains(c)){
			if( vocal_count.contains(c) ){
				int count = vocal_count.value(c);
				count += 1;
				vocal_count[c] = count;
			}

			else{
				vocal_count.insert(c, 1);
			}
		}

		else if(_consonants.contains(c)){
			if( cons_count.contains(c) ){
				int count = cons_count.value(c);
				count += 1;
				cons_count[c] = count;
			}

			else{
				cons_count.insert(c, 1);
			}
		}

		else if(_numbers.contains(c)){
			if( number_count.contains(c) ){
				int count = number_count.value(c);
				count += 1;
				number_count[c] = count;
			}

			else{
				number_count.insert(c, 1);
			}
		}
	}

	for(const QChar& c : vocal_count.keys()){
		int count = vocal_count[c];

		for(int i=0; i<count; i++){
			ret += c;
		}
	}

	for(const QChar& c : cons_count.keys()){
		int count = cons_count[c];

		for(int i=0; i<count; i++){
			ret += c;
		}
	}

	for(const QChar& c : number_count.keys()){
		int count = number_count[c];

		for(int i=0; i<count; i++){
			ret += c;
		}
	}

	return ret;
}


void Hash::print() const{
	sp_log(Log::Info) << org_str << " -> " << hash << ": " << penalty;
}
