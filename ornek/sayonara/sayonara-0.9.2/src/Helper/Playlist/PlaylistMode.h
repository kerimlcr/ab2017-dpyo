/* PlaylistMode.h */

/* Copyright (C) 2011  Lucio Carreras
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

#ifndef PLAYLISTMODE_H_
#define PLAYLISTMODE_H_

#include <iostream>
#include <QList>
#include <QVariant>
#include <QStringList>

using namespace std;

// this class has to be inline because of the settings registry
class PlaylistMode {

public:
	enum PlaylistModeState
	{
		Off  = 0,
		On   = 1,
		Disabled = 2 // this has to be because of consistence
	};

private:
	PlaylistModeState	_rep1;
	PlaylistModeState	_repAll;
	PlaylistModeState	_append;
	PlaylistModeState	_shuffle;
	PlaylistModeState	_dynamic;
	PlaylistModeState	_gapless;

	PlaylistModeState set_state(bool active, bool enabled){
		quint8 ret = PlaylistMode::Off;
		if(active){
			ret |= PlaylistMode::On;
		}

		if(!enabled){
			ret |= PlaylistMode::Disabled;
		}

		return (PlaylistModeState) ret;
	}

public:
	PlaylistModeState rep1() const { return _rep1; }
	PlaylistModeState repAll() const { return _repAll; }
	PlaylistModeState append() const { return _append; }
	PlaylistModeState shuffle() const { return _shuffle; }
	PlaylistModeState dynamic() const { return _dynamic; }
	PlaylistModeState gapless() const { return _gapless; }

	void setRep1(PlaylistModeState state){ _rep1 = state; }
	void setRepAll(PlaylistModeState state){ _repAll = state; }
	void setAppend(PlaylistModeState state){ _append = state; }
	void setShuffle(PlaylistModeState state){ _shuffle = state; }
	void setDynamic(PlaylistModeState state){ _dynamic = state; }
	void setGapless(PlaylistModeState state){ _gapless = state; }

	void setRep1(bool on, bool enabled=true){ _rep1 = set_state(on, enabled); }
	void setRepAll(bool on, bool enabled=true){ _repAll = set_state(on, enabled); }
	void setAppend(bool on, bool enabled=true){ _append = set_state(on, enabled); }
	void setShuffle(bool on, bool enabled=true){ _shuffle = set_state(on, enabled); }
	void setDynamic(bool on, bool enabled=true){ _dynamic = set_state(on, enabled); }
	void setGapless(bool on, bool enabled=true){ _gapless = set_state(on, enabled); }

	static bool isActive(PlaylistModeState pl)
	{
		quint8 ipl = (quint8) pl;
		return ( ipl & PlaylistMode::On );
	}

	static bool isEnabled(PlaylistModeState pl){
		quint8 ipl = (quint8) pl;
		return (( ipl & PlaylistMode::Disabled ) == 0);
	}

	static bool isActiveAndEnabled(PlaylistModeState pl)
	{
		return (isEnabled(pl) && isActive(pl));
	}

	PlaylistMode(){
		_rep1 = PlaylistMode::Off;
		_repAll = PlaylistMode::On;
		_append = PlaylistMode::Off;

		_shuffle = PlaylistMode::Off;
		_gapless = PlaylistMode::Off;
		_dynamic = PlaylistMode::Off;
	}

	void print(){
		cout << "rep1 = "   << (int) _rep1 << ", "
			<< "repAll = "  << (int) _repAll << ", "
			<< "append = "  << (int) _append <<", "
			<< "dynamic = " << (int) _dynamic << ","
			<< "gapless = " << (int) _gapless << endl;
			
	}

	QString toString() const {
		QString str;
		str += QString::number((int) _append)  + QString(",");
		str += QString::number((int) _repAll)  + QString(",");
		str += QString::number((int) _rep1)    + QString(",");
		str += "0,";
		str += QString::number((int) _shuffle) + QString(",");
		str += QString::number((int) _dynamic) + QString(",");
		str += QString::number((int) _gapless);

		return str;
	}

    static PlaylistMode fromString(QString str){

        PlaylistMode plm;
		QStringList list = str.split(',');

        if(list.size() < 6) return plm;

        plm.setAppend((PlaylistModeState) list[0].toInt());
        plm.setRepAll((PlaylistModeState) list[1].toInt());
        plm.setRep1((PlaylistModeState) list[2].toInt());
        plm.setShuffle((PlaylistModeState) list[4].toInt());
        plm.setDynamic((PlaylistModeState) list[5].toInt());

		if(list.size() > 6){
			plm.setGapless((PlaylistModeState) list[6].toInt());
		}

        return plm;
	}

	bool operator==(const PlaylistMode& pm) const {

		if(pm.append() != _append) return false;
		if(pm.repAll() != _repAll) return false;
		if(pm.rep1() != _rep1) return false;
		if(pm.shuffle() != _shuffle) return false;
		if(pm.dynamic() != _dynamic) return false;
		if(pm.gapless() != _gapless) return false;

		return true;
	}
};

#endif
