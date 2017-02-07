/* ArtistInfo.cpp */

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


#include "ArtistInfo.h"

#include "Database/LibraryDatabase.h"
#include "Helper/Compressor/Compressor.h"

#include "Helper/MetaData/Artist.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"

ArtistInfo::ArtistInfo(const MetaDataList& v_md) :
	MetaDataInfo(v_md){

	insert_number(InfoStrings::nAlbums, _albums.size());

	// clear, because it's from Metadata. We are not interested in these
	// rather fetch artists' additional data, if there's only one artist
	_additional_info.clear();

	if(_artist_ids.size() == 1){
		Artist artist;
		bool success;

		int artist_id = _artist_ids.first();
		success = _db->getArtistByID(artist_id, artist);

		if(success){
			_additional_info.clear();
			calc_similar_artists(artist);
			// custom fields
			const QList<CustomField>& custom_fields = artist.get_custom_fields();
			for(const CustomField& field : custom_fields){

				QString name = field.get_display_name();
				QString value = field.get_value();
				if(value.isEmpty()){
					continue;
				}

				_additional_info[name] += field.get_value();
			}
		}
	}

	else if(_artists.size() > 1){
		insert_number(InfoStrings::nArtists, _artists.size());
	}


	set_header();
	set_subheader();
	set_cover_location();
}

ArtistInfo::~ArtistInfo(){}

void ArtistInfo::set_header(){
	_header = calc_artist_str();

}

void ArtistInfo::calc_similar_artists(Artist& artist)
{
	QString sim_path = Helper::get_sayonara_path() + "/similar_artists/" + artist.name  + ".comp";
	if(!QFile::exists(sim_path)){
		return;
	}

	QByteArray content, decomp;
	bool success = Helper::File::read_file_into_byte_arr(sim_path, content);
	if(!success){
		return;
	}

	decomp = Compressor::decompress(content);
	if(decomp.isEmpty()){
		return;
	}

	QStringList sim_artists  = QString::fromLocal8Bit(decomp).split("\n");
	for(const QString& sim_artist : sim_artists){
		QStringList lst = sim_artist.split("\t");
		if(lst.size() < 3){
			continue;
		}

		QString match = lst[0];
		QString artist_name = lst[2];

		artist.add_custom_field("sim_artist_" + artist_name,
								"sim_artist_" + match + artist_name,
								artist_name);
	}
}

void ArtistInfo::set_subheader(){
	_subheader = "";
}

void ArtistInfo::set_cover_location(){

	if( _artists.size() == 1){
		QString artist = _artists.first();
		_cover_location = CoverLocation::get_cover_location(artist);
	}

	else{
		_cover_location = CoverLocation::getInvalidLocation();
	}
}


QString ArtistInfo::get_cover_album() const
{
	return "";
}


QString ArtistInfo::get_additional_info_as_string() const
{
	QString str;
	QStringList sim_artists;
	for(const QString& key : _additional_info.keys()){

		if(key.startsWith("sim_artist_")){
			sim_artists << key;
		}

		else {
			str += BOLD(key) + ": " + _additional_info[key] + CAR_RET;
		}
	}

	std::sort(sim_artists.begin(), sim_artists.end(), [](const QString& artist1, const QString artist2){
		return (artist1 > artist2);
	});

	if(!sim_artists.isEmpty()){
		str = BOLD(tr("Similar artists:")) + CAR_RET + CAR_RET;
	}


	int i=0;
	QStringList artist_list;
	for(const QString& sim_artist : sim_artists){
		if(i++ > 50){
			break;
		}

		QString artist_name = _additional_info[sim_artist];

		int id = _db->getArtistID(artist_name);
		
		if( id >= 0 ){
			artist_list << BOLD(artist_name);
		}

		else {
			artist_list << artist_name;
		}

	}

	str += artist_list.join(", ");


	return str;
}
