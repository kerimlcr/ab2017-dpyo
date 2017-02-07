/* SoundOutReceiver.h */

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



#ifndef SOUNDOUTRECEIVER_H
#define SOUNDOUTRECEIVER_H

#include <QList>

class LevelReceiver {
public:
	virtual void set_level(float left, float right)=0;
};

class SpectrumReceiver {
public:
	virtual void set_spectrum(const QList<float>& spectrum)=0;
};

#endif // LEVELRECEIVER_H

