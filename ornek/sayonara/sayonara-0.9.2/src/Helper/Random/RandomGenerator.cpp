/* RandomGenerator.cpp */

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

#include "Helper/Random/RandomGenerator.h"

RandomGenerator::RandomGenerator()
{
	update_seed();
}

RandomGenerator::~RandomGenerator()
{

}


void RandomGenerator::update_seed(){
	_seed = std::chrono::system_clock::now().time_since_epoch().count();
	_generator = std::mt19937(_seed);

}

int RandomGenerator::get_number(int min, int max){
	std::uniform_int_distribution<int> d(min, max);
	return d(_generator);
}

int RandomGenerator::get_random_number(int min, int max)
{
	RandomGenerator generator;
	return generator.get_number(min, max);
}
