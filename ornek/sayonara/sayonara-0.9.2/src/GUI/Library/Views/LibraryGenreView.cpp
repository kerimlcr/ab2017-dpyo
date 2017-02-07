/* LibraryGenreView.cpp */

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



#include "LibraryGenreView.h"
#include "Database/DatabaseHandler.h"
#include "Database/DatabaseConnector.h"

#include "Components/TagEdit/TagEdit.h"
#include "Components/TagEdit/MetaDataChangeNotifier.h"

#include "GUI/Helper/CustomMimeData.h"
#include "GUI/Helper/Delegates/TreeDelegate.h"

#include "Helper/MetaData/MetaData.h"
#include "Helper/Helper.h"
#include "Helper/Tree.h"



LibraryGenreView::LibraryGenreView(QWidget* parent) :
	QTreeWidget(parent)
{
	QStringList genres;

	_genres = new GenreNode("root");
	_delegate = new TreeDelegate(this);
	_tag_edit = new TagEdit(this);
	_filled = false;


	MetaDataChangeNotifier* mcn = MetaDataChangeNotifier::getInstance();

	this->setAcceptDrops(true);
	this->setDragDropMode(LibraryGenreView::DragDrop);

	this->setAlternatingRowColors(true);
	this->setItemDelegate(_delegate);

	connect(this, &QTreeWidget::itemCollapsed, this, &LibraryGenreView::item_collapsed);
	connect(this, &QTreeWidget::itemExpanded, this, &LibraryGenreView::item_expanded);

	connect(mcn, &MetaDataChangeNotifier::sig_metadata_changed, this, &LibraryGenreView::metadata_changed);

	connect(_tag_edit, &QThread::finished, this, &LibraryGenreView::update_genre_tags_finished);
	connect(_tag_edit, &TagEdit::sig_progress, this, &LibraryGenreView::progress_changed);

	genres = DatabaseConnector::getInstance()->getAllGenres();
	fill_list(genres);
}

LibraryGenreView::~LibraryGenreView()
{
	delete _genres; _genres = nullptr;
}


QSize LibraryGenreView::sizeHint() const
{
	QSize sz = QTreeView::sizeHint();
	sz.setWidth(200);
	return sz;
}

void LibraryGenreView::reload_genres()
{
	for(GenreNode* n : _genres->children){
		_genres->remove_child(n);
		delete n; n=nullptr;
	}

	this->clear();

	// fill it on next show event
	_filled = false;

	DatabaseConnector* db = DatabaseConnector::getInstance();
	QStringList genres = db->getAllGenres();
	fill_list(genres);
}

int LibraryGenreView::get_row_count() const
{
	int n_rows = _genres->children.size();
	return n_rows;
}



void LibraryGenreView::fill_list(const QStringList& genres){

	if(_filled){
		return;
	}

	_filled = true;

	this->init_data(genres);
	this->insert_genres(nullptr, _genres);
}


void LibraryGenreView::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept();
}

void LibraryGenreView::dragMoveEvent(QDragMoveEvent *e)
{
	QItemSelectionModel* ism;
	QModelIndex idx;

	idx = this->indexAt(e->pos());
	if(!idx.isValid()){
		sp_log(Log::Debug) << "Invalid index";
		return;
	}

	ism = this->selectionModel();

	ism->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	e->accept();
}

void LibraryGenreView::dragLeaveEvent(QDragLeaveEvent *e)
{
	this->clearSelection();
	e->accept();
}

void LibraryGenreView::dropEvent(QDropEvent *e)
{
	e->accept();

	const QMimeData* mime_data;
	const CustomMimeData* cmd;
	MetaDataList v_md;

	QString genre;
	QModelIndex idx;

	this->clearSelection();

	mime_data = e->mimeData();

	cmd = static_cast<const CustomMimeData*>(mime_data);
	if(!cmd){
		sp_log(Log::Debug) << "Cannot apply genre to data";
		return;
	}

	idx = this->indexAt(e->pos());
	if(!idx.isValid()){
		sp_log(Log::Debug) << "Invalid index";
		return;
	}

	this->setAcceptDrops(false);

	genre = idx.data().toString();
	v_md = cmd->getMetaData();

	sp_log(Log::Debug) << "Will apply " << genre << " to " << v_md.size() << " tracks";

	_tag_edit->set_metadata(v_md);
	_tag_edit->add_genre_to_metadata(genre);
	_tag_edit->commit();

	emit sig_progress(tr("Updating genres"), 0);

}


void LibraryGenreView::update_genre_tags_finished(){

	reload_genres();

	emit sig_progress("", -1);

	this->setAcceptDrops(true);
}


void LibraryGenreView::item_expanded(QTreeWidgetItem* item)
{
	_expanded_items << item->text(0);
}

void LibraryGenreView::item_collapsed(QTreeWidgetItem* item)
{
	_expanded_items.removeAll(item->text(0));
}

void LibraryGenreView::progress_changed(int progress)
{
	emit sig_progress(tr("Updating genres"), progress);
}

void LibraryGenreView::metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new)
{
	Q_UNUSED(v_md_old)
	Q_UNUSED(v_md_new)

	reload_genres();
}

void LibraryGenreView::keyPressEvent(QKeyEvent* e)
{

	if( e->key() == Qt::Key_Enter ||
		e->key() == Qt::Key_Return)
	{
		QTreeWidgetItem* item = this->currentItem();
		item->setExpanded(true);
	}

	QTreeWidget::keyPressEvent(e);
}


void LibraryGenreView::insert_genres(QTreeWidgetItem* parent_item, GenreNode* node){

	QTreeWidgetItem* item;
	QStringList text = { Helper::cvt_str_to_first_upper(node->data) };

	if(node->parent == _genres){
		item = new QTreeWidgetItem(this, text);
	}

	else{
		item = new QTreeWidgetItem(parent_item, text);
	}

	for(GenreNode* child : node->children){
		insert_genres(item, child);
	}

	if(_expanded_items.contains(node->data, Qt::CaseInsensitive)){
		item->setExpanded(true);
	}
}


void LibraryGenreView::init_data(const QStringList& genres){



	QStringList genre_copy;

	// just needed for the first 2 steps
	QMap<int, QList<int>> item_parent_map;

	// stores the index in genres and the corresponding node
	QMap<int, GenreNode*> item_node_map;

	genre_copy = genres;
	if(!genre_copy.contains("")){
		genre_copy << "";
	}


	// find all parents of all genres ( O(n²) )
	for(auto it = genre_copy.begin(); it != genre_copy.end(); it++){

		QString genre = *it;
		int idx = (it - genre_copy.begin());

		if(genre.isEmpty()){
			continue;
		}

		for(auto subit = genre_copy.begin(); subit != genre_copy.end(); subit++){

			int subidx = (subit - genre_copy.begin());
			QString sub_genre = *subit;

			if( ( sub_genre.contains(genre, Qt::CaseInsensitive) ) &&
					( sub_genre.compare(genre, Qt::CaseInsensitive) != 0) )

			{
				QList<int> parents = item_parent_map[subidx];

				parents << idx;

				item_parent_map[subidx] = parents;
			}
		}
	}

	int original_genre_size = genre_copy.size();
	// create new entries for all items with more than one parent
	// append them to to the end of genre_copy
	for(int idx=0; idx<original_genre_size; idx++){

		QString genre = genre_copy[idx];
		QList<int> parent_idxs = item_parent_map[idx];

		GenreNode* node = new GenreNode(genre.toLower());
		item_node_map[idx] = node;

		for(int i=parent_idxs.size() - 1; i>=1; i--){

			GenreNode* new_node = new GenreNode(genre.toLower());
			QList<int> new_parent_idxs;

			int new_idx = genre_copy.size();
			int parent_idx = parent_idxs.takeLast();

			new_parent_idxs << new_idx;
			item_parent_map[new_idx] << parent_idx;

			item_node_map[new_idx] = new_node;

			genre_copy << genre;
		}

		item_parent_map[idx] = parent_idxs;
	}


	// get all parents and add the children
	for(auto it = genre_copy.begin(); it != genre_copy.end(); it++){

		int idx = (it - genre_copy.begin());
		QList<int> parent_idxs; // should have at most 1 element
		GenreNode* node;		// corresponding node to genre_copy
		GenreNode* parent_node = nullptr;	// parent

		node = item_node_map[idx];
		parent_idxs = item_parent_map[idx];

		if(parent_idxs.isEmpty()){
			parent_node = _genres;
		}

		else{
			int first_parent_idx;
			first_parent_idx = parent_idxs[0];
			parent_node = item_node_map[ first_parent_idx ];
		}

		if(node != nullptr){
			parent_node->add_child(node);
		}
		else{
			sp_log(Log::Debug) << "node ist null";
		}
	}

	_genres->sort(true);
	_genres->print(0);
}
