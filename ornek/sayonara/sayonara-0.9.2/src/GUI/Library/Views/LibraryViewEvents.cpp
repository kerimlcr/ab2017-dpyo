/* LibraryViewEvents.cpp */

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


#include "LibraryView.h"
#include "HeaderView.h"
#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"
#include <QMimeData>

bool LibraryView::event(QEvent* e)
{
	if(e->type() == QEvent::ContextMenu){
		SP::Set<int> selections = get_selections();

		QContextMenuEvent* cme = dynamic_cast<QContextMenuEvent*>(e);
		QPoint pos = cme->globalPos();

		if(_type == MetaDataList::Interpretation::Tracks && selections.size() == 1)
		{
			_rc_menu->show_action(LibraryContextMenu::EntryLyrics, true);
		}
		else{
			_rc_menu->show_action(LibraryContextMenu::EntryLyrics, false);
		}

		bool is_right_type = 
			(_type == MetaDataList::Interpretation::Artists ||
			_type == MetaDataList::Interpretation::Albums);

		if(is_right_type){

			bool has_selections = (selections.size() > 1);

			_merge_action->setVisible(has_selections);

			if(has_selections){
				_merge_menu->clear();

				for(int i : selections)
				{
					QString name = _model->get_string(i);
					int id = _model->get_id_by_row(i);
					QAction* action = new QAction(name, _merge_menu);
					action->setData(id);

					_merge_menu->addAction(action);
					connect(action, &QAction::triggered, this, &LibraryView::merge_action_triggered);
				}
			}
		}

		rc_menu_show(pos);
	}

	return SearchableTableView::event(e);
}

// mouse events
void LibraryView::mousePressEvent(QMouseEvent* event) 
{
	QPoint pos_org = event->pos();
	QPoint pos = QWidget::mapToGlobal(pos_org);

	if(_model->rowCount() == 0){
		return;
	}

	switch(event->button()) {

		case Qt::LeftButton:
			_drag_pos = pos_org;
			break;

		case Qt::MidButton:
			emit sig_middle_button_clicked(pos);
			break;

		default:
			break;
	}

	SearchableTableView::mousePressEvent(event);
}


void LibraryView::mouseMoveEvent(QMouseEvent* event) 
{
	int distance = (event->pos() - _drag_pos).manhattanLength();

	if( event->buttons() & Qt::LeftButton &&
			distance > QApplication::startDragDistance())
	{
		do_drag();
	}
}

void LibraryView::mouseDoubleClickEvent(QMouseEvent *event) 
{
	event->setModifiers(Qt::NoModifier);
	QTableView::mouseDoubleClickEvent(event);
}

void LibraryView::mouseReleaseEvent(QMouseEvent* event) 
{
	switch (event->button()) {

		case Qt::LeftButton:

			SearchableTableView::mouseReleaseEvent(event);
			event->accept();

			break;

		default:
			break;
	}
}
// mouse events end


// keyboard events
void LibraryView::keyPressEvent(QKeyEvent* event) 
{
	int key = event->key();

	Qt::KeyboardModifiers  modifiers = event->modifiers();

	bool shift_pressed = (modifiers & Qt::ShiftModifier);
	bool alt_pressed = (modifiers & Qt::AltModifier);
	bool ctrl_pressed = (modifiers & Qt::ControlModifier);

	if((key == Qt::Key_Up || key == Qt::Key_Down)) {
		if(this->selectionModel()->selection().isEmpty()) {
			if(_model->rowCount() > 0) {
				selectRow(0);
			}
			return;
		}

		if(ctrl_pressed)
			event->setModifiers(Qt::NoModifier);
	}

	SearchableTableView::keyPressEvent(event);
	if(!event->isAccepted()) return;

	SP::Set<int> selections = get_selections();

	switch(key) {

		case Qt::Key_Escape:

			clearSelection();
			this->selectionModel()->clearSelection();

			break;

		case Qt::Key_Return:
		case Qt::Key_Enter:

			if(selections.size() == 0) break;
			if(ctrl_pressed) break;

			// standard enter
			if(!shift_pressed && !alt_pressed){
				int first_idx = selections.first();
				emit doubleClicked( _model->index(first_idx, 0));
			}

			// enter with shift
			else if(shift_pressed && !alt_pressed) {
				emit sig_append_clicked();
			}

			else if(alt_pressed) {
				emit sig_play_next_clicked();
			}

			break;

		case Qt::Key_End:
			this->selectRow(_model->rowCount() - 1);
			break;

		case Qt::Key_Home:
			this->selectRow(0);
			break;

		default:
			break;
	}
}
// keyboard end

void LibraryView::dropEvent(QDropEvent *event) 
{
	event->accept();
	const QMimeData* mime_data = event->mimeData();

	if(!mime_data) return;

	QString text = "";

	if(mime_data->hasText()){
		text = mime_data->text();
	}

	// extern drops
	if( !mime_data->hasUrls() || text.compare("tracks", Qt::CaseInsensitive) == 0) {
		return;
	}

	QStringList filelist;
	for(const QUrl& url : mime_data->urls()) {

		QString path = url.path();

		if(QFile::exists(path)) {
			filelist << path;
		}
	}

	emit sig_import_files(filelist);
}

void LibraryView::dragEnterEvent(QDragEnterEvent *event) 
{
	event->accept();
}

void  LibraryView::dragMoveEvent(QDragMoveEvent *event) 
{
	event->accept();
}

void LibraryView::resizeEvent(QResizeEvent *event)
{
	SearchableTableView::resizeEvent(event);
	get_header_view()->refresh_sizes(this);
}
