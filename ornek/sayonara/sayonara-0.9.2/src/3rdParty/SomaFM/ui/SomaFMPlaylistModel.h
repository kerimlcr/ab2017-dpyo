/* SomaFMPlaylistModel.h */

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



/* SomaFMPlaylistModel.h */

#ifndef SOMAFMPLAYLISTMODEL_H
#define SOMAFMPLAYLISTMODEL_H

#include <QStringListModel>
#include <QMimeData>
#include <QUrl>

#include "3rdParty/SomaFM/SomaFMStation.h"

class SomaFMStation;
class SomaFMPlaylistModel : public QStringListModel
{
    Q_OBJECT

private:
	SomaFMStation _station;

public:
    SomaFMPlaylistModel(QObject* parent=nullptr);
    QMimeData* mimeData(const QModelIndexList &indexes) const override;

    void setStation(const SomaFMStation& station);



};


#endif // SOMAFMPLAYLISTMODEL_H
