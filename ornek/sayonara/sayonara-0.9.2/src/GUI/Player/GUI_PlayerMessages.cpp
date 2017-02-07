/* GUI_PlayerMessages.cpp */

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

#include "GUI_Player.h"
#include <QMessageBox>

GlobalMessage::Answer convert_answer(QMessageBox::StandardButton answer)
{
	switch(answer){

		case QMessageBox::Ok:
			return GlobalMessage::Answer::Ok;

		case QMessageBox::Cancel:
		case QMessageBox::Abort:
			return GlobalMessage::Answer::Cancel;

		case QMessageBox::Yes:
			return GlobalMessage::Answer::Yes;

		case QMessageBox::No:
			return GlobalMessage::Answer::No;

		default:
			return GlobalMessage::Answer::Undefined;
	}
}

GlobalMessage::Answer GUI_Player::error_received(const QString &error, const QString &sender_name)
{
	QString title = sender_name.isEmpty() ? tr("Error") + ":" : sender_name + " " + tr("error") + ":";
	return convert_answer (QMessageBox::critical(this, title, error));
}

GlobalMessage::Answer GUI_Player::warning_received(const QString &warning, const QString &sender_name)
{
	QString title = sender_name.isEmpty() ? tr("Warning") + ":" : sender_name + " " + tr("warning") + ":";
	return convert_answer (QMessageBox::warning(this, title, warning));
}

GlobalMessage::Answer GUI_Player::info_received(const QString &info, const QString &sender_name)
{
	QString title = sender_name.isEmpty() ? tr("Info") + ":" : sender_name + " " + tr("info") + ":";

	return convert_answer (QMessageBox::information(this, title, info));
}

GlobalMessage::Answer GUI_Player::question_received(const QString &question, const QString &sender_name, GlobalMessage::QuestionType type)
{
	QString title = sender_name.isEmpty() ? tr("Info") + ":" : sender_name + " " + tr("info") + ":";

	if(type == GlobalMessage::QuestionType::YesNo){
		return convert_answer(QMessageBox::information(this, title, question, QMessageBox::Yes, QMessageBox::No));
	}

	else {
		return convert_answer(QMessageBox::information(this, title, question, QMessageBox::Ok, QMessageBox::Cancel));
	}
}
