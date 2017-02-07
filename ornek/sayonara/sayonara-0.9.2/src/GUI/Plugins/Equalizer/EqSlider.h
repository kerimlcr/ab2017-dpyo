/* EqSlider.h */

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



#ifndef EQSLIDER_H
#define EQSLIDER_H

#include "GUI/Helper/SayonaraWidget/SayonaraSlider.h"
#include <QKeyEvent>

/**
 * @brief The EqSlider class
 * @ingroup Equalizer
 */
class EqSlider : 
	public SayonaraSlider 
{
	Q_OBJECT

	signals:
		void sig_value_changed(int idx, int val);

	public:
		EqSlider(QWidget* parent);
		virtual ~EqSlider() {}

		/**
		 * @brief sets everything the slider has to be aware about
		 * @param idx the index of the slider
		 * @param label the value label of the slider
		 */
		void setData(int idx, QLabel* label);

		/**
		 * @brief get the value label
		 * @return
		 */
		QLabel* getLabel() const;

		/**
		 * @brief get the index of the slider
		 * @return
		 */
		int getIndex() const;

		/**
		 * @brief set the index of the slider
		 * @param idx new index
		 */
		void setIndex(int idx);

		/**
		 * @brief get the gstreamer compatible value
		 * @return
		 */
		double get_eq_value() const;

		/**
		 * @brief set the gstreamer compatible value
		 * @param val
		 */
		void set_eq_value(double val);

	protected:
		void sliderChange(SliderChange change) override;
		void keyPressEvent(QKeyEvent* e) override;

	private:
		QLabel* _label=nullptr;
		int		_idx;
};


#endif // EQSLIDER_H
