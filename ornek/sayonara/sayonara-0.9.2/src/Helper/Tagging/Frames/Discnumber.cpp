/* Discnumber.cpp */

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


#include "Discnumber.h"

ID3v2Frame::Discnumber::Discnumber()
{
	n_discs = 1;
	disc = 1;
}

ID3v2Frame::Discnumber::Discnumber(quint8 disc_, quint8 n_discs_){
	n_discs = n_discs_;
	disc = disc_;
}


QString ID3v2Frame::Discnumber::to_string(){
	return QString("TPOS: ") + QString::number(disc) + "/" + QString::number(n_discs);
}

ID3v2Frame::DiscnumberFrame::DiscnumberFrame(TagLib::FileRef *file_ref) :
	AbstractFrame<Discnumber, TagLib::ID3v2::TextIdentificationFrame>(file_ref, "TPOS")
{

}

ID3v2Frame::DiscnumberFrame::~DiscnumberFrame(){

}

void ID3v2Frame::DiscnumberFrame::map_model_to_frame(){

	QByteArray byte_arr_header, byte_arr_body;
	TagLib::ByteVector data;

	int size;

	byte_arr_body.push_back(QString::number(_data_model.disc).toLatin1());
	byte_arr_body.push_back('/');
	byte_arr_body.push_back(QString::number(_data_model.disc).toLatin1());
	byte_arr_body.push_back((char) 0x00);

	size = byte_arr_body.size();

	byte_arr_header.push_back("TPOS");
	byte_arr_header.push_back((char) 0x00);
	byte_arr_header.push_back((char) 0x00);
	byte_arr_header.push_back((char) 0x00);
	byte_arr_header.push_back((quint8) size);
	byte_arr_header.push_back((char) 0x00);
	byte_arr_header.push_back((char) 0x00);

	byte_arr_header.push_back(byte_arr_body);

	data.setData(byte_arr_header.data(), byte_arr_header.size());
	_frame->setData(data);
}

void ID3v2Frame::DiscnumberFrame::map_frame_to_model(){
	TagLib::ByteVector vec = _frame->render();
	quint32 i, size;
	quint8 disc, n_discs;

	size  = ((int) vec[4]) << 21;
	size += ((int) vec[5]) << 14;
	size += ((int) vec[6]) << 7;
	size += ((int) vec[7]);

	disc=0;
	for(i=10; i<size; i++){
		if(i==vec.size()){
			break;
		}

		char c = vec.at(i);
		if(c == '/'){
			i++;
			break;
		}

		if(c >= '0' && c <='9'){
			disc = (disc * 10) + (c - '0');
		}
	}

	n_discs = 0;
	while(i<size && i<vec.size()){
		char c = vec.at(i);
		if(c >= '0' && c <='9'){
			n_discs = (n_discs * 10) + (c - '0');
		}
		i++;
	}

	_data_model.disc = disc;
	_data_model.n_discs = n_discs;
}

TagLib::ID3v2::Frame* ID3v2Frame::DiscnumberFrame::create_id3v2_frame(){
	return new TagLib::ID3v2::TextIdentificationFrame(TagLib::ByteVector());
}

