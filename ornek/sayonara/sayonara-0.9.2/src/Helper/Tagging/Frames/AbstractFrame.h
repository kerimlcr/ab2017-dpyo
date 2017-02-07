/* AbstractFrame.h */

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



#ifndef ABSTRACTFRAME_H
#define ABSTRACTFRAME_H

#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>

#include <QString>
#include "Helper/Logger/Logger.h"

/**
 * @brief ID3v2Frame namespace
 * @ingroup Tagging
 */
namespace ID3v2Frame
{
	template<typename ModelType_t, typename FrameType_t>
	/**
	 * @brief The AbstractFrame class\n
	 * for example
	 * AbstractFrame<Discnumber, TagLib::ID3v2::TextIdentificationFrame>
	 * @ingroup Tagging
	 */
	class AbstractFrame {

		protected:
			const char*				_four=nullptr;
			TagLib::ID3v2::Tag*		_tag=nullptr;
			ModelType_t				_data_model;
			FrameType_t*			_frame=nullptr;


		protected:

			/**
			 * @brief create_id3v2_frame creates new id3v2 frame
			 * if there's no frame we have to create it manually
			 * every subclass has to implement this function
			 * @return pointer to newly created frame
			 */
			virtual TagLib::ID3v2::Frame* create_id3v2_frame()=0;

			/**
			 * @brief map_model_to_frame\n
			 * maps the model to the frame and vice versa
			 * so the frame knows how to get/set data
			 */
			virtual void map_model_to_frame()=0;
			virtual void map_frame_to_model()=0;


		public:
		// constructor
			AbstractFrame(TagLib::FileRef* file_ref, const char* four) :
				_four(four),
				_tag(nullptr),
				_frame(nullptr)
			{
				TagLib::MPEG::File* file_mp3;
				TagLib::ID3v2::FrameListMap map;
				TagLib::ID3v2::FrameList frame_list;

				file_mp3 = dynamic_cast<TagLib::MPEG::File*>(file_ref->file());
				if(!file_mp3) {
					return;
				}

				_tag = file_mp3->ID3v2Tag();
				if(!_tag){
					return;
				}

				// map, containing [four][frame list]
				map = _tag->frameListMap();
				frame_list = map[_four];
				if(!frame_list.isEmpty()) {
					_frame = dynamic_cast<FrameType_t*> (frame_list.front());
				}
			}

			// destructor
			virtual ~AbstractFrame(){

			}


			//
			/**
			 * @brief sets the _data_model by reading from the frame
			 * @param data reference to data filled with _data_model
			 * @return false, if frame cannot be accessed, true else
			 */
			virtual bool read(ModelType_t& data){
				if(!_frame){
					return false;
				}

				map_frame_to_model();
				data = _data_model;

				return true;
			}


			//
			/**
			 * @brief insert the _data_model into the frame
			 * @param data_model the data model
			 * @return false if frame cannot be accessed
			 */
			virtual bool write(const ModelType_t& data_model){

				bool created = false;

				if(!_tag){
					return false;
				}

				if(!_frame){

					_frame = dynamic_cast<FrameType_t*>(create_id3v2_frame());

					if(!_frame){
						return false;
					}
					created = true;
				}

				_data_model = data_model;

				map_model_to_frame();

				if(created){
					// after that, no need to delete _frame
					_tag->addFrame(_frame);
				}

				return true;
			}


			/**
			 * @brief if the frame was found when called read()
			 * @return true, if the frame was found
			 */
			bool is_frame_found() const {
				return (_frame != nullptr);
			}
	};
}
#endif // ABSTRACTFRAME_H
