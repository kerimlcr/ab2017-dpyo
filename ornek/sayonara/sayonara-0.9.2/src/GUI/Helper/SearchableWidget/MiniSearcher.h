/* MiniSearcher.h */

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



#ifndef MINISEARCHER_H
#define MINISEARCHER_H

#include <QFrame>
#include <QLineEdit>
#include <QBoxLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QScrollArea>

class SearchableTableView;
class SearchableListView;

enum class MiniSearcherButtons : quint8
{
  NoButton=0,
  FwdButton,
  BwdButton,
  BothButtons
};

class MiniSearcherLineEdit : public QLineEdit {

	Q_OBJECT

signals:
	void sig_tab_pressed();
	void sig_le_focus_lost();



public slots:
	void keyPressEvent(QKeyEvent *);


public:
	MiniSearcherLineEdit(QWidget* parent=nullptr);
	virtual ~MiniSearcherLineEdit();
	bool event(QEvent *);

};


class MiniSearcher : public QFrame
{
    Q_OBJECT

signals:
    void sig_reset();
	void sig_text_changed(const QString&);
	void sig_find_next_row();
	void sig_find_prev_row();

private slots:

	void line_edit_text_changed(const QString&);
	void line_edit_focus_lost();
	void left_clicked();
	void right_clicked();



private:
	QAbstractScrollArea*    _parent=nullptr;
	QMap<QChar, QString>    _triggers;
	QPushButton*            _left_button=nullptr;
	QPushButton*            _right_button=nullptr;
	MiniSearcherLineEdit*   _line_edit=nullptr;
	QBoxLayout*             _layout=nullptr;

    bool isInitiator(QKeyEvent* event);
    void init(QString text);
	void initLayout(MiniSearcherButtons b);


public:

	MiniSearcher(SearchableListView* parent, MiniSearcherButtons b=MiniSearcherButtons::NoButton);
	MiniSearcher(SearchableTableView *parent, MiniSearcherButtons b=MiniSearcherButtons::NoButton);


    bool isInitialized();
    bool check_and_init(QKeyEvent* event);
	void setExtraTriggers(QMap<QChar, QString> triggers);
	QString getCurrentText();
	void keyPressEvent(QKeyEvent *e);

public slots:
	void reset();
};

#endif // MINISEARCHER_H
