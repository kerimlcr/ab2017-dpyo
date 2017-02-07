/* GUI_PreferenceDialog.cpp */

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



#include "GUI_PreferenceDialog.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.cpp"
#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"

#include <QLayout>
#include <QItemDelegate>


GUI_PreferenceDialog::GUI_PreferenceDialog(QWidget *parent) :
	PreferenceDialogInterface(parent),
	Ui::GUI_PreferenceDialog()
{
	setup_parent(this);

	connect(list_preferences, &QListWidget::currentRowChanged, this, &GUI_PreferenceDialog::row_changed);

	connect(btn_apply, &QPushButton::clicked, this, &GUI_PreferenceDialog::commit);
	connect(btn_ok, &QPushButton::clicked, this, &GUI_PreferenceDialog::commit_and_close);
	connect(btn_cancel, &QPushButton::clicked, this, &GUI_PreferenceDialog::revert);
}

GUI_PreferenceDialog::~GUI_PreferenceDialog()
{

}

void GUI_PreferenceDialog::register_preference_dialog(PreferenceWidgetInterface* dialog)
{
	_dialogs << dialog;

	QListWidgetItem* item = new QListWidgetItem(dialog->get_action_name());

	item->setSizeHint(QSize(item->sizeHint().width(), 20));
	list_preferences->addItem(item);
}

void GUI_PreferenceDialog::language_changed()
{
	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
	list_preferences->setMouseTracking(false);
	list_preferences->setItemDelegate(new QItemDelegate(list_preferences));

	int i=0;
	for(PreferenceWidgetInterface* dialog : _dialogs){
		QListWidgetItem* item = list_preferences->item(i);
		item->setText(dialog->get_action_name());
		i++;
	}

	PreferenceDialogInterface::language_changed();
}



QString GUI_PreferenceDialog::get_action_name() const
{
	return tr("Preferences");
}


void GUI_PreferenceDialog::init_ui()
{

}

void GUI_PreferenceDialog::commit_and_close(){
	commit();
	close();
}

void GUI_PreferenceDialog::commit()
{
	for(PreferenceWidgetInterface* iface : _dialogs){
		if(iface->is_ui_initialized()){
			iface->commit();
		}
	}
}

void GUI_PreferenceDialog::revert()
{
	for(PreferenceWidgetInterface* iface : _dialogs){
		if(iface->is_ui_initialized()){
			iface->revert();
		}
	}

	close();
}

void GUI_PreferenceDialog::row_changed(int row)
{
	if(!between(row, _dialogs)){
		return;
	}

	hide_all();
	PreferenceWidgetInterface* widget = _dialogs[row];
	QLayout* layout = widget_preferences->layout();
	layout->setContentsMargins(0,0,0,0);

	if(layout){
		layout->addWidget(widget);
		layout->setAlignment(Qt::AlignTop);
	}

	lab_pref_title->setText(widget->get_action_name());
	widget->show();

}


void GUI_PreferenceDialog::hide_all(){

	for(PreferenceWidgetInterface* iface : _dialogs){
		iface->setParent(nullptr);
		iface->hide();

	}
}
