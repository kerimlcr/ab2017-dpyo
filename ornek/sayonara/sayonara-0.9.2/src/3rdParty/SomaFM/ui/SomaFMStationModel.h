/* SomaFMStationModel.h */

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



/* SomaFMStationModel.h */

#ifndef SomaFMStationModel_H
#define SomaFMStationModel_H

#include "GUI/Helper/SearchableWidget/AbstractSearchModel.h"
#include "3rdParty/SomaFM/SomaFMStation.h"
#include <QMap>
#include <QList>
class QMimeData;
class SomaFMStationModel : public AbstractSearchTableModel
{
	Q_OBJECT
public:
	explicit SomaFMStationModel(QObject *parent = 0);

private:
	QList<SomaFMStation> _stations;

	enum class Status {
		Waiting,
		Error,
		OK
	};

	Status _status;

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent=QModelIndex()) const override;
	int columnCount(const QModelIndex& parent=QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
	QMimeData* mimeData(const QModelIndexList &indexes) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;


	// AbstractSearchModelInterface interface
public:
	QModelIndex getFirstRowIndexOf(QString substr) override;
	QModelIndex getNextRowIndexOf(QString substr, int cur_row, const QModelIndex& parent) override;
	QModelIndex getPrevRowIndexOf(QString substr, int cur_row, const QModelIndex& parent) override;
	QMap<QChar, QString> getExtraTriggers() override;

	void set_stations(const QList<SomaFMStation>& stations);
	void replace_station(const SomaFMStation& station);
	bool has_stations() const;
	void set_waiting();
};


#endif // SomaFMStationModel_H
