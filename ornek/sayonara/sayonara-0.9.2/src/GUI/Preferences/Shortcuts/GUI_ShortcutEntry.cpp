
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



#include "GUI_ShortcutEntry.h"
#include <QKeySequence>
#include <QShortcut>
#include <QMessageBox>
#include "GUI/Preferences/Shortcuts/ShortcutLineEdit.h"
#include "GUI/Helper/IconLoader/IconLoader.h"

GUI_ShortcutEntry::GUI_ShortcutEntry(const Shortcut& shortcut, QWidget* parent) :
	SayonaraWidget(parent),
	Ui::GUI_ShortcutEntry(),
	_shortcut(shortcut)

{
	setupUi(this);

	_sch = ShortcutHandler::getInstance();

	lab_description->setText(_shortcut.get_name());
	le_entry->setText(_shortcut.get_shortcuts().join(", "));

	connect(btn_edit, &QPushButton::clicked, this, &GUI_ShortcutEntry::edit_clicked);
	connect(btn_default, &QPushButton::clicked, this, &GUI_ShortcutEntry::default_clicked);
	connect(btn_test, &QPushButton::clicked, this, &GUI_ShortcutEntry::test_clicked);
	connect(le_entry, &ShortcutLineEdit::sig_sequence_entered, this, &GUI_ShortcutEntry::sig_sequence_entered);

	skin_changed();
}

QList<QKeySequence> GUI_ShortcutEntry::get_sequences() const
{
	return le_entry->get_sequences();
}

void GUI_ShortcutEntry::show_sequence_error()
{
	this->le_entry->setText("");
	QMessageBox::warning(this, tr("Error"), tr("Shortcut already in use"));
}

void GUI_ShortcutEntry::commit()
{
	QString identifier = _shortcut.get_identifier();

	_sch->set_shortcut(identifier, le_entry->text().split(", "));
	_shortcut = _sch->get_shortcut(identifier);
}

void GUI_ShortcutEntry::clear(){
	le_entry->setText("");
}

void GUI_ShortcutEntry::revert(){
	le_entry->setText(_shortcut.get_shortcuts().join(", "));
}


void GUI_ShortcutEntry::default_clicked(){
	le_entry->setText(_shortcut.get_default().join(", "));
}

void GUI_ShortcutEntry::test_clicked()
{	
	QStringList splitted = le_entry->text().split(", ");
	QList<QKeySequence> sequences;

	for(const QString& str : splitted){
		sequences << QKeySequence::fromString(str, QKeySequence::NativeText);
	}

	emit sig_test_pressed(sequences);

}

void GUI_ShortcutEntry::language_changed()
{
	retranslateUi(this);

	lab_description->setText(_shortcut.get_name());

	btn_default->setToolTip(tr("Default"));
	btn_edit->setToolTip(tr("Edit"));
	btn_test->setToolTip(tr("Test"));
}

void GUI_ShortcutEntry::skin_changed()
{
	IconLoader* icon_loader = IconLoader::getInstance();
	btn_default->setIcon(icon_loader->get_icon("undo", "undo"));
	btn_edit->setIcon(icon_loader->get_icon("accessories-text-editor", "edit"));
	btn_test->setIcon(icon_loader->get_icon("dialog-info", "info"));
}



void GUI_ShortcutEntry::edit_clicked(){
	le_entry->clear();
	le_entry->setFocus();
}

