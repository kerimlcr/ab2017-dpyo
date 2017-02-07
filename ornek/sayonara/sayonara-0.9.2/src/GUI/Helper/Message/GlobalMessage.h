/* GlobalMessage.h */

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



#ifndef GLOBALMESSAGE_H
#define GLOBALMESSAGE_H

#include "Helper/globals.h"

#include <QString>
#include <QObject>


class GlobalMessageReceiverInterface;

/**
 * @brief The GlobalMessage class
 * @ingroup GUIHelper
 */
class GlobalMessage : public QObject
{
	Q_OBJECT
	SINGLETON_QOBJECT(GlobalMessage)

	GlobalMessageReceiverInterface* _receiver=nullptr;

private slots:
	void receiver_destroyed();

public:

	enum class Answer : quint8 {
		Yes=0,
		No,
		Ok,
		Cancel,
		Undefined
	};

	enum class QuestionType : quint8 {
		YesNo=0,
		OkCancel
	};


	GlobalMessage::Answer info(
			const QString& info,
			const QString& sender_name=QString());

	GlobalMessage::Answer warning(
			const QString& warning,
			const QString& sender_name=QString());

	GlobalMessage::Answer error(
			const QString& error,
			const QString& sender_name=QString());

	GlobalMessage::Answer question(
			const QString& question,
			const QString& sender_name=QString(),
			QuestionType type=GlobalMessage::QuestionType::YesNo);

	/**
	 * @brief register a receiver here, so it is called whenever a message has to be written
	 * @param receiver the receiver class
	 * @return false, if there's already another receiver. True else
	 */
	bool register_receiver(GlobalMessageReceiverInterface* receiver);
};


#endif // GLOBALMESSAGE_H
