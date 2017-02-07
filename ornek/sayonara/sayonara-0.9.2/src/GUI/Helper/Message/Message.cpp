/* Message.cpp */

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



#include "Message.h"


GlobalMessage::Answer Message::info(const QString& info, const QString& sender_name){
        return GlobalMessage::getInstance()->info(info, sender_name);
}

GlobalMessage::Answer Message::warning(const QString& warning, const QString& sender_name){
        return GlobalMessage::getInstance()->warning(warning, sender_name);
}

GlobalMessage::Answer Message::error(const QString& error, const QString& sender_name){
        return GlobalMessage::getInstance()->error(error, sender_name);
}

GlobalMessage::Answer Message::question_yn(const QString& error, const QString& sender_name){
        return GlobalMessage::getInstance()->question(error, sender_name, GlobalMessage::QuestionType::YesNo);
}

GlobalMessage::Answer Message::question_ok(const QString& error, const QString& sender_name){
        return GlobalMessage::getInstance()->question(error, sender_name, GlobalMessage::QuestionType::OkCancel);
}

