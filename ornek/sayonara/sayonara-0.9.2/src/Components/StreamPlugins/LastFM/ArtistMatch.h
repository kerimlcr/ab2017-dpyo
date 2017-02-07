/* ArtistMatch.h */

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



#ifndef ARTISTMATCH_H
#define ARTISTMATCH_H

#include <QMap>
#include <QString>
#include <QObject>


class ArtistMatch{



public:

	struct ArtistDesc {
		QString artist_name;
		QString mbid;

		ArtistDesc(const QString& artist_name, const QString& mbid);

		bool operator ==(const ArtistDesc& other) const;
		bool operator <(const ArtistDesc& other) const;
		bool operator <=(const ArtistDesc& other) const;

		QString to_string() const;
	};

	/**
	 * @brief The Quality enum used to access the bin of interest. See ArtistMatch::get(Quality q)
	 */
	enum class Quality : quint8 {
		Poor = 0,
		Well = 1,
		Very_Good = 2
	};


	ArtistMatch();
	ArtistMatch(const QString& artist_name);
	ArtistMatch(const ArtistMatch& other);

	/**
	 * @brief checks, if structure is valid.
	 * @return false, if all bins are empty. True else
	 */
	bool is_valid() const;

	/**
	 * @brief Compares two ArtistMatch structures
	 * @param am the other ArtistMatch
	 * @return true, if the artist string is the same. False else
	 */
	bool operator ==(const ArtistMatch& am) const;

	/**
	 * @brief adds an artist string to the corresponding bin
	 * @param artist artist string
	 * @param match the match value
	 */
	void add(const ArtistDesc& artist, double match);

	/**
	 * @brief get bin by quality
	 * @param q quality. See ArtistMatch::Quality
	 * @return the desired bin
	 */
	QMap<ArtistDesc, double> get(Quality q) const;

	/**
	 * @brief get the corresponding artist name of the ArtistMatch structure
	 * @return artist name
	 */
	QString get_artist_name() const;

	/**
	 * @brief converts the artist match to string
	 * @return string representation
	 */
	QString to_string() const;


private:
	QMap<ArtistDesc, double> _very_good;
	QMap<ArtistDesc, double> _well;
	QMap<ArtistDesc, double> _poor;

	QString _artist;

};




#endif // ARTISTMATCH_H
