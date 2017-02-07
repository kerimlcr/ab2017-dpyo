/* GUI_Broadcast.cpp */

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


#include "GUI_Broadcast.h"

#include "GUI/Helper/Message/Message.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"

GUI_Broadcast::GUI_Broadcast(QWidget *parent) :
	PlayerPluginInterface(parent),
	Ui::GUI_Broadcast()
{
	_server = new StreamServer(this);

	connect(_server, &StreamServer::sig_new_connection, this, &GUI_Broadcast::new_connection);
	connect(_server, &StreamServer::sig_connection_closed, this, &GUI_Broadcast::connection_closed);
	connect(_server, &StreamServer::sig_can_listen, this, &GUI_Broadcast::can_listen);

	_server->retry();

}


GUI_Broadcast::~GUI_Broadcast(){

}

QString GUI_Broadcast::get_name() const
{
	return "Broadcast";
}

QString GUI_Broadcast::get_display_name() const
{
	return tr("Broadcast");
}


void GUI_Broadcast::language_changed(){

	if(is_ui_initialized()){
		retranslateUi(this);
		set_status_label();
	}
}

void GUI_Broadcast::init_ui()
{
	if(is_ui_initialized()){
		return;
	}

	setup_parent(this);

	combo_clients->setItemDelegate(new ComboBoxDelegate(combo_clients));

	btn_dismiss->setEnabled(false);
	btn_dismiss_all->setEnabled(false);

	connect(btn_dismiss, &QPushButton::clicked, this, &GUI_Broadcast::dismiss_clicked);
	connect(btn_dismiss_all, &QPushButton::clicked, this, &GUI_Broadcast::dismiss_all_clicked);
	connect(combo_clients, combo_current_index_changed_int, this, &GUI_Broadcast::combo_changed);
	connect(btn_retry, &QPushButton::clicked, this, &GUI_Broadcast::retry);

	set_status_label();

	REGISTER_LISTENER(SetNoDB::MP3enc_found, mp3_enc_found);
}

void GUI_Broadcast::set_status_label(){

	if(!is_ui_initialized()){
		return;
	}

	int n_listeners = combo_clients->count();
	QString str_listeners;

	if(n_listeners == 1){
		str_listeners = tr("%1 listener").arg(n_listeners);
	}

	else{
		str_listeners = tr("%1 listeners").arg(n_listeners);
	}

	lab_status->setText(str_listeners);
}

// finally connection is established
void GUI_Broadcast::new_connection(const QString& ip){

	if(!is_ui_initialized()){
		return;
	}

	//sp_log(Log::Debug) << "Add item " << ip;
	combo_clients->addItem(ip);
	set_status_label();
	combo_clients->setCurrentIndex(combo_clients->count() -1);
	btn_dismiss_all->setEnabled(true);
}


void GUI_Broadcast::connection_closed(const QString& ip){

	if(!is_ui_initialized()){
		return;
	}

	int idx;

	if(ip.isEmpty()) return;
	sp_log(Log::Info) << "Connection closed: " << ip;

	for(idx=0; idx<combo_clients->count(); idx++){
		if(combo_clients->itemText(idx).contains(ip)){
			break;
		}
	}

	if(idx >= combo_clients->count()){
		return;
	}

	combo_clients->removeItem(idx);

	if(combo_clients->count() == 0){
		btn_dismiss->setEnabled(false);
		btn_dismiss_all->setEnabled(false);
	}

	set_status_label();
}

void GUI_Broadcast::can_listen(bool success){

	if(!is_ui_initialized()){
		return;
	}

	lab_status->setVisible(success);
	lab_error->setVisible(!success);
	btn_retry->setVisible(!success);

	if(!success){
		QString msg = tr("Cannot broadcast on port %1").arg(_settings->get(Set::Broadcast_Port));
		msg += "\n" + tr("Maybe another application is using this port?");

		Message::warning(msg);
	}
}

void GUI_Broadcast::retry(){
	_server->retry();
}


void GUI_Broadcast::dismiss_at(int idx){

	if(!is_ui_initialized()){
		return;
	}

	QString ip = combo_clients->itemText(idx);

	if(ip.startsWith("(d)")) return;

	combo_clients->setItemText(idx, QString("(d) ") + ip);

	_server->dismiss(idx);
}


void GUI_Broadcast::dismiss_clicked(){

	int idx = combo_clients->currentIndex();
	dismiss_at(idx);
	btn_dismiss->setEnabled(false);

}


void GUI_Broadcast::dismiss_all_clicked(){

	for(int idx = 0; idx <combo_clients->count(); idx++){
		dismiss_at(idx);
	}

	btn_dismiss_all->setEnabled(false);
	btn_dismiss->setEnabled(false);
}

void GUI_Broadcast::combo_changed(int idx){

	Q_UNUSED(idx)
	QString text = combo_clients->currentText();

	if(text.startsWith("(d)")){
		btn_dismiss->setEnabled(false);
	}
	else{
		btn_dismiss->setEnabled(true);
	}
}



void GUI_Broadcast::mp3_enc_found(){

	if(!is_ui_initialized()){
		return;
	}

	bool active = _settings->get(SetNoDB::MP3enc_found);
	if(!active){
		combo_clients->hide();
		btn_dismiss->hide();
		btn_dismiss_all->hide();
		lab_status->hide();
		lab_error->setText(tr("Cannot find lame mp3 encoder"));
	}

	else{
		lab_error->hide();
		btn_retry->hide();
	}
}
