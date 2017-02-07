/* MyColumnHeader.h */

/* Copyright (C) 2013  Lucio Carreras
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
 * MyColumnHeader.h
 *
 *  Created on: 19.12.2012
 *      Author: lugmair
 */

#ifndef MYCOLUMNHEADER_H_
#define MYCOLUMNHEADER_H_

#include <QString>

#include "Helper/globals.h"
#include "Components/Library/Sorting.h"
#include <QAction>


class ColumnHeader : public QObject {
	Q_OBJECT

public:
	enum class SizeType : quint8
	{
		Abs=0,
		Rel,
		Undefined
	};

protected:
	ColumnHeader(bool switchable, SortOrder sort_asc, SortOrder sort_desc);

protected:
	QAction*		_action=nullptr;
	bool 			_switchable;

	int 			_preferred_size_abs;
	double			_preferred_size_rel;

	SortOrder		_sort_asc;
	SortOrder		_sort_desc;
	SizeType		_size_type;

public:
	ColumnHeader(bool switchable, SortOrder sort_asc, SortOrder sort_desc, int preferred_size_abs);
	ColumnHeader(bool switchable, SortOrder sort_asc, SortOrder sort_desc, double preferred_size_rel, int min_size);

	void set_preferred_size_abs(int preferred_size);
	void set_preferred_size_rel(double preferred_size);

	int get_preferred_size_abs() const;
	double get_preferred_size_rel() const;

	SortOrder get_asc_sortorder() const;
	SortOrder get_desc_sortorder() const;

	ColumnHeader::SizeType get_size_type() const;

	bool is_visible() const;
	bool is_hidden() const;
	bool is_switchable() const;

	void retranslate();

	QAction* get_action();
	virtual QString get_title() const=0;
};

class SharpHeader : public ColumnHeader {
	Q_OBJECT
public:
	using ColumnHeader::ColumnHeader;
	QString get_title() const override {
		return "#";
	}
};

class ArtistHeader : public ColumnHeader {
	Q_OBJECT
public:
	using ColumnHeader::ColumnHeader;
	QString get_title() const override {
		return tr("Artist");
	}
};

class AlbumHeader : public ColumnHeader {
	Q_OBJECT
public:
	using ColumnHeader::ColumnHeader;
	QString get_title() const override {
		return tr("Album");
	}
};

class TitleHeader : public ColumnHeader {
	Q_OBJECT
public:
	using ColumnHeader::ColumnHeader;
	QString get_title() const override {
		return tr("Title");
	}
};

class NumTrackHeader : public ColumnHeader {
	Q_OBJECT
public:
	using ColumnHeader::ColumnHeader;
	QString get_title() const override {
		return tr("#Tracks");
	}
};

class DurationHeader : public ColumnHeader {
	Q_OBJECT
public:
	using ColumnHeader::ColumnHeader;
	QString get_title() const override {
		return tr("Duration");
	}
};


class DurationShortHeader : public ColumnHeader {
	Q_OBJECT
public:
	using ColumnHeader::ColumnHeader;
	QString get_title() const override {
		return tr("Dur.");
	}
};

class YearHeader : public ColumnHeader {
	Q_OBJECT
public:
	using ColumnHeader::ColumnHeader;
	QString get_title() const override {
		return tr("Year");
	}
};

class RatingHeader : public ColumnHeader {
	Q_OBJECT
public:
	using ColumnHeader::ColumnHeader;
	QString get_title() const override {
		return tr("Rating");
	}
};

class BitrateHeader : public ColumnHeader {
	Q_OBJECT
public:
	using ColumnHeader::ColumnHeader;
	QString get_title() const override {
		return tr("Bitrate");
	}
};

class FilesizeHeader : public ColumnHeader {
	Q_OBJECT
public:
	using ColumnHeader::ColumnHeader;
	QString get_title() const override {
		return tr("Filesize");
	}
};


class ColumnHeaderList :
	public QList<ColumnHeader*> {

	public:
	   int get_shown_columns() const;
	   int get_nth_shown_col(int n) const;
};

  #endif /* MYCOLUMNHEADER_H_ */
