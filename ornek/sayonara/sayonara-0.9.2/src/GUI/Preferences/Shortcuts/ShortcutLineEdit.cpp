/* ShortcutLineEdit.cpp */

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



#include "ShortcutLineEdit.h"
#include "Helper/Logger/Logger.h"

bool is_modifier(int key){
	return (key == Qt::Key_Control
			|| key == Qt::Key_Alt
			|| key == Qt::Key_AltGr
			|| key == Qt::Key_Shift
			|| key == Qt::Key_Meta
			);
}


ShortcutLineEdit::ShortcutLineEdit(QWidget*parent) :
	QLineEdit(parent)
{
}

QList<QKeySequence> ShortcutLineEdit::get_sequences() const
{
	QStringList lst = this->text().split(",");
	QList<QKeySequence> sequences;
	for(const QString& str : lst){
		if(str.isEmpty()){
			continue;
		}

		sequences << QKeySequence::fromString(str, QKeySequence::NativeText);
	}

	return sequences;
}

void ShortcutLineEdit::keyPressEvent(QKeyEvent* e)
{
	int key = e->key();

	if(key == Qt::Key_Escape && e->modifiers() == Qt::NoModifier){
		this->setText("");
		return;
	}

	if(e->modifiers() == Qt::NoModifier){
		return;
	}

	if(is_modifier(key)){
		key = 0;
	}

	if(e->modifiers() & Qt::ControlModifier){
		key |= Qt::CTRL;
	}

	if(e->modifiers() & Qt::ShiftModifier){
		key |= Qt::SHIFT;
	}

	if(e->modifiers() & Qt::MetaModifier){
		key |= Qt::META;
	}

	if(e->modifiers() & Qt::AltModifier){
		key |= Qt::ALT;
	}

	if(e->modifiers() & Qt::KeypadModifier){
		key |= Qt::KeypadModifier;
	}

	QKeySequence ks(key);
	this->setText(ks.toString(QKeySequence::NativeText));

	emit sig_sequence_entered();
}
