/* Popularimeter.cpp */

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


#include "Popularimeter.h"

ID3v2Frame::Popularimeter::Popularimeter(){
	email = "sayonara player";
	rating_byte = 0x00;
	playcount = 0;
}

ID3v2Frame::Popularimeter::Popularimeter(const QString& email_, quint8 rating_byte_, int playcount_){
	playcount = playcount_;
	rating_byte = rating_byte_;
	email = email_;
}


void ID3v2Frame::Popularimeter::set_sayonara_rating(quint8 max_5){
	if(max_5 >= 5){
		rating_byte = 255;
	}
	else if(max_5 == 4){
		rating_byte =  196;
	}
	else if(max_5 == 3){
		rating_byte =  128;
	}
	else if(max_5 == 2){
		rating_byte =  64;
	}
	else if(max_5 == 1){
		rating_byte =  1;
	}
	else{
		rating_byte = 0;
	}
}

quint8 ID3v2Frame::Popularimeter::get_sayonara_rating() const
{
	if(rating_byte > 224){
		return 5;
	}
	else if(rating_byte > 160){
		return 4;
	}
	else if(rating_byte > 96){
		return 3;
	}
	else if(rating_byte > 32){
		return 2;
	}
	else if(rating_byte > 0){
		return 1;
	}
	return 0;
}

QString ID3v2Frame::Popularimeter::to_string(){
	return QString("POPM: ") + email + " " + QString::number(rating_byte) + " " + QString::number(playcount);
}



ID3v2Frame::PopularimeterFrame::PopularimeterFrame(TagLib::FileRef* f) :
	AbstractFrame<Popularimeter, TagLib::ID3v2::PopularimeterFrame>(f, "POPM")
{

}

ID3v2Frame::PopularimeterFrame::~PopularimeterFrame(){

}

TagLib::ID3v2::Frame* ID3v2Frame::PopularimeterFrame::create_id3v2_frame(){
	return new TagLib::ID3v2::PopularimeterFrame();
}

void ID3v2Frame::PopularimeterFrame::map_model_to_frame(){
	_frame->setEmail(TagLib::String(_data_model.email.toUtf8().data(), TagLib::String::UTF8));
	_frame->setCounter(_data_model.playcount);
	_frame->setRating(_data_model.rating_byte);
	TagLib::ByteVector vec = _frame->render();
	QByteArray arr(vec.data(), vec.size());
	sp_log(Log::Debug) << arr;

}

void ID3v2Frame::PopularimeterFrame::map_frame_to_model(){
	_data_model.playcount = _frame->counter();

	_data_model.email = QString::fromLatin1(_frame->email().toCString());
	_data_model.rating_byte = (quint8) _frame->rating();
}
