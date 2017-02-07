/* GUI_Equalizer.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * GUI_Equalizer.cpp
 *
 *  Created on: May 18, 2011
 *      Author: Lucio Carreras
 */


#include "GUI_Equalizer.h"
#include "Components/Engine/EngineHandler.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"
#include "Helper/EqualizerPresets.h"

QString calc_lab(int val) {

	if(val > 0) {
        double v = val / 2.0;
        if(val % 2 == 0)
			return QString::number(v) + ".0";
        else
			return QString::number(v);
    }

    return QString::number(val) + ".0";

}

GUI_Equalizer::GUI_Equalizer(QWidget *parent) :
	PlayerPluginInterface(parent),
	Ui::GUI_Equalizer()
{


}

GUI_Equalizer::~GUI_Equalizer() {}

QString GUI_Equalizer::get_name() const
{
	return "Equalizer";
}

QString GUI_Equalizer::get_display_name() const
{
	return tr("Equalizer");
}


void GUI_Equalizer::language_changed() {
	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
}

void GUI_Equalizer::init_ui()
{
	if(is_ui_initialized()){
		return;
	}

	setup_parent(this);

	_engine = EngineHandler::getInstance();

	combo_presets->setItemDelegate(new ComboBoxDelegate(this));

	_active_idx = -1;

	sli_0->setData(0, label);
	sli_1->setData(1, label_2);
	sli_2->setData(2, label_3);
	sli_3->setData(3, label_4);
	sli_4->setData(4, label_5);
	sli_5->setData(5, label_6);
	sli_6->setData(6, label_7);
	sli_7->setData(7, label_8);
	sli_8->setData(8, label_9);
	sli_9->setData(9, label_10);

	_sliders.push_back(sli_0);
	_sliders.push_back(sli_1);
	_sliders.push_back(sli_2);
	_sliders.push_back(sli_3);
	_sliders.push_back(sli_4);
	_sliders.push_back(sli_5);
	_sliders.push_back(sli_6);
	_sliders.push_back(sli_7);
	_sliders.push_back(sli_8);
	_sliders.push_back(sli_9);

	cb_gauss->setChecked( _settings->get(Set::Eq_Gauss));

	for(EqSlider* s : _sliders) {

		connect(s, &EqSlider::sig_value_changed, this, &GUI_Equalizer::sli_changed);
		connect(s, &EqSlider::sig_slider_got_focus, this, &GUI_Equalizer::sli_pressed);
		connect(s, &EqSlider::sig_slider_lost_focus, this, &GUI_Equalizer::sli_released);
	}

	connect(btn_tool, &MenuToolButton::sig_save, this, &GUI_Equalizer::btn_save_clicked);
	connect(btn_tool, &MenuToolButton::sig_delete, this, &GUI_Equalizer::btn_delete_clicked);
	connect(btn_tool, &MenuToolButton::sig_undo, this, &GUI_Equalizer::btn_undo_clicked);
	connect(btn_tool, &MenuToolButton::sig_default, this, &GUI_Equalizer::btn_default_clicked);

	connect(cb_gauss, &QCheckBox::toggled, this, &GUI_Equalizer::cb_gauss_toggled);
	connect(combo_presets, &QComboBox::editTextChanged, this, &GUI_Equalizer::text_changed);

	fill_eq_presets();
}


int GUI_Equalizer::find_combo_text(QString text){
	int ret = -1;

	for(int i=0; i<combo_presets->count(); i++){
		if(combo_presets->itemText(i).compare(text, Qt::CaseInsensitive) == 0){
			ret = i;
		}
	}

	return ret;
}


void GUI_Equalizer::sli_pressed(){

	EqSlider* sli = static_cast<EqSlider*>(sender());
	int idx = sli->getIndex();

	_active_idx= idx;

	int i=0;
	for(EqSlider* slider : _sliders){
		_old_val[i] = slider->value();
		i++;
	}
}


void GUI_Equalizer::sli_released(){

	_active_idx = -1;
}


static double scale[] = {1.0, 0.6, 0.20, 0.06, 0.01};

void GUI_Equalizer::sli_changed(int idx, int new_val) {

	btn_tool->show_action(ContextMenu::EntryUndo, true);

	EqSlider* s = _sliders[idx];
	s->getLabel()->setText(calc_lab(new_val));

	_engine->set_equalizer(idx, new_val);

	// this slider has been changed actively
    if( idx == _active_idx && cb_gauss->isChecked() ){
		int delta = new_val - _old_val[idx];

		for(int i=idx-9; i<idx+9; i++){
			if(i < 0) continue;
			if(i == idx) continue;
			if(i >= _sliders.size()) break;

			// how far is the slider away from me?
			int x = abs(_active_idx - i);

			if(x > 4) continue;

			double new_val = _old_val[i] + (delta * scale[x]);

			_sliders[i]->setValue(new_val);
		}
	}
}


void GUI_Equalizer::fill_eq_presets() {

	if(!is_ui_initialized()){
		return;
	}

	QStringList items;

	int last_idx = _settings->get(Set::Eq_Last);
    _presets = _settings->get(Set::Eq_List);
	_presets.prepend(EQ_Setting());

	for(const EQ_Setting& s : _presets) {

		items << s.name();
	}

	combo_presets->insertItems(0, items);

	connect(combo_presets, combo_current_index_changed_int, this, &GUI_Equalizer::preset_changed);

	if(last_idx < _presets.size() && last_idx >= 0 ) {
		combo_presets->setCurrentIndex(last_idx);
	}

	else{
		last_idx = 0;
	}

	preset_changed(last_idx);
}


void GUI_Equalizer::preset_changed(int index) {

	if(index >= _presets.size()) {
		btn_tool->show_actions(ContextMenu::EntryNone);
		return;
	}

	EQ_Setting setting = _presets[index];

	btn_tool->show_action(ContextMenu::EntryUndo, false);

	bool is_default = setting.is_default();
	btn_tool->show_action(ContextMenu::EntryDefault, !is_default);

	bool is_default_name = setting.is_default_name();
	btn_tool->show_action(ContextMenu::EntryDelete, ((index > 0) && !is_default_name));

	QList<int> values = setting.values();

	for(int i=0; i<values.size(); i++){

		if(i >= _sliders.size()){
			break;
		}

		int value = values[i];

		_sliders[i]->setValue(value);
		_old_val[i] = value;
	}

    _settings->set(Set::Eq_Last, index);
}


void GUI_Equalizer::cb_gauss_toggled(bool b){
	_settings->set(Set::Eq_Gauss, b);
}

void GUI_Equalizer::btn_default_clicked()
{
	int cur_idx = combo_presets->currentIndex();
	QString cur_text = combo_presets->currentText();
	if(cur_text.trimmed().isEmpty()){
		return;
	}

	if( !EQ_Setting::is_default_name(cur_text) ){
		return;
	}

	_presets[cur_idx].set_values( EQ_Setting::get_default_values(cur_text) );
	preset_changed(cur_idx);
}


void GUI_Equalizer::btn_save_clicked() {

	QString text = combo_presets->currentText();
	if(text.isEmpty()){
		return;
	}

	int found_idx = find_combo_text(text);

	if(found_idx <= 0){
		EQ_Setting s = EQ_Setting::fromString(text + ":0:0:0:0:0:0:0:0:0:0");
		_presets << s;
		combo_presets->addItem(text);
		found_idx = combo_presets->count() - 1;
	}

	for(int i=0; i<_sliders.size(); i++){
		_presets[found_idx].set_value(i, _sliders[i]->value());
	}

	_presets.removeFirst();
	_settings->set(Set::Eq_List, _presets);
	_presets.prepend(EQ_Setting());

	combo_presets->setCurrentIndex(found_idx);
	preset_changed(found_idx);
}

void GUI_Equalizer::btn_delete_clicked(){

	btn_tool->show_action(ContextMenu::EntryUndo, false);
	int idx = combo_presets->currentIndex();

	combo_presets->setCurrentIndex(0);

	_presets.removeAt(idx);
	combo_presets->removeItem(idx);

	_presets.removeFirst();
	_settings->set(Set::Eq_List, _presets);
	_presets.prepend(EQ_Setting());
}

void GUI_Equalizer::btn_undo_clicked(){

	btn_tool->show_action(ContextMenu::EntryUndo, false);
	QString text = combo_presets->currentText();

	int found_idx = find_combo_text(text);

	if(found_idx <= 0){
		for(EqSlider* sli : _sliders){
			sli->setValue(0);
		}
	}

	else{
		for(int i=0; i<_sliders.size(); i++){
			_sliders[i]->setValue( _presets[found_idx].value(i) );
		}
	}
}

void GUI_Equalizer::text_changed(const QString& str){
	btn_tool->show_action(ContextMenu::EntrySave, str.size() > 0);
}
