/* LibraryGenreView.h */

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



#ifndef LIBRARYGENREVIEW_H
#define LIBRARYGENREVIEW_H


#include <QMap>
#include <QTreeWidget>
#include <QThread>

#include <QItemSelectionModel>

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QShowEvent>

class CustomMimeData;
class MetaDataList;
class MetaDataChangeNotifier;
class TagEdit;

template<typename T>
class Tree;
class TreeDelegate;

typedef Tree<QString> GenreNode;



class LibraryGenreView : public QTreeWidget
{
	Q_OBJECT

signals:
	void sig_progress(const QString& message, int progress);

public:
	LibraryGenreView(QWidget* parent=nullptr);
	~LibraryGenreView();

	QSize sizeHint() const override;
	void reload_genres();
	int get_row_count() const;

private:

	GenreNode*				_genres=nullptr;
	TreeDelegate*			_delegate=nullptr;
	TagEdit*				_tag_edit=nullptr;
	QStringList				_expanded_items;
	bool					_filled;

private:
	void fill_list(const QStringList& genres);
	void init_data(const QStringList& genres);
	void insert_genres(QTreeWidgetItem* parent_item, GenreNode* node);

private slots:
	void update_genre_tags_finished();
	void item_expanded(QTreeWidgetItem* item);
	void item_collapsed(QTreeWidgetItem* item);

	void progress_changed(int progress);

	void metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new);


protected:
	void keyPressEvent(QKeyEvent* e) override;
	void dragEnterEvent(QDragEnterEvent* e) override;
	void dragMoveEvent(QDragMoveEvent* e) override;
	void dragLeaveEvent(QDragLeaveEvent* e) override;
	void dropEvent(QDropEvent* e) override;
};

#endif // LIBRARYGENREVIEW_H
