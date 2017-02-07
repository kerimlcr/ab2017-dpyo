/* GUI_TagEdit.cpp */

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


#include "GUI_TagEdit.h"
#include "TagLineEdit.h"
#include "Components/TagEdit/TagExpression.h"
#include "Components/Covers/CoverLocation.h"
#include "GUI/Helper/Message/Message.h"
#include "Helper/Tagging/Tagging.h"

#include "Components/TagEdit/TagEdit.h"

#include <QDir>
#include <QDesktopServices>

GUI_TagEdit::GUI_TagEdit(QWidget* parent) :
	SayonaraWidget(parent)
{
	setupUi(this);

	_tag_edit = new TagEdit(this);

	frame_tag_from_path->setVisible(cb_tag_from_path->isChecked());

	connect(btn_next, &QPushButton::clicked, this, &GUI_TagEdit::next_button_clicked);
	connect(btn_prev, &QPushButton::clicked, this, &GUI_TagEdit::prev_button_clicked);
	connect(btn_apply_tag, &QPushButton::clicked, this, &GUI_TagEdit::apply_tag_clicked);
	connect(btn_apply_tag_all, &QPushButton::clicked, this, &GUI_TagEdit::apply_tag_all_clicked);
	connect(rb_dont_replace, &QRadioButton::toggled, this, &GUI_TagEdit::rb_dont_replace_toggled);

	connect(cb_album_all, &QCheckBox::toggled, this, &GUI_TagEdit::album_all_changed);
	connect(cb_artist_all, &QCheckBox::toggled, this, &GUI_TagEdit::artist_all_changed);
	connect(cb_genre_all, &QCheckBox::toggled, this, &GUI_TagEdit::genre_all_changed);
	connect(cb_year_all, &QCheckBox::toggled, this, &GUI_TagEdit::year_all_changed);
	connect(cb_discnumber_all, &QCheckBox::toggled, this, &GUI_TagEdit::discnumber_all_changed);
	connect(cb_rating_all, &QCheckBox::toggled, this, &GUI_TagEdit::rating_all_changed);
	connect(cb_cover_all, &QCheckBox::toggled, this, &GUI_TagEdit::cover_all_changed);
	connect(le_tag, &QLineEdit::textChanged, this, &GUI_TagEdit::tag_text_changed);

	connect(btn_title, &QPushButton::toggled, this, &GUI_TagEdit::btn_title_checked);
	connect(btn_artist, &QPushButton::toggled, this, &GUI_TagEdit::btn_artist_checked);
	connect(btn_album, &QPushButton::toggled, this, &GUI_TagEdit::btn_album_checked);
	connect(btn_track_nr, &QPushButton::toggled, this, &GUI_TagEdit::btn_track_nr_checked);
	connect(btn_year, &QPushButton::toggled, this, &GUI_TagEdit::btn_year_checked);
	connect(btn_disc_nr, &QPushButton::toggled, this, &GUI_TagEdit::btn_disc_nr_checked);
	connect(btn_tag_help, &QPushButton::clicked, this, &GUI_TagEdit::btn_tag_help_clicked);

	connect(_tag_edit, &TagEdit::sig_progress, this, &GUI_TagEdit::progress_changed);
	connect(_tag_edit, &TagEdit::sig_metadata_received, this, &GUI_TagEdit::metadata_changed);
	connect(_tag_edit, &TagEdit::finished, this, &GUI_TagEdit::commit_finished);

	connect(btn_ok, &QPushButton::clicked, this, &GUI_TagEdit::commit);
	connect(btn_undo, &QPushButton::clicked, this, &GUI_TagEdit::undo_clicked);
	connect(btn_undo_all, &QPushButton::clicked, this, &GUI_TagEdit::undo_all_clicked);
	connect(btn_cancel, &QPushButton::clicked, this, &GUI_TagEdit::cancel);

	reset();
}


GUI_TagEdit::~GUI_TagEdit(){

}

void GUI_TagEdit::language_changed(){
	retranslateUi(this);
}

void GUI_TagEdit::commit_finished()
{
	btn_ok->setEnabled(true);
	btn_cancel->setEnabled(true);
}

TagEdit* GUI_TagEdit::get_tag_edit() const
{
	return _tag_edit;
}

void GUI_TagEdit::cancel(){

	undo_all_clicked();

	emit sig_cancelled();
}

void GUI_TagEdit::progress_changed(int val){

	pb_progress->setVisible(val >= 0);

	if(val >= 0){
		pb_progress->setValue(val);
	}

	if(val < 0){
		metadata_changed( _tag_edit->get_all_metadata() );
	}
}

void GUI_TagEdit::metadata_changed(const MetaDataList& md){

	Q_UNUSED(md)

	reset();

	_cur_idx = 0;
	track_idx_changed();
}


bool GUI_TagEdit::check_idx(int idx) const
{
	return between(idx, _tag_edit->get_n_tracks());
}


void GUI_TagEdit::next_button_clicked(){

	write_changes(_cur_idx);

	_cur_idx++;

	track_idx_changed();
}


void GUI_TagEdit::prev_button_clicked(){

	write_changes(_cur_idx);

	_cur_idx--;

	track_idx_changed();
}

void GUI_TagEdit::track_idx_changed(){

	bool valid;
	int n_tracks = _tag_edit->get_n_tracks();

	btn_next->setEnabled(_cur_idx >= 0 && _cur_idx < n_tracks - 1);
	btn_prev->setEnabled(_cur_idx > 0 && _cur_idx < n_tracks);

	if(!check_idx(_cur_idx)) return;

	MetaData md = _tag_edit->get_metadata(_cur_idx);

	if(le_tag->text().isEmpty()){

		le_tag->setText(md.filepath());
	}

	else if( !(btn_album->isChecked() ||
			btn_artist->isChecked() ||
			btn_title->isChecked() ||
			btn_year->isChecked() ||
			btn_disc_nr->isChecked() ||
			btn_track_nr->isChecked()))
	{
		le_tag->setText(md.filepath());
	}

	valid = _tag_expression.update_tag(le_tag->text(), md.filepath());
	set_tag_colors( valid );

	le_title->setText(md.title);

	if(!cb_album_all->isChecked()){
		le_album->setText(md.album);
	}

	if(!cb_artist_all->isChecked()){
		le_artist->setText(md.artist);
	}

	if(!cb_genre_all->isChecked()){
		QStringList genres;
		for(const QString& genre : md.genres){
			if(!genre.trimmed().isEmpty()){
				genres << genre;
			}
		}

		le_genre->setText( genres.join(", ") );
	}

	if(!cb_year_all->isChecked()){
		sb_year->setValue(md.year);
	}

	if(!cb_discnumber_all->isChecked()){
		sb_discnumber->setValue(md.discnumber);
	}

	if(!cb_rating_all->isChecked()){
		lab_rating->set_rating(md.rating);
	}

	if(!cb_cover_all->isChecked()){
		set_cover(md);

		if(_tag_edit->has_cover_replacement(_cur_idx)){
			rb_replace->setChecked(true);
			rb_dont_replace->setChecked(false);
		}
	
		else{
			rb_dont_replace->setChecked(true);
			rb_replace->setChecked(false);
		}
	}

	bool is_id3v2 = _tag_edit->is_id3v2_tag(_cur_idx);
	frame_cover->setVisible(is_id3v2);

	sb_track_num->setValue(md.track_num);

	lab_filepath->clear();
	lab_filepath->setText( md.filepath() );
	lab_track_num->setText(
			tr("Track ") +
			QString::number( _cur_idx+1 ) + "/" + QString::number( n_tracks )
	);
}

void GUI_TagEdit::reset(){

	_cur_idx = -1;

	cb_album_all->setChecked(false);
	cb_artist_all->setChecked(false);
	cb_genre_all->setChecked(false);
	cb_discnumber_all->setChecked(false);
	cb_rating_all->setChecked(false);
	cb_year_all->setChecked(false);
	cb_cover_all->setChecked(false);

	lab_track_num ->setText(tr("Track ") + "0/0");
	btn_prev->setEnabled(false);
	btn_next->setEnabled(false);

	le_album->clear();
	le_artist->clear();
	le_title->clear();
	le_genre->clear();
	le_tag->clear();
	sb_year->setValue(0);
	sb_discnumber->setValue(0);
	lab_rating->set_rating(0);
	sb_track_num->setValue(0);

	le_album->setEnabled(true);
	le_artist->setEnabled(true);
	le_genre->setEnabled(true);
	le_tag->setEnabled(true);
	sb_year->setEnabled(true);
	sb_discnumber->setEnabled(true);
	lab_rating->setEnabled(true);

	rb_dont_replace->setChecked(true);
	rb_replace->setChecked(false);

	btn_cover_replacement->setEnabled(true);
	show_replacement_field(false);

	QIcon icon(CoverLocation::getInvalidLocation().cover_path());
	btn_cover_replacement->setIcon( icon );

	lab_filepath->clear();
	pb_progress->setVisible(false);

	btn_album->setChecked(false);
	btn_artist->setChecked(false);
	btn_title->setChecked(false);
	btn_year->setChecked(false);
	btn_disc_nr->setChecked(false);
	btn_track_nr->setChecked(false);

	_cover_path_map.clear();

}

void GUI_TagEdit::album_all_changed(bool b){
	le_album->setEnabled(!b);
}

void GUI_TagEdit::artist_all_changed(bool b){
	le_artist->setEnabled(!b);
}

void GUI_TagEdit::genre_all_changed(bool b){
	le_genre->setEnabled(!b);
}

void GUI_TagEdit::year_all_changed(bool b){
	sb_year->setEnabled(!b);
}

void GUI_TagEdit::discnumber_all_changed(bool b){
	sb_discnumber->setEnabled(!b);
}

void GUI_TagEdit::rating_all_changed(bool b){
	lab_rating->setEnabled(!b);
}

void GUI_TagEdit::cover_all_changed(bool b){

	if(!b){
		if(between(_cur_idx, _tag_edit->get_n_tracks()) ){
			set_cover(_tag_edit->get_metadata(_cur_idx));
		}
	}

	rb_dont_replace->setEnabled(!b);

	btn_cover_replacement->setEnabled(!b);
}

void GUI_TagEdit::undo_clicked(){

	_tag_edit->undo(_cur_idx);
	track_idx_changed();
}

void GUI_TagEdit::undo_all_clicked(){
	_tag_edit->undo_all();
	track_idx_changed();
}


void GUI_TagEdit::write_changes(int idx){

	if( !check_idx(idx) ) {
		return;
	}

	MetaData md = _tag_edit->get_metadata(idx);

	md.title = le_title->text();
	md.artist = le_artist->text();
	md.album = le_album->text();
	md.genres = le_genre->text().split(", ");
	md.discnumber = sb_discnumber->value();
	md.year = sb_year->value();
	md.track_num = sb_track_num->value();
	md.rating = lab_rating->get_rating();

	_tag_edit->update_track(idx, md);

	if(is_cover_replacement_active()){
		update_cover(idx, _cover_path_map[idx]);
	}
}

void GUI_TagEdit::commit(){

	if(!btn_ok->isEnabled()){
		return;
	}

	btn_ok->setEnabled(false);
	btn_cancel->setEnabled(false);
	btn_undo->setEnabled(false);
	btn_undo_all->setEnabled(false);

	write_changes(_cur_idx);

	for(int i=0; i<_tag_edit->get_n_tracks(); i++){

		if(i == _cur_idx) continue;

		MetaData md = _tag_edit->get_metadata(i);

		if( cb_album_all->isChecked()){
			md.album = le_album->text();
		}
		if( cb_artist_all->isChecked()){
			md.artist = le_artist->text();
		}
		if( cb_genre_all->isChecked()){
			md.genres = le_genre->text().split(", ");
		}

		if( cb_discnumber_all->isChecked() ){
			md.discnumber = sb_discnumber->value();
		}

		if( cb_rating_all->isChecked()){
			md.rating = lab_rating->get_rating();
		}

		if( cb_year_all->isChecked()){
			md.year = sb_year->value();
		}

		if( cb_cover_all->isChecked() ){

			update_cover(i, _cover_path_map[_cur_idx]);
		}

		_tag_edit->update_track(i, md);
	}

	_tag_edit->commit();
}

void GUI_TagEdit::show_replacement_field(bool b){
	lab_replacement->setVisible(b);
	btn_cover_replacement->setVisible(b);
	cb_cover_all->setVisible(b);
	cb_cover_all->setChecked(false);
}

bool GUI_TagEdit::is_cover_replacement_active() const
{
	return (rb_replace->isChecked() &&
			frame_cover->isVisible() &&
			btn_cover_replacement->isVisible());
}

void GUI_TagEdit::set_cover(const MetaData& md){

	QByteArray img_data;
	QString mime_type;
	bool has_cover = Tagging::extract_cover(md, img_data, mime_type);

	if(!has_cover){
		btn_cover_original->setIcon(QIcon());
		btn_cover_original->setText(tr("None"));
	}

	else{
		QImage img = QImage::fromData(img_data, mime_type.toLocal8Bit().data());
		QPixmap pm = QPixmap::fromImage(img);
		QIcon icon;
		icon.addPixmap(pm);
		btn_cover_original->setIcon(icon);
		btn_cover_original->setText(QString());
	}

	CoverLocation cl = CoverLocation::get_cover_location(md);
	btn_cover_replacement->set_cover_location(cl);

	cb_cover_all->setEnabled(cl.valid());
	btn_cover_replacement->setEnabled(cl.valid() && !cb_cover_all->isChecked());

	if(cl.valid()){
		_cover_path_map[_cur_idx] = cl.cover_path();
	}
}

void GUI_TagEdit::update_cover(int idx, const QString& cover_path){

	QImage img(cover_path);
	if(img.isNull()){
		return;
	}

	_tag_edit->update_cover(idx, img);
}


void GUI_TagEdit::rb_dont_replace_toggled(bool b){

	show_replacement_field(!b);
}


/*** TAG ***/

void GUI_TagEdit::set_tag_colors(bool valid){

	if( !valid ){
		le_tag->setStyleSheet("color: red;");
	}

	else{
		le_tag->setStyleSheet("");
	}

	btn_apply_tag->setEnabled(valid);
	btn_apply_tag_all->setEnabled(valid);
}

void GUI_TagEdit::tag_text_changed(const QString& str){

	if( !check_idx(_cur_idx) ) {
		return;
	}

	bool valid;
	MetaData md = _tag_edit->get_metadata(_cur_idx);

	valid = _tag_expression.update_tag(str, md.filepath() );

	set_tag_colors( valid );

}

void GUI_TagEdit::apply_tag(int idx){

	if(!check_idx(idx)) {
		return;
	}

	QMap<Tag, ReplacedString> tag_cap_map = _tag_expression.get_tag_val_map();
	MetaData md = _tag_edit->get_metadata(idx);

	for(const QString& tag : tag_cap_map.keys()){

		ReplacedString cap = tag_cap_map[tag];

		if(tag.compare(TAG_TITLE) == 0){
			md.title = cap;
		}

		else if(tag.compare(TAG_ALBUM) == 0){
			md.album = cap;
		}

		else if(tag.compare( TAG_ARTIST) == 0){
			md.artist = cap;
		}

		else if(tag.compare(TAG_TRACK_NUM) == 0){
			md.track_num = cap.toInt();
		}

		else if(tag.compare(TAG_YEAR) == 0){
			md.year = cap.toInt();
		}

		else if(tag.compare(TAG_DISC) == 0){
			md.discnumber = cap.toInt();
		}
	}

	_tag_edit->update_track(idx, md);

	if(idx == _cur_idx){
		// force gui update
		track_idx_changed();
	}
}

void GUI_TagEdit::apply_tag_clicked(){
	apply_tag(_cur_idx);
}

void GUI_TagEdit::apply_tag_all_clicked(){

	IdxList not_valid;

	QString not_valid_str = tr("Cannot apply tag for") + "<br /><br /> ";
	MetaDataList v_md = _tag_edit->get_all_metadata();
	int n_tracks = v_md.size();

	for(int i=0; i<n_tracks; i++){

		bool valid;

		valid = _tag_expression.update_tag(le_tag->text(), v_md[i].filepath() );

		if(! valid ){
			not_valid << i;
			not_valid_str +=
					QString::number(i+1) + "/" +
					QString::number(n_tracks) + " " +
					v_md[i].title + " " + tr("by") + " " +
					v_md[i].artist + "<br />";
		}
	}

	not_valid_str += "<br />" + tr("Ignore these tracks?");

	GlobalMessage::Answer answer = GlobalMessage::Answer::Yes;
	if(! not_valid.isEmpty() ){
		answer = Message::question_yn(not_valid_str);
	}

	if(answer == GlobalMessage::Answer::Yes){

		for(int i=0; i<n_tracks; i++){
			if(not_valid.contains(i)) continue;

			_tag_expression.update_tag(le_tag->text(), v_md[i].filepath() );
			apply_tag(i);
		}
	}
}


bool GUI_TagEdit::replace_selected_tag_text(Tag t, bool b){

	TextSelection ts = le_tag->get_text_selection();

	if(ts.selection_start < 0 && b) {
		sp_log(Log::Debug) << "Nothing selected...";
		return false;
	}

	QString text = le_tag->text();

	// replace the string by a tag
	if(b){

		ReplacedString selected_text = text.mid( ts.selection_start, ts.selection_size );
		if(!_tag_expression.check_tag(t, selected_text)) return false;

		text.replace( ts.selection_start, ts.selection_size, t );
		le_tag->setText(text);

		_tag_str_map[t] = selected_text;
	}

	// replace tag by the original string
	else{

		text.replace(t, _tag_str_map[t]);
		le_tag->setText(text);

		_tag_str_map.remove(t);
	}


	if(check_idx(_cur_idx)){

		// fetch corresponding filepath and update the tag expression
		MetaData md = _tag_edit->get_metadata(_cur_idx);
		bool valid = _tag_expression.update_tag(text, md.filepath() );

		set_tag_colors( valid );
	}

	return true;
}

void GUI_TagEdit::btn_title_checked(bool b){
	if(!replace_selected_tag_text(TAG_TITLE, b)){
		btn_title->setChecked(false);
	}
}

void GUI_TagEdit::btn_artist_checked(bool b){
	if(!replace_selected_tag_text(TAG_ARTIST, b)){
		btn_artist->setChecked(false);
	}
}

void GUI_TagEdit::btn_album_checked(bool b){
	if(!replace_selected_tag_text(TAG_ALBUM, b)){
		btn_album->setChecked(false);
	}
}
void GUI_TagEdit::btn_track_nr_checked(bool b){
	if(!replace_selected_tag_text(TAG_TRACK_NUM, b)){
		btn_track_nr->setChecked(false);
	}
}
void GUI_TagEdit::btn_disc_nr_checked(bool b){
	if(!replace_selected_tag_text(TAG_DISC, b)){
		btn_disc_nr->setChecked(false);
	}
}
void GUI_TagEdit::btn_year_checked(bool b){
	if(!replace_selected_tag_text(TAG_YEAR, b)){
		btn_year->setChecked(false);
	}
}

void GUI_TagEdit::btn_tag_help_clicked()
{
	QUrl url(QString("http://sayonara-player.com/faq.php#tag-edit"));
	QDesktopServices::openUrl(url);
}

void GUI_TagEdit::show_button_commit(bool b)
{
	btn_ok->setVisible(b);
}

void GUI_TagEdit::show_button_cancel(bool b)
{
	btn_cancel->setVisible(b);
}
