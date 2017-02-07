/* CoverFetchThread.h */

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
 * CoverFetchThread.h
 *
 *  Created on: Jun 28, 2011
 *      Author: Lucio Carreras
 */

#ifndef COVERFETCHTHREAD_H_
#define COVERFETCHTHREAD_H_

#include <QImage>
#include <QStringList>
#include "CoverLocation.h"

class AsyncWebAccess;
/**
 * @brief The CoverFetchThread class, This is not a real QThread class, but behaves like one because of AsyncWebAccess
 * @ingroup Covers
 */
class CoverFetchThread : public QObject {

	Q_OBJECT


signals:
	/**
	 * @brief emitted, when thread has finished
	 * @param b true, if couvers could be fetched. false else
	 */
	void sig_finished(bool b);

	/**
	 * @brief emitted, when covers has been found
	 * @param cl CoverLocation including the local cover path
	 */
	void sig_cover_found(const QString& cover_path);


public:
	CoverFetchThread(QObject* parent, const CoverLocation& cl, const int n_covers);
    virtual ~CoverFetchThread();

	/**
	 * @brief start fetching covers, if the url does not contain "google",
	 *   a direct link to an image is assumed and will be downloaded directly
	 * @return always true
	 */
	bool start();

	/**
	 * @brief fetch next cover
	 * @return false, if there are no more covers to fetch
	 */
	bool more();


private:

	AsyncWebAccess*		_single_img_fetcher=nullptr;
	AsyncWebAccess*		_multi_img_fetcher=nullptr;
	AsyncWebAccess*		_content_fetcher=nullptr;

	QString				_url;
	QString				_target_file;
	QStringList			_addresses;

	/**
	 * @brief _n_covers number of covers to fetch
	 */
	int					_n_covers;

	/**
	 * @brief _covers_found number of covers found
	 */
	int					_covers_found;


	/**
	 * @brief calc_addresses extracs png addresses from the website
	 * @param num number of covers to fetch
	 * @param website website content
	 * @param regex the regular expression used to extract the covers
	 * @return a list of png addresses
	 */
	QStringList calc_addresses(int num,
							  const QByteArray& website,
							  const QString& regex) const;

	/**
	 * @brief calc_addresses_from_google extracs png addresses from the Google website
	 * @param num number of covers to fetch
	 * @param website website content
	 * @return a list of png addresses
	 */
	QStringList calc_addresses_from_google(int num,
										  const QByteArray& website) const;

	/**
	 * @brief save_and_emit_image saves the image to disc, creates CoverLocation object and emits the sig_cover_found signal
	 * @param filepath where to save the image on disc
	 * @param img the cover image
	 */
	void save_and_emit_image(const QString& filepath,
							 const QImage& img);


private slots:
	/**
	 * @brief A single image has been fetched (reached when _n_covers was set to 1),
	 *   calls save_and_emit_image
	 * @param success indicates if image could be fetched successfully
	 */
	void single_image_fetched(bool success);

	/**
	 * @brief multi_image_fetched (reached when _n_covers was set to > 1),
	 *   calls save_and_emit_image for first image;
	 * @param success indicates if images could be fetched successfully
	 */
	void multi_image_fetched(bool success);

	/**
	 * @brief The website content has been fetched
	 * @param success indicates if content could be fetched
	 */
	void content_fetched(bool success);

};

#endif /* COVERFETCHTHREAD_H_ */
