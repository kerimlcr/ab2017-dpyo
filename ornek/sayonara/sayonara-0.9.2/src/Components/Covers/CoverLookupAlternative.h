/* CoverLookupAlternative.h */

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



#ifndef COVERLOOKUPALTERNATIVE_H
#define COVERLOOKUPALTERNATIVE_H

#include "CoverLocation.h"
#include "CoverLookup.h"
#include "Helper/MetaData/Artist.h"
#include "Helper/MetaData/Album.h"

/**
 * @brief The CoverLookupAlternative class
 * @ingroup Covers
 */
class CoverLookupAlternative : public CoverLookupInterface
{

    Q_OBJECT

private:

    CoverLookupAlternative(QObject* parent, int n_covers);

	CoverLookupPtr      _cl;

	int					_n_covers;
    bool				_run;

	CoverLocation		_cover_location;

public:

	CoverLookupAlternative(QObject* parent, const CoverLocation& cl, int n_covers);

    ~CoverLookupAlternative();

	void stop() override;
    void start();


private slots:
	void cover_found(const QString& cover_path);
    void finished(bool);
};


#endif // COVERLOOKUPALTERNATIVE_H
