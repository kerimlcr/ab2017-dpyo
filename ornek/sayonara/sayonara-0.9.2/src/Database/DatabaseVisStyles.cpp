/* DatabaseVisStyles.cpp */

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


#include "Database/SayonaraQuery.h"
#include "Database/DatabaseVisStyles.h"
#include "GUI/Plugins/Engine/StyleTypes.h"

#include <QColor>

DatabaseVisStyles::DatabaseVisStyles(QSqlDatabase db, quint8 db_id) :
	DatabaseModule(db, db_id)
{
}


QString col2String(QColor col) {
	QString str;
	str = QString::number(col.red()) + "," +
	QString::number(col.green()) + "," +
	QString::number(col.blue()) + "," +
	QString::number(col.alpha());
	return str;

}


bool colFromString(QString str, QColor& c) {
	QStringList colors = str.split(",");

	if(colors.size() < 3) {
		return false;
	}

	c.setRed(colors[0].toInt());
	c.setGreen(colors[1].toInt());
	c.setBlue(colors[2].toInt());

	if(colors.size() == 4){
		c.setAlpha(colors[3].toInt());
	}

	else{
		c.setAlpha(255);
	}

	return true;

}

QList<RawColorStyle> DatabaseVisStyles::get_raw_color_styles() {

	DB_TRY_OPEN(_db);

	QList<RawColorStyle> ret_val;

	SayonaraQuery q (_db);
	q.prepare("SELECT * FROM VisualStyles;" );

	if(!q.exec()) {
		q.show_error("Could not fetch color styles");
		return ret_val;
	}

	while(q.next()) {
		RawColorStyle rcs;

		rcs.col_list.name = q.value(0).toString();
		QColor col1, col2, col3, col4;
		bool col3v, col4v;
		colFromString(q.value(1).toString(), col1);
		colFromString(q.value(2).toString(), col2);
		col3v = colFromString(q.value(3).toString(), col3);
		col4v = colFromString(q.value(4).toString(), col4);


		rcs.col_list.colors << col1;
		rcs.col_list.colors << col2;
		if(col3v) rcs.col_list.colors << col3;
		if(col4v) rcs.col_list.colors << col4;
		rcs.n_bins_spectrum = q.value(5).toInt();
		rcs.rect_height_spectrum = q.value(6).toInt();
		rcs.n_fading_steps_spectrum = q.value(7).toInt();
		rcs.hor_spacing_spectrum = q.value(8).toInt();
		rcs.ver_spacing_spectrum = q.value(9).toInt();
		rcs.rect_width_level = q.value(10).toInt();
		rcs.rect_height_level = q.value(11).toInt();
		rcs.hor_spacing_level = q.value(12).toInt();
		rcs.ver_spacing_level = q.value(13).toInt();
		rcs.n_fading_steps_level = q.value(14).toInt();

		ret_val << rcs;
	}

	return ret_val;
}


bool DatabaseVisStyles::insert_raw_color_style_to_db(const RawColorStyle& rcs) {

	if(raw_color_style_exists(rcs.col_list.name))
		return update_raw_color_style(rcs);

	DB_RETURN_NOT_OPEN_BOOL(_db);

	QString col_str;
	for(int i=0; i<4; i++) {
		col_str += ":col" + QString::number(i + 1) + ", ";
	}

	SayonaraQuery q (_db);
	QString sql_str;
	sql_str = "INSERT INTO VisualStyles VALUES ("
			":name, "
			+ col_str +
			":n_bins_sp, "
			":rect_height_sp, "
			":fading_steps_sp, "
			":h_spacing_sp, "
			":v_spacing_sp, "
			":rect_width_lv, "
			":rect_height_lv, "
			":h_spacing_lv, "
			":v_spacing_lv, "
			":fading_steps_lv"
			")";

	q.prepare(sql_str);
	q.bindValue(":name", rcs.col_list.name);
	q.bindValue(":col1", col2String(rcs.col_list.colors[0]));
	q.bindValue(":col2", col2String(rcs.col_list.colors[1]));

	if(rcs.col_list.colors.size() > 2) q.bindValue(":col3", col2String(rcs.col_list.colors[2]));
	else q.bindValue(":col3", "");

	if(rcs.col_list.colors.size() > 3) q.bindValue(":col4", col2String(rcs.col_list.colors[3]));
	else q.bindValue(":col4", "");

	q.bindValue(":n_bins_sp", rcs.n_bins_spectrum);
	q.bindValue(":rect_height_sp", rcs.rect_height_spectrum);
	q.bindValue(":fading_steps_sp", rcs.n_fading_steps_spectrum);
	q.bindValue(":h_spacing_sp", rcs.hor_spacing_spectrum);
	q.bindValue(":v_spacing_sp", rcs.ver_spacing_spectrum);
	q.bindValue(":rect_width_lv", rcs.rect_width_level);
	q.bindValue(":rect_height_lv", rcs.rect_height_level);
	q.bindValue(":h_spacing_lv", rcs.hor_spacing_level);
	q.bindValue(":v_spacing_lv", rcs.ver_spacing_level);
	q.bindValue(":fading_steps_lv", rcs.n_fading_steps_level);


	if(!q.exec()) {
		q.show_error("Could not insert style");
		return false;
	}

	return true;
}



bool DatabaseVisStyles::update_raw_color_style(const RawColorStyle& rcs) {

	if(!raw_color_style_exists(rcs.col_list.name))
		return insert_raw_color_style_to_db(rcs);

	DB_RETURN_NOT_OPEN_BOOL(_db);

	QString col_str;
	for(int i=0; i < 4; i++) {
		col_str += "col" + QString::number(i + 1) + "=:col" + QString::number(i + 1) + ", ";
	}

	SayonaraQuery q (_db);
	QString sql_str;
	sql_str = "UPDATE VisualStyles SET "

			+ col_str +
			"nBinsSpectrum=:n_bins_sp, "
			"rectHeightSpectrum=:rect_height_sp, "
			"fadingStepsSpectrum=:fading_steps_sp, "
			"horSpacingSpectrum=:h_spacing_sp, "
			"vertSpacingSpectrum=:v_spacing_sp, "
			"rectWidthLevel=:rect_width_lv, "
			"rectHeightLevel=:rect_height_lv, "
			"horSpacingLevel=:h_spacing_lv, "
			"verSpacingLevel=:v_spacing_lv, "
			"fadingStepsLevel=:fading_steps_lv"
			" WHERE name=:name";

	q.prepare(sql_str);
	q.bindValue(":name", rcs.col_list.name);
	q.bindValue(":col1", col2String(rcs.col_list.colors[0]));
	q.bindValue(":col2", col2String(rcs.col_list.colors[1]));

	if(rcs.col_list.colors.size() > 2) q.bindValue(":col3", col2String(rcs.col_list.colors[2]));
	else q.bindValue(":col3", "");

	if(rcs.col_list.colors.size() > 3) q.bindValue(":col4", col2String(rcs.col_list.colors[3]));
	else q.bindValue(":col4", "");

	q.bindValue(":n_bins_sp", rcs.n_bins_spectrum);
	q.bindValue(":rect_height_sp", rcs.rect_height_spectrum);
	q.bindValue(":fading_steps_sp", rcs.n_fading_steps_spectrum);
	q.bindValue(":h_spacing_sp", rcs.hor_spacing_spectrum);
	q.bindValue(":v_spacing_sp", rcs.ver_spacing_spectrum);
	q.bindValue(":rect_width_lv", rcs.rect_width_level);
	q.bindValue(":rect_height_lv", rcs.rect_height_level);
	q.bindValue(":h_spacing_lv", rcs.hor_spacing_level);
	q.bindValue(":v_spacing_lv", rcs.ver_spacing_level);
	q.bindValue(":fading_steps_lv", rcs.n_fading_steps_level);


	if(!q.exec()) {
		q.show_error(QString("Could not update style ") + rcs.col_list.name);
		return false;
	}

	return true;
}



bool DatabaseVisStyles::delete_raw_color_style(QString name) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	q.prepare("DELETE FROM visualstyles WHERE name=:name;");
	q.bindValue(":name", name);

	if(!q.exec()) {
		q.show_error(QString("Could not delete Raw color style ") + name);
		return false;
	}

	return true;
}


bool DatabaseVisStyles::raw_color_style_exists(QString name) {

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q (_db);
	q.prepare("SELECT * FROM visualstyles WHERE name=:name;");
	q.bindValue(":name", name);

	if(!q.exec()) {
		q.show_error("Cannot check if raw color style exists");
		return false;
	}

	if(!q.next()){
		return false;
	}

	return true;
}

