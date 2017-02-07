/* LibraryPluginHandler.cpp */

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



#include "LibraryPluginHandler.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"
#include "LibraryContainer/LibraryContainer.h"

#include "Helper/Helper.h"

#include <QLayout>

LibraryPluginHandler::LibraryPluginHandler(QObject* parent) :
	QObject(parent),
	SayonaraClass()
{
	_cur_idx = -1;
	_library_parent = nullptr;
	REGISTER_LISTENER(Set::Player_Language, language_changed);
}


LibraryPluginHandler::~LibraryPluginHandler()
{

}


void LibraryPluginHandler::init(const QList<LibraryContainerInterface*>& containers)
{

	QString lib_dir;
	QDir plugin_dir;
	QStringList dll_filenames;
	QString cur_plugin;

	lib_dir = Helper::get_lib_path();
	plugin_dir = QDir(lib_dir);
	dll_filenames = plugin_dir.entryList(QDir::Files);
	cur_plugin = _settings->get(Set::Lib_CurPlugin);


	for(LibraryContainerInterface* container : containers){

		if(!container){
			continue;
		}

		sp_log(Log::Debug) << "Add plugin " << container->get_display_name();

		_libraries << container;
	}

	for(const QString& filename : dll_filenames) {

		QObject* raw_plugin;
		LibraryContainerInterface* container;

		QPluginLoader loader(plugin_dir.absoluteFilePath(filename));

		raw_plugin = loader.instance();
		if(!raw_plugin) {
			sp_log(Log::Warning) << "Cannot load plugin: " << filename << ": " << loader.errorString();
			loader.unload();
			continue;
		}

		container = dynamic_cast<LibraryContainerInterface*>(raw_plugin);
		if(!container) {
			loader.unload();
			continue;
		}

		sp_log(Log::Info) << "Found library plugin " << container->get_display_name();
		_libraries << container;
	}

	sp_log(Log::Info) << "Found " << _libraries.size() << " library types";

	int i=0;
	for(LibraryContainerInterface* container : _libraries ){

		if(cur_plugin == container->get_name()){
			_cur_idx = i;
			init_library(i);
			emit sig_idx_changed(i);
			break;
		}

		i++;
	}
}



void LibraryPluginHandler::init_library(int idx)
{
	LibraryContainerInterface* library = _libraries[idx];
	if(library->is_initialized()){
		return;
	}

	QWidget* ui=nullptr;
	QLayout* layout;
	QComboBox* libchooser;

	library->init_ui();
	library->set_initialized();
	ui = library->get_ui();
	ui->setParent(_library_parent);

	layout = ui->layout();
	if(layout){
		layout->setContentsMargins(5, 0, 8, 0);
	}

	libchooser = library->get_libchooser();
	libchooser->setIconSize(QSize(16,16));
	libchooser->setMinimumWidth(200);
	libchooser->setMaximumWidth(200);
	libchooser->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

	for(LibraryContainerInterface* other_library : _libraries){
		libchooser->addItem(other_library->get_icon(), other_library->get_display_name());
	}

	libchooser->setCurrentIndex(idx);
	libchooser->setItemDelegate(new ComboBoxDelegate(ui));

	connect(libchooser, combo_activated_int, this, &LibraryPluginHandler::index_changed);
}



void LibraryPluginHandler::index_changed(int idx){

	_cur_idx = idx;

	init_library(_cur_idx);

	int i=0;
	for(LibraryContainerInterface* container : _libraries){


		if(!container->is_initialized()){
			i++;
			continue;
		}

	
		QWidget* ui, *parent;
		QString name;

		QComboBox* libchooser = container->get_libchooser();
		libchooser->setItemIcon(i, container->get_icon());

		ui = container->get_ui();
		parent = ui->parentWidget();
		name = container->get_display_name();
		if(i == idx){
			ui->setVisible(true);

			libchooser->setCurrentIndex(i);

			if(parent){
				ui->resize(parent->size());
			}

			ui->update();

			_settings->set(Set::Lib_CurPlugin, container->get_name());
		}

		else{
			ui->setVisible(false);
		}

		i++;
	}

	emit sig_idx_changed(idx);
}

LibraryContainerInterface* LibraryPluginHandler::get_cur_library() const {
	if(!between(_cur_idx, _libraries)) {
		return nullptr;
	}

	return _libraries[_cur_idx];
}

int LibraryPluginHandler::get_cur_library_idx() const
{
	return _cur_idx;
}

void LibraryPluginHandler::set_library_parent(QWidget* parent)
{
	_library_parent = parent;

	for(LibraryContainerInterface* container : _libraries){
		if(container->is_initialized()){
			container->get_ui()->setParent(parent);
		}
	}
}

void LibraryPluginHandler::language_changed(){

	for(LibraryContainerInterface* container : _libraries){

		if(!container->is_initialized()){
			continue;
		}

		QComboBox* libchooser = container->get_libchooser();
		int i=0;

		for(LibraryContainerInterface* container2 : _libraries){
			libchooser->setItemText(i, container2->get_display_name());
			i++;
		}
	}
}


QList<LibraryContainerInterface*> LibraryPluginHandler::get_libraries() const {
	return _libraries;
}

