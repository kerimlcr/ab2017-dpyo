/* GlobalMessageReceiverInterface.h */

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



#ifndef GLOBALMESSAGERECEIVERINTERFACE_H
#define GLOBALMESSAGERECEIVERINTERFACE_H

#include <QString>
#include "GlobalMessage.h"

/**
 * @brief The GlobalMessageReceiverInterface class\n
 * implement this class in order to have the possibility to show messages
 * @ingroup GUIHelper
 * @ingroup Interfaces
 */
class GlobalMessageReceiverInterface {

private:
        QString _name;

public:
        GlobalMessageReceiverInterface(const QString& name);

        QString get_name() const;


        virtual GlobalMessage::Answer question_received(const QString& info, const QString& sender_name=QString(),GlobalMessage::QuestionType type=GlobalMessage::QuestionType::YesNo )=0;
        virtual GlobalMessage::Answer info_received(const QString& info, const QString& sender_name=QString())=0;
        virtual GlobalMessage::Answer warning_received(const QString& warning, const QString& sender_name=QString())=0;
        virtual GlobalMessage::Answer error_received(const QString& error, const QString& sender_name=QString())=0;
};


#endif // GLOBALMESSAGERECEIVERINTERFACE_H
