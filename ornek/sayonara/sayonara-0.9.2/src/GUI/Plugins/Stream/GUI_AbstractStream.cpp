/* GUI_AbstractStream.cpp */

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

#include "GUI_AbstractStream.h"
#include "GUI/Helper/Message/Message.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"

#include "Helper/MetaData/MetaData.h"
#include "Helper/Parser/PlaylistParser.h"
#include "Helper/Parser/PodcastParser.h"

#include "Components/StreamPlugins/Streams/AbstractStreamHandler.h"
#include "Database/DatabaseConnector.h"

#include <QAbstractItemView>
#include <QCompleter>

GUI_AbstractStream::GUI_AbstractStream(AbstractStreamHandler* stream_handler, QWidget* parent) :
	PlayerPluginInterface(parent)
{
	_db = DatabaseConnector::getInstance();

	_stream_handler = stream_handler;
}

GUI_AbstractStream::~GUI_AbstractStream(){

}

void GUI_AbstractStream::init_connections(){

	_combo_stream->setAutoCompletion(false);

	_combo_stream->setFocusPolicy(Qt::StrongFocus);
	_le_url->setFocusPolicy(Qt::StrongFocus);
	_btn_play->setFocusPolicy(Qt::StrongFocus);

	setTabOrder(_combo_stream, _le_url);
	setTabOrder(_le_url, _btn_play);

	_btn_tool->show_action(ContextMenu::EntryNew, true);

	connect(_btn_play, &QPushButton::clicked, this, &GUI_AbstractStream::listen_clicked);
	connect(_btn_tool, &MenuToolButton::sig_save, this, &GUI_AbstractStream::save_clicked);
	connect(_btn_tool, &MenuToolButton::sig_delete, this, &GUI_AbstractStream::delete_clicked);
	connect(_btn_tool, &MenuToolButton::sig_new, this, &GUI_AbstractStream::new_clicked);

	connect(_combo_stream, combo_activated_int, this, &GUI_AbstractStream::combo_idx_changed);
	connect(_combo_stream, &QComboBox::editTextChanged, this, &GUI_AbstractStream::text_changed);

	connect(_le_url, &QLineEdit::textChanged, this, &GUI_AbstractStream::text_changed);

	connect(_stream_handler, &AbstractStreamHandler::sig_error, this, &GUI_AbstractStream::error);
	connect(_stream_handler, &AbstractStreamHandler::sig_data_available, this, &GUI_AbstractStream::data_available);
}

void GUI_AbstractStream::init_streams(){

	StreamMap data;

	if( _stream_handler->get_all_streams(data) ){
		setup_stations(data);
	}
}


void GUI_AbstractStream::language_changed(){

}

void GUI_AbstractStream::init_ui()
{
	_btn_play->setMinimumSize(QSize(24,24));
	_btn_play->setMaximumSize(QSize(24,24));
	_btn_tool->setToolTip(tr("Menu"));
	_btn_tool->setText(tr("Menu"));

	REGISTER_LISTENER(Set::Player_Style, _sl_skin_changed);
}

void GUI_AbstractStream::error(){

	_btn_play->setDisabled(false);
	_lab_listen->setText(tr("Listen"));

	sp_log(Log::Info) << "Stream Handler error";
	Message::warning(tr("Cannot open stream") + "\n" + _le_url->text());

}

void GUI_AbstractStream::data_available(){

	_btn_play->setDisabled(false);
	_lab_listen->setText(tr("Listen"));
}

void GUI_AbstractStream::_sl_skin_changed()
{
	_btn_play->setIcon( IconLoader::getInstance()->get_icon("media-playback-start", "play"));
}


void GUI_AbstractStream::play(QString url, QString station_name){

	bool success = _stream_handler->parse_station(url, station_name);
	if(!success){
		sp_log(Log::Info) << "Stream Handler busy";
		_btn_play->setEnabled(true);
		_lab_listen->setEnabled(true);
		_lab_listen->setText(tr("Play"));
	}
}


void GUI_AbstractStream::listen_clicked(){
	QString name, url;

	if( _combo_stream->currentIndex() <= 0) {
		url = _le_url->text();
		name = get_title_fallback_name();
	}

	else{
		url = _le_url->text();
		name = _combo_stream->currentText();
	}

	url = url.trimmed();
	if(url.size() > 5) {

		play(url, name);
		_btn_play->setDisabled(true);
		_lab_listen->setText(tr("Busy..."));
	}
}

void GUI_AbstractStream::combo_idx_changed(int idx){

	QString cur_station_name = _combo_stream->currentText();
	QString address = _stations[cur_station_name];
	bool listen_enabled;

	if(address.size() > 0) {
		_le_url->setText(address);
	}

	if(idx == 0) {
		_le_url->setText("");
	}

	listen_enabled = (_le_url->text().size() > 5);

	_btn_tool->show_action(ContextMenu::EntryDelete, idx > 0);

	_btn_play->setEnabled(listen_enabled);
	_lab_listen->setEnabled(listen_enabled);
	_combo_stream->setToolTip(address);
}


GlobalMessage::Answer GUI_AbstractStream::show_delete_confirm_dialog(){

	QString cur_station_name = _combo_stream->currentText();
	return Message::question_yn(tr("Do you really want to delete %1").arg(cur_station_name));
}

void GUI_AbstractStream::new_clicked(){

	if(_combo_stream->count() > 0){
		_combo_stream->setCurrentIndex(0);
		_combo_stream->setItemText(0, "");
	}

	else{
		_combo_stream->addItem("");
		_combo_stream->setCurrentIndex(0);
	}

	_le_url->setText("");
}

void GUI_AbstractStream::text_changed(const QString& str)
{
	Q_UNUSED(str)

	bool listen_enabled = (!_le_url->text().isEmpty());

	_btn_play->setEnabled(listen_enabled);
	_lab_listen->setEnabled(listen_enabled);

	_btn_tool->show_action(ContextMenu::EntrySave,  listen_enabled &&
						  !_combo_stream->currentText().isEmpty());
}

void GUI_AbstractStream::delete_clicked(){

	if(_combo_stream->currentIndex() <= 0) return;

	QString cur_station_name = _combo_stream->currentText();
	GlobalMessage::Answer ret = show_delete_confirm_dialog();

	if(ret == GlobalMessage::Answer::Yes) {

		if( _stream_handler->delete_stream(cur_station_name) ) {

			StreamMap map;
			sp_log(Log::Info) << cur_station_name << "successfully deleted";

			if( _stream_handler->get_all_streams(map) ) {
				setup_stations(map);
			}
		}
	}
}

void GUI_AbstractStream::save_clicked(){

	QString name = _combo_stream->currentText();
	QString url = _le_url->text();
	GlobalMessage::Answer answer;

	if(name.isEmpty() || url.isEmpty()){
		return;
	}

	StreamMap map;


	for(int i=0; i<_combo_stream->count(); i++){
		QString text = _combo_stream->itemText(i);
		if(text == name){
			answer = Message::question_yn(tr("Overwrite?") + "\n" + name + "\n" + url);
			if(answer == GlobalMessage::Answer::Yes){
				_stream_handler->update_url(name, url);
				_stream_handler->get_all_streams(map);
				setup_stations(map);
				return;
			}

			else{
				return;
			}
		}
	}

	_stream_handler->save(name, url);
	_stream_handler->get_all_streams(map);
	setup_stations(map);
}

void GUI_AbstractStream::setup_stations(const StreamMap& stations){

	QString old_name = _combo_stream->currentText();
	QString old_url = _le_url->text();

	_combo_stream->clear();
	_le_url->clear();

	_stations = stations;
	_stations[""] = "";

	if(_stations.size() == 1){
		_combo_stream->setCurrentIndex(0);
	}

	for(auto it = _stations.begin(); it != _stations.end(); it++) {
		_combo_stream->addItem(it.key(), it.value());
	}

	_combo_stream->view()->setItemDelegate(new ComboBoxDelegate(this));

	_btn_play->setEnabled(false);
	_lab_listen->setEnabled(false);
	_btn_tool->show_action(ContextMenu::EntrySave, false);
	_btn_tool->show_action(ContextMenu::EntryDelete, false);

	_combo_stream->setCurrentText(old_name);
	_le_url->setText(old_url);
}

