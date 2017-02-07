/* CrossFader.h */

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



/* CrossFader.h */

#ifndef CROSSFADER_H
#define CROSSFADER_H

#include <QtGlobal>
#include <thread>

class FaderThreadData;
class FaderThread;
class CrossFader
{

public:

    enum class FadeMode : quint8 {
		NoFading=0,
		FadeIn,
		FadeOut
    };

    CrossFader();

    virtual double get_current_volume() const =0;
    virtual void set_current_volume(double vol)=0;

    quint64 get_fading_time() const;

    void fade_in();
    void fade_out();

    void fader_timed_out();


private:
    FadeMode	    _fade_mode;
	double			_fade_step;
	FaderThread*    _fader=nullptr;

	FaderThreadData* _fader_data=nullptr;


private:
    void increase_volume();
    void decrease_volume();

    void init_fader();


protected:

    void	    abort_fader();

};


#endif // CROSSFADER_H
