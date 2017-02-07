/* PlaylistItemModel.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * PlaylistItemModel.h
 *
 *  Created on: Apr 8, 2011
 *      Author: Lucio Carreras
 */

#ifndef PLAYLISTITEMMODEL_H_
#define PLAYLISTITEMMODEL_H_

#include "Components/Playlist/Playlist.h"
#include "GUI/Helper/SearchableWidget/AbstractSearchModel.h"

#include "Helper/MetaData/MetaData.h"

#include <QUrl>
#include <QLabel>
#include <QAbstractListModel>
#include <QStringList>

class CustomMimeData;
class PlaylistItemModel : public AbstractSearchListModel {
	Q_OBJECT

public:
	PlaylistItemModel(PlaylistPtr pl, QObject* parent=nullptr);

	virtual ~PlaylistItemModel();

	int rowCount(const QModelIndex &parent=QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
	const MetaData& get_md(int row) const;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	bool setData(const QModelIndex &index, const QVariant& var, int role=Qt::DisplayRole) override;

	void clear();

	void remove_rows(const SP::Set<int>& rows);
	void move_rows(const SP::Set<int>& rows, int target_index);
	void copy_rows(const SP::Set<int>& rows, int target_index);
	void insert_metadata(const MetaDataList& v_md, int target_index);

	void set_current_track(int row);
	int get_current_track() const;

	QModelIndex getFirstRowIndexOf(QString substr) override;
	QModelIndex getPrevRowIndexOf(QString substr, int row, const QModelIndex &parent=QModelIndex()) override;
	QModelIndex getNextRowIndexOf(QString substr, int row, const QModelIndex &parent=QModelIndex()) override;
	QMap<QChar, QString> getExtraTriggers() override;


	void get_metadata(const IdxList& rows, MetaDataList& v_md);
	CustomMimeData* get_custom_mimedata(const QModelIndexList& indexes) const;
	QMimeData* mimeData(const QModelIndexList& indexes) const override;

	bool has_local_media(const IdxList& idxs) const;


protected:
	PlaylistPtr			_pl=nullptr;

private slots:
	void				playlist_changed(int pl_idx);

};

#endif /* PLAYLISTITEMMODEL_H_ */
