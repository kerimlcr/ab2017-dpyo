/* GUI_InfoDialog.h

 * Copyright (C) 2012-2016 Lucio Carreras
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras,
 * Jul 19, 2012
 *
 */

#ifndef GUI_INFODIALOG_H_
#define GUI_INFODIALOG_H_

#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Components/Covers/CoverLocation.h"

#include "GUI/InfoDialog/ui_GUI_InfoDialog.h"

#include <QCloseEvent>
#include <QDateTime>
#include <QTabBar>

class GUI_TagEdit;
class LyricLookupThread;
class InfoDialogContainer;

/**
 * @brief The GUI_InfoDialog class
 * @ingroup GUI
 */
class GUI_InfoDialog :
		public SayonaraDialog,
		private Ui::InfoDialog
{

	Q_OBJECT

public:

	enum class Tab : quint8 {
		Info=0,
		Lyrics=1,
		Edit=2
	};

	GUI_InfoDialog(InfoDialogContainer* container, QWidget* parent=nullptr);
	virtual ~GUI_InfoDialog();

	void set_metadata(const MetaDataList& vd, MetaDataList::Interpretation interpretation);
	bool has_metadata() const;
	void show(GUI_InfoDialog::Tab tab);


private slots:
	void lyrics_fetched();
	void lyric_server_changed(int idx);

	void tab_index_changed_int(int idx);
	void tab_index_changed(GUI_InfoDialog::Tab idx);

	void skin_changed() override;
	void language_changed() override;


private:

	InfoDialogContainer*	_info_dialog_container=nullptr;
	GUI_TagEdit*			_ui_tag_edit=nullptr;
	LyricLookupThread*		_lyric_thread=nullptr;

	MetaDataList::Interpretation	_md_interpretation;

	QString 				_cover_artist;
	QString					_cover_album;
	CoverLocation			_cl;

	MetaDataList			_v_md;
	bool					_is_initialized;


private:

	void init();

	void prepare_cover(const CoverLocation& cover_path);
	void prepare_lyrics();
	void prepare_info(MetaDataList::Interpretation mode);

	void closeEvent(QCloseEvent *e) override;
	void showEvent(QShowEvent *e) override;

};

#endif /* GUI_INFODIALOG_H_ */
