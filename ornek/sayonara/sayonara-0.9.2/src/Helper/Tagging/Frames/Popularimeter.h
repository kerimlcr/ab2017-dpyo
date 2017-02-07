/* Popularimeter.h */

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



#ifndef POPULARIMETER_H
#define POPULARIMETER_H


#include "AbstractFrame.h"

#include <QString>
#include <taglib/popularimeterframe.h>

namespace ID3v2Frame
{
	/**
	 * @brief The Popularimeter class
	 * @ingroup Tagging
	 */
	class Popularimeter
	{
		public:

			QString		email;
			quint8		rating_byte;	// value between 0 and 0xff
			int			playcount;

			Popularimeter();
			Popularimeter(const QString& email_, quint8 rating_byte_, int playcount);
			void set_sayonara_rating(quint8 max_5);
			quint8 get_sayonara_rating() const;
			QString to_string();
	};


	/**
	 * @brief The PopularimeterFrame class
	 * @ingroup Tagging
	 */
	class PopularimeterFrame :
			public AbstractFrame<Popularimeter, TagLib::ID3v2::PopularimeterFrame>
	{

		protected:
			TagLib::ID3v2::Frame* create_id3v2_frame() override;

		public:
			PopularimeterFrame(TagLib::FileRef* file_ref);
			~PopularimeterFrame();

			void map_model_to_frame() override;
			void map_frame_to_model() override;
	};
}
#endif // POPULARIMETER_H
