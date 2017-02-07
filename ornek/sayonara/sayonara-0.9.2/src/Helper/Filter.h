/* Filter.h

 * Copyright (C) 2012-2016 Lucio Carreras  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Jul 9, 2012 
 *
 */

#ifndef FILTER_H_
#define FILTER_H_

#include <QString>


struct Filter {

	enum class Mode : quint8
	{
		Fulltext=0,
		Filename,
		Genre
	};

	QString		filtertext;
	Mode		mode;
	bool		cleared;

	Filter(){
		filtertext = "";
		mode = Mode::Fulltext;
		cleared = true;
	}
};


#endif /* FILTER_H_ */
