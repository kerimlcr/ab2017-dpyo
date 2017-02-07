/* RandomGenerator.h */

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



#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include <random>
#include <chrono>

/**
 * @brief The RandomGenerator class
 * @ingroup Helper
 */
class RandomGenerator
{

private:
	unsigned int _seed;
	std::mt19937 _generator;

public:
	RandomGenerator();
	~RandomGenerator();

	/**
	 * @brief create new seed
	 */
	void update_seed();

	/**
	 * @brief get a random number
	 * @param min minimum
	 * @param max maximum
	 * @return number between [minimum, maximum]
	 */
	int get_number(int min, int max);

	static int get_random_number(int min, int max);
};

#endif // RANDOMGENERATOR_H

