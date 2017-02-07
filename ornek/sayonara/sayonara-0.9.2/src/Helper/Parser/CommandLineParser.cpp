/* CommandLineParser.cpp */

/* Copyright (C) 2011-2015  Lucio Carreras
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




#include "CommandLineParser.h"
#include "Helper/Logger/Logger.h"
#include "Helper/FileHelper.h"

CommandLineData::CommandLineData()
{
    multiple_instances = false;
    abort = false;
}


CommandLineData CommandLineParser::parse(int argc, char** argv)
{
    CommandLineData data;

    for(int i=1; i<argc; i++) {
	QString str(argv[i]);

	if(str.compare("--help") == 0){
	    help();
	    data.abort = true;
	    return data;
	}

	if(str.compare("--multi-instances") == 0){
	    data.multiple_instances = true;
	}

	else
	{
	    data.files_to_play << Helper::File::get_absolute_filename(QString(argv[i]));
	}
    }

    return data;
}


void CommandLineParser::help()
{
    sp_log(Log::Info) << "sayonara [options] <list>";
    sp_log(Log::Info) << "<list> can consist of either files or directories or both";
    sp_log(Log::Info) << "Options:";
    sp_log(Log::Info) << "\t--multi-instances  Run more than one instance";
    sp_log(Log::Info) << "\t--help             Print this help dialog";
    sp_log(Log::Info) << "Bye.";
}
