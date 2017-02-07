/* SayonaraWidgetTemplate.h */

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



#ifndef SAYONARAWIDGETTEMPLATE_H
#define SAYONARAWIDGETTEMPLATE_H

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QPalette>
#include <QColor>
#include <QShortcut>

#include "Helper/Settings/SayonaraClass.h"

#define combo_current_index_changed_int	static_cast<void (QComboBox::*) (int)>(&QComboBox::currentIndexChanged)
#define combo_activated_int	static_cast<void (QComboBox::*) (int)>(&QComboBox::activated)
#define spinbox_value_changed_int	static_cast<void (QSpinBox::*) (int)>(&QSpinBox::valueChanged)

template<typename T>
/**
 * @brief Template for Sayonara Widgets. This template is responsible for holding a reference to the settings and so, the
 * REGISTER_LISTENER and REGISTER_LISTENER_NO_CALL macros are available
 * @ingroup Widgets
 * @ingroup Interfaces
 */
class SayonaraWidgetTemplate :
		public T,
		protected SayonaraClass
{

public:
	SayonaraWidgetTemplate(QWidget* parent=nullptr) :
		T(parent),
		SayonaraClass()
	{

	}

	virtual ~SayonaraWidgetTemplate(){

	}

	/**
	 * @brief Returns, if the current skin is dark 
	 * @return true, if dark. false else
	 */
	bool is_dark() const
	{
		bool dark = (_settings->get(Set::Player_Style) == 1);

		QPalette palette = this->palette();
		QColor color = palette.color(QPalette::Normal, QPalette::Background);

		if(color.lightness() < 128 || dark){
			return true;
		}

		else{
			return false;
		}
	}



protected:

	QString elide_text(const QString &text, QWidget *widget, int max_lines){

		QFontMetrics metric = widget->fontMetrics();
		int width = widget->width();

		QStringList splitted = text.split(" ");
		QStringList ret;
		QString tmp;
		QString line;

		for( const QString& str : splitted){

			tmp = line + str;

			if(metric.boundingRect(tmp).width() > width){
				ret << line;

				if(ret.size() == max_lines){
					line = "";
					break;
				}

				line = str;
			}

			else{
				line += str + " ";
			}
		}


		QString final_str;
		if(ret.isEmpty()){
			final_str = text;
		}

		else if(line.isEmpty()){
			final_str = ret.join("\n");
			final_str += "...";
		}

		else {
			final_str = ret.join("\n") + line;
		}


		return final_str;
	}
};


#endif // SAYONARAWIDGETTEMPLATE_H
