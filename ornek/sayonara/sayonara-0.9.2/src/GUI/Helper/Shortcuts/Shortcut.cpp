/* Shortcut.cpp */

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



#include "Shortcut.h"
#include "ShortcutHandler.h"
#include "ShortcutWidget.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"

Shortcut::Shortcut(ShortcutWidget* parent, const QString& identifier, const QString& name, const QStringList& default_shortcuts) :
	SayonaraClass()
{
	_name = name;
	_identifier = identifier;
	_parent=parent;

	_default_shortcuts = default_shortcuts;
	for(QString& str : _default_shortcuts){
		str.replace(" +", "+");
		str.replace("+ ", "+");
	}

	_shortcuts = _default_shortcuts;

	RawShortcutMap rsm = _settings->get(Set::Player_Shortcuts);

	if(rsm.contains(identifier)){
		_shortcuts = rsm[identifier];
	}

	else{
		_shortcuts = _default_shortcuts;
	}
}

Shortcut::Shortcut(ShortcutWidget* parent, const QString& identifier, const QString& name, const QString& default_shortcut) :
	Shortcut(parent, identifier, name, QStringList(default_shortcut))
{

}

Shortcut::Shortcut(const Shortcut& other)
{
	_parent =				other._parent;
	_name =					other._name;
	_identifier =			other._identifier;
	_default_shortcuts =	other._default_shortcuts;
	_shortcuts =			other._shortcuts;
	_qt_shortcuts =			other._qt_shortcuts;
}

Shortcut::Shortcut(){

}

QString Shortcut::get_name() const
{
	if(_parent){
		QString name = _parent->get_shortcut_text(_identifier);
		if(!name.isEmpty()){
			return name;
		}
	}
	return _name;
}

QStringList Shortcut::get_default() const
{
	return _default_shortcuts;
}

QList<QKeySequence> Shortcut::get_sequences() const
{
	QList<QKeySequence> sequences;
	for(const QString& str : get_shortcuts()){
		QKeySequence seq = QKeySequence::fromString(str, QKeySequence::NativeText);
		sequences << seq;
	}

	return sequences;
}

QStringList Shortcut::get_shortcuts() const
{
	return _shortcuts;
}

QString Shortcut::get_identifier() const
{
	return _identifier;
}

Shortcut Shortcut::getInvalid(){
	return Shortcut();
}

bool Shortcut::is_valid() const
{
	return !(_identifier.isEmpty());
}



void Shortcut::create_qt_shortcut(QWidget* parent, QObject* receiver, const char* slot)
{
	QList<QShortcut*> shortcuts = init_qt_shortcut(parent);
	for(QShortcut* sc : shortcuts){
		parent->connect(sc, SIGNAL(activated()), receiver, slot);
	}
}


QList<QShortcut*> Shortcut::init_qt_shortcut(QWidget* parent)
{

	QList<QShortcut*> lst;
	if(get_sequences().size() > 1){
		sp_log(Log::Debug) << "Number of shortcuts: " << get_sequences().size();
	}

	for(const QKeySequence& sequence : get_sequences()){
		QShortcut* shortcut = new QShortcut(parent);

		shortcut->setContext(Qt::WindowShortcut);
		shortcut->setKey(sequence);

		_qt_shortcuts << shortcut;

		lst << shortcut;
	}

	ShortcutHandler::getInstance()->set_shortcut(*this);

	return lst;
}


void Shortcut::change_shortcut(const QStringList &shortcuts){

	_shortcuts = shortcuts;
	for(QString& str : _shortcuts){
		str.replace(" +", "+");
		str.replace("+ ", "+");
	}

	for(QShortcut* sc : _qt_shortcuts){
		QList<QKeySequence> sequences = get_sequences();
		for(const QKeySequence& ks : sequences){
			sc->setKey(ks);
		}
	}

	ShortcutHandler::getInstance()->set_shortcut(*this);
}
