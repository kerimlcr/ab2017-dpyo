
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


#include "GUI_FontConfig.h"
#include <QApplication>
#include <QFont>

GUI_FontConfig::GUI_FontConfig(QWidget* parent) :
	PreferenceWidgetInterface(parent),
	Ui::GUI_FontConfig()
{
	//REGISTER_LISTENER(Set::Player_Language, language_changed);
}

GUI_FontConfig::~GUI_FontConfig()
{

}

QString GUI_FontConfig::get_action_name() const
{
	return tr("Fonts");
}

void GUI_FontConfig::init_ui()
{
	setup_parent(this);

	_font_db = new QFontDatabase();

	connect(combo_fonts, &QFontComboBox::currentFontChanged, this, &GUI_FontConfig::combo_fonts_changed);
	connect(btn_default, &QPushButton::clicked, this, &GUI_FontConfig::default_clicked);

	combo_fonts->setEditable(false);
	combo_fonts->setFontFilters(QFontComboBox::ScalableFonts);

	revert();
}


void GUI_FontConfig::combo_fonts_changed(const QFont& font){

	_cur_font_size = combo_sizes->currentText().toInt();

	QStringList sizes = get_available_font_sizes(font);
	fill_sizes(sizes);

	int font_size = _cur_font_size;
	if(font_size <= 0){
		font_size = QApplication::font().pointSize();
	}

	int cur_font_size_idx = combo_sizes->findText(QString::number(font_size));

	if(cur_font_size_idx >= 0){
		combo_sizes->setCurrentIndex(cur_font_size_idx);
	}

	combo_lib_size->setCurrentIndex(0);
	combo_pl_size->setCurrentIndex(0);

	_is_default = false;
}


QStringList GUI_FontConfig::get_available_font_sizes(const QString& font_name, const QString& style)
{
	QStringList ret;
	QList<int> font_sizes =  _font_db->pointSizes(font_name, style);
	for(int font_size : font_sizes){
		ret << QString::number(font_size);
	}

	return ret;
}

QStringList GUI_FontConfig::get_available_font_sizes(const QFont& font)
{
	return get_available_font_sizes(font.family(), font.styleName());
}

void GUI_FontConfig::fill_sizes(const QStringList& sizes)
{
	combo_sizes->clear();
	combo_lib_size->clear();
	combo_pl_size->clear();

	combo_lib_size->addItem(tr("Inherit"));
	combo_pl_size->addItem(tr("Inherit"));

	for(QString sz : sizes){
		int isz = sz.toInt();
		if(isz < 7 || isz > 14){
			continue;
		}

		combo_sizes->addItem(sz);
		combo_lib_size->addItem(sz);
		combo_pl_size->addItem(sz);
	}
}


void GUI_FontConfig::commit()
{
	bool ok;
	int font_size;

	font_size = combo_sizes->currentText().toInt(&ok);
	_settings->set(Set::Player_FontName, combo_fonts->currentText());

	if(ok){
		_settings->set(Set::Player_FontSize, font_size);
	}

	font_size = combo_lib_size->currentText().toInt(&ok);
	if(!ok){
		font_size = -1;
	}
	_settings->set(Set::Lib_FontSize, font_size);

	font_size = combo_pl_size->currentText().toInt(&ok);
	if(!ok){
		font_size = -1;
	}
	_settings->set(Set::PL_FontSize, font_size);

	_settings->set(Set::Lib_FontBold, cb_lib_bold->isChecked());

	_cur_font_size = font_size;
}

void GUI_FontConfig::revert(){

	QString cur_family = _settings->get(Set::Player_FontName);
	int cur_font_size = _settings->get(Set::Player_FontSize);
	int cur_pl_font_size = _settings->get(Set::PL_FontSize);
	int cur_lib_font_size = _settings->get(Set::Lib_FontSize);
	bool bold = _settings->get(Set::Lib_FontBold);

	int idx = combo_fonts->findText(cur_family);
	if(idx >= 0){
		combo_fonts->setCurrentIndex(idx);
	}

	fill_sizes( get_available_font_sizes(combo_fonts->currentFont()) );

	idx = combo_sizes->findText(QString::number(cur_font_size));
	if(idx >= 0){
		combo_sizes->setCurrentIndex(idx);
	}

	idx = combo_lib_size->findText(QString::number(cur_lib_font_size));
	if(idx >= 0){
		combo_lib_size->setCurrentIndex(idx);
	}
	else{
		combo_lib_size->setCurrentIndex(0);
	}

	idx = combo_pl_size->findText(QString::number(cur_pl_font_size));
	if(idx >= 0){
		combo_pl_size->setCurrentIndex(idx);
	}
	else{
		combo_pl_size->setCurrentIndex(0);
	}

	cb_lib_bold->setChecked(bold);
}


void GUI_FontConfig::default_clicked()
{
	QFont font = QApplication::font();

	int cur_font_idx = combo_fonts->findText(font.family());
	if(cur_font_idx >= 0){
		combo_fonts->setCurrentIndex(cur_font_idx);
	}

	combo_lib_size->setCurrentIndex(0);
	combo_pl_size->setCurrentIndex(0);
	cb_lib_bold->setChecked(true);

	int cur_font_size_idx = combo_sizes->findText(QString::number(font.pointSize()));
	if(cur_font_size_idx >= 0){
		combo_sizes->setCurrentIndex(cur_font_size_idx);
	}

	_is_default = true;
}


void GUI_FontConfig::language_changed()
{
	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
	PreferenceWidgetInterface::language_changed();
}

void GUI_FontConfig::skin_changed()
{

}
