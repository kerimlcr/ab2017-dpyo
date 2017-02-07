/* ChangeablePipeline.h */

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



#ifndef CHANGEABLEPIPELINE_H
#define CHANGEABLEPIPELINE_H

#include "gst/gst.h"
class ChangeablePipeline
{
public:
    ChangeablePipeline();

    void add_element(GstElement* element, GstElement* first_element, GstElement* second_element);
    void remove_element(GstElement* element, GstElement* first_element, GstElement* second_element);

protected:
    virtual GstElement* get_pipeline() const=0;
};

#endif // CHANGEABLEPLAYLIST_H
