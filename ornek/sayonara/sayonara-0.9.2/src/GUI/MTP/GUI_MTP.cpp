/* GUI_MTP.cpp */

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



#include "GUI_MTP.h"

#include "GUI/Helper/Message/Message.h"
#include "GUI/Helper/Delegates/TreeDelegate.h"
#include "GUI/Helper/GUI_Helper.h"

#include <thread>
#include <functional>


static void open_device(GUI_MTP* mtp, MTP_RawDevicePtr raw_device){
	MTP_DevicePtr device = raw_device->open();
	mtp->device_opened(device);
}


GUI_MTP::GUI_MTP(QWidget* parent) :
	SayonaraDialog(parent),
	Ui::GUI_MTP()
{
	_initialized = false;
}


GUI_MTP::~GUI_MTP()
{
	if(_open_devices.size() > 0){
		sp_log(Log::Debug) << "Devices open: " << _open_devices[0].use_count();
	}

	if(_raw_devices.size() > 0){
		sp_log(Log::Debug) << "Raw Devices open: " << _raw_devices[0].use_count();
	}

	if(_mtp_copy_files){
		delete _mtp_copy_files;
	}

	_files.clear();
	_folders.clear();
	_storages.clear();
	_open_devices.clear();
	_raw_devices.clear();
}


void GUI_MTP::refresh_clicked(){

	_raw_devices.clear();
	_open_devices.clear();
	_storages.clear();
	_folders.clear();
	_files.clear();

	if(_mtp == nullptr){
		_mtp = new MTP(this);

		connect(_mtp, &MTP::finished, this, &GUI_MTP::scan_thread_finished);
	}

	btn_go->setEnabled(false);
	btn_go->setText("Initializing...");
	_mtp->start();
}

void GUI_MTP::delete_clicked(){

	for(QTreeWidgetItem* item : tree_view->selectedItems()){
		quint32 id = item->data(1, 0).toInt();
		if(id == 0){
			continue;
		}

		if(!_files.contains(id)){
			continue;
		}

		MTP_FilePtr file = _files[id];
		file->remove();
		_files.remove(id);

		item->setDisabled(true);
		item->setSelected(false);
		_storages[combo_storages->currentIndex()]->remove_id(id);
		folder_idx_changed(item->parent(), 0);
		tree_view->removeItemWidget(item, 0);
	}

	for(QTreeWidgetItem* item : tree_view->selectedItems()){
		quint32 id = item->data(1, 0).toInt();
		if(id == 0){
			continue;
		}

		if(!_folders.contains(id)){
			continue;
		}

		MTP_FolderPtr folder = _folders[id];
		folder->remove();
		_folders.remove(id);

		item->setDisabled(true);
		item->setSelected(false);
		_storages[combo_storages->currentIndex()]->remove_id(id);
		folder_idx_changed(item->parent(), 0);
		tree_view->removeItemWidget(item, 0);

	}
}


void GUI_MTP::scan_thread_finished(){

	combo_devices->clear();

	sp_log(Log::Debug) << "Scan thread finished";

	btn_go->setEnabled(true);
	btn_go->setText("Refresh");

	QList<MTP_RawDevicePtr> raw_devices = _mtp->get_raw_devices();
	if(raw_devices.size() == 0){
		Message::warning("No devices found");
		return;
	}

	for(MTP_RawDevicePtr raw_device : raw_devices){
		QString device_string = raw_device->get_device_string();

		_raw_devices << raw_device;
		combo_devices->addItem(device_string);
	}
}


void GUI_MTP::device_idx_changed(int idx){

	_storages.clear();
	combo_storages->clear();

	if( !between(idx, _raw_devices) ){
		return;
	}

	MTP_RawDevicePtr raw_device = _raw_devices[idx];


	std::thread* open_thread = new std::thread(open_device, this, raw_device);
	Q_UNUSED(open_thread);

	btn_go->setText(tr("Opening") + "...");
	btn_go->setDisabled(true);

}

void GUI_MTP::device_opened(MTP_DevicePtr device){

	btn_go->setText(tr("Refresh"));
	btn_go->setDisabled(false);
	if(!device){

		//Message::warning("Cannot open device");
		return;
	}

	_open_devices << device;

	QList<MTP_StoragePtr> storages = device->storages();
	sp_log(Log::Debug) << "Device " << device->id() << " has " << storages.size() << " storages";

	for(MTP_StoragePtr storage : storages){

		QString name = storage->name() + ": " + storage->identifier();
		sp_log(Log::Debug) << "New Storage: " << name;
		_storages << storage;
		combo_storages->addItem(name);
	}

	if(combo_storages->currentIndex() == -1 && combo_storages->count() > 0){
		combo_storages->setCurrentIndex(0);
	}
}

void GUI_MTP::storage_idx_changed(int idx){

	if( !between(idx, _storages) ){
		return;
	}

	MTP_StoragePtr storage = _storages[idx];
	tree_view->clear();

	QList<MTP_FolderPtr> folders = storage->folders();
	sp_log(Log::Debug) << "Storage has " << folders.size() << " folders";
	enable_drag_drop(folders.size() > 0);

	for(MTP_FolderPtr folder : folders){

		_folders[folder->id()] = folder;
		QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << folder->name());

		item->setData(1, 0, folder->id());
		item->setText(0, folder->name());
		item->addChild(new QTreeWidgetItem());

		tree_view->addTopLevelItem(item);
	}
}

void GUI_MTP::folder_idx_expanded(QTreeWidgetItem *item){
	Q_UNUSED(item)
	//folder_idx_changed(item, 0);
}

void GUI_MTP::folder_idx_changed(QTreeWidgetItem* item, int column){

	Q_UNUSED(column)
	btn_delete->setEnabled(true);

	if(!item){
		return;
	}

	quint32 folder_id = item->data(1, 0).toInt();
	if(folder_id == 0){
		return;
	}

	MTP_FolderPtr folder = _folders[folder_id];
	QList<MTP_FolderPtr> children = folder->children();

	for(int i = item->childCount() - 1; i>=0; i--){
		item->takeChild(i);
	}

	for(MTP_FolderPtr folder : children){

		_folders[folder->id()] = folder;

		QTreeWidgetItem* child_item;

		child_item = new QTreeWidgetItem();

		child_item->setData(1, 0, folder->id());
		child_item->setText(0, folder->name());
		child_item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
		child_item->addChild(new QTreeWidgetItem());

		item->addChild(child_item);
	}

	MTP_StoragePtr storage = _storages[combo_storages->currentIndex()];
	QList<MTP_FilePtr> files = storage->files_of_folder(folder->id());

	for(MTP_FilePtr file : files){

		_files[file->id()] = file;

		QTreeWidgetItem* child_item = new QTreeWidgetItem();

		child_item->setText(0, file->filename());
		child_item->setData(1, 0, file->id());
		child_item->setIcon(0, GUI::get_icon("undo"));
		child_item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);

		item->addChild(child_item);
	}
}

void GUI_MTP::enable_drag_drop(bool b){

	tree_view->setAcceptDrops(b);
	tree_view->setDragEnabled(b);
	this->setAcceptDrops(b);

}

void GUI_MTP::dragEnterEvent(QDragEnterEvent* e)
{
	e->accept();
}

void GUI_MTP::dragMoveEvent(QDragMoveEvent* e)
{
	e->accept();

	QPoint pos = e->pos();
	tree_view->clearSelection();

	QTreeWidgetItem* item = tree_view->itemAt(pos - tree_view->pos());

	if(!item){
		return;
	}

	item->setSelected(true);
}

void GUI_MTP::dragLeaveEvent(QDragLeaveEvent* e)
{
	e->accept();
}

void GUI_MTP::dropEvent(QDropEvent* e)
{
	e->accept();
	quint32 folder_id;
	MetaDataList v_md;
	QTreeWidgetItem* item;
	MTP_FolderPtr folder;
	const QMimeData* mime_data;
	const CustomMimeData* cmd;

	QPoint pos;

	pos = e->pos();
	item = tree_view->itemAt(pos - tree_view->pos());

	if(!item){
		return;
	}

	folder_id = item->data(1, 0).toInt();
	if(folder_id == 0){
		return;
	}

	folder = _folders[folder_id];
	sp_log(Log::Debug) << "Will drop into folder " << folder->name();

	mime_data = e->mimeData();
	if(!mime_data){
		sp_log(Log::Debug) << "Cannot drop metadata";
		return;
	}

	cmd = static_cast<const CustomMimeData*>(mime_data);
	if(cmd){


		if(_mtp_copy_files){
			delete _mtp_copy_files;
			_mtp_copy_files = nullptr;
		}


		enable_drag_drop(false);

		v_md = cmd->getMetaData();

		if(v_md.isEmpty() && cmd->hasUrls()){
			QList<QUrl> urls = mime_data->urls();
			QStringList str_urls;

			for(const QUrl& url : urls){
				str_urls << url.toLocalFile();
			}

			_mtp_copy_files = new MTP_CopyFiles(str_urls, folder, nullptr);
			sp_log(Log::Debug) << "Will drop " << str_urls.size() << " files ";
		}

		else{
			_mtp_copy_files = new MTP_CopyFiles(v_md, folder, nullptr);
			sp_log(Log::Debug) << "Will drop " << v_md.size() << " Tracks ";
		}

		btn_delete->setEnabled(false);
		btn_go->setEnabled(false);

		connect(_mtp_copy_files, &MTP_CopyFiles::sig_progress, this, &GUI_MTP::progress_changed);
		connect(_mtp_copy_files, &MTP_CopyFiles::finished, this, &GUI_MTP::copy_thread_finished);

		_mtp_copy_files->start();
	}

}

void GUI_MTP::showEvent(QShowEvent* e)
{
	if(!_initialized){
		setupUi(this);

		tree_view->setExpandsOnDoubleClick(true);
		tree_view->setAnimated(true);
		tree_view->setAutoExpandDelay(100);
		tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
		tree_view->setItemDelegate(new TreeDelegate(this));

		pb_progress->hide();

		enable_drag_drop(false);

		_mtp = nullptr;
		_mtp_copy_files = nullptr;

		connect(tree_view, &QTreeWidget::itemActivated, this, &GUI_MTP::folder_idx_changed);
		connect(tree_view, &QTreeWidget::itemExpanded, this, &GUI_MTP::folder_idx_expanded);
		connect(combo_devices, combo_current_index_changed_int, this, &GUI_MTP::device_idx_changed);
		connect(combo_storages, combo_current_index_changed_int, this, &GUI_MTP::storage_idx_changed);
		connect(btn_go, &QPushButton::clicked, this, &GUI_MTP::refresh_clicked);
		connect(btn_delete, &QPushButton::clicked, this, &GUI_MTP::delete_clicked);

		btn_delete->setEnabled(false);
		_initialized = true;
	}

	SayonaraDialog::showEvent(e);
}

void GUI_MTP::progress_changed(int progress){
	if(progress == -1){
		pb_progress->hide();
	}

	else{
		pb_progress->show();
		pb_progress->setValue(progress);
	}
}

void GUI_MTP::copy_thread_finished(){
	pb_progress->hide();
	enable_drag_drop(true);
	btn_delete->setEnabled(true);
	btn_go->setEnabled(true);
}
