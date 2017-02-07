/* $Id$
 *
 * GNOME Internet Radio Locator
 *
 * Copyright (C) 2014  Ole Aamot Software
 *
 * Author: Ole Aamot <oka@oka.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libgnome/gnome-exec.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-application-registry.h>

#include "girl.h"
#include "girl-streams.h"

extern GirlData *girl;
extern GList *girl_channels;
extern GList *girl_streams;
extern GList *girl_listeners;
extern GList *girl_streams;

extern GtkWidget *girl_app;

#if 0
GirlStreamsInfo *girl_streams_new (GirlStreamsInfo * head,
				   gchar *mime,
				   gchar *uri,
				   gchar *codec,
				   gchar *samplerate,
				   gchar *channels,
	                           gchar *bitrate) {
}
#endif

static void
girl_streams_parser(GirlStreamsInfo *streams, xmlDocPtr doc,
		    xmlNodePtr cur)
{
	/* xmlNodePtr sub; */
	/* char *chans; */

	g_return_if_fail(streams != NULL);
	g_return_if_fail(doc != NULL);
	g_return_if_fail(cur != NULL);

	streams->mime = (gchar *)xmlGetProp(cur, (const xmlChar *)"mime");
	GIRL_DEBUG_MSG("streams->mime = %s\n", streams->mime);
	streams->uri = (gchar *)xmlGetProp(cur, (const xmlChar *)"uri");
	GIRL_DEBUG_MSG("streams->uri = %s\n", streams->uri);
	streams->samplerate = (gchar *)xmlGetProp(cur, (const xmlChar *)"samplerate");
	GIRL_DEBUG_MSG("streams->samplerate = %s\n", streams->samplerate);
	streams->codec = (gchar *)xmlGetProp(cur, (const xmlChar *)"codec");
	GIRL_DEBUG_MSG("streams->codec = %s\n", streams->codec);
	streams->bitrate = (gchar *)xmlGetProp(cur, (const xmlChar *)"bitrate");
	GIRL_DEBUG_MSG("streams->bitrate = %s\n", streams->bitrate);
	streams->channels = (GirlChannels)xmlGetProp(cur, (const xmlChar *)"channels");


#if 0
	GIRL_DEBUG_MSG("streams->channels = %\n", streams->channels);
	
	sub = cur->xmlChildrenNode;

	while (sub != NULL) {

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "frequency"))) {
			streams->frequency =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("streams->frequency = %s\n",
			    streams->frequency);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "location"))) {
			streams->location =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("streams->location = %s\n", streams->location);
			/* fprintf(stdout, "%s (%s), ", streams->name, streams->location); */
		}

		if ((!xmlStrcmp
		     (sub->name, (const xmlChar *) "description"))) {
			streams->description =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("streams->description = %s\n", streams->description);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "uri"))) {
			streams->uri =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("streams->uri = %s\n", streams->uri);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "encoder"))) {

			GirlEncoderInfo *encoder = g_new0(GirlEncoderInfo, 1);
			streams->encoder = encoder;

			streams->encoder->mimetype =
			    xmlGetProp(sub, "mime");
			GIRL_DEBUG_MSG("streams->encoder->mimetype = %s\n",
			    streams->encoder->mimetype);
			if (xmlGetProp(sub, "bitrate") != NULL) {
				streams->encoder->bitrate =
				    atol(xmlGetProp(sub, "bitrate"));
				GIRL_DEBUG_MSG("streams->encoder->bitrate = %li\n",
				    streams->encoder->bitrate);
			}

			if (xmlGetProp(sub, "samplerate") != NULL) {
				streams->encoder->samplerate =
				    atol(xmlGetProp(sub, "samplerate"));
			}

			GIRL_DEBUG_MSG("streams->encoder->samplerate = %li\n",
			    streams->encoder->samplerate);
			streams->encoder->uri = xmlGetProp(sub, "uri");
			GIRL_DEBUG_MSG("streams->encoder->uri = %s\n",
			    streams->encoder->uri);

			chans = xmlGetProp(sub, "channels");

			if (chans != NULL) {
				if (strcmp(chans, "stereo") == 0) {
					streams->encoder->channels =
					    GIRL_CHANNELS_STEREO;
					GIRL_DEBUG_MSG("streams->encoder->channels = %d\n", streams->encoder->channels);
				} else if (strcmp(chans, "mono") == 0) {
					streams->encoder->channels =
					    GIRL_CHANNELS_MONO;
					GIRL_DEBUG_MSG("streams->encoder->channels = %d\n", streams->encoder->channels);
				} else if (strcmp(chans, "5:1") == 0) {
					streams->encoder->channels =
					    GIRL_CHANNELS_5_1;
					GIRL_DEBUG_MSG("streams->encoder->channels = %d\n", streams->encoder->channels);
				}
				g_free(chans);
			}

		}
		/* if... "encoder" */
		sub = sub->next;
	}
#endif

	return;
}

GirlStreamsInfo *girl_streams_load_from_http(GirlStreamsInfo * head,
					     gpointer data)
{
	GirlStreamsInfo *gstreams;
	gstreams = girl_streams_load_from_file (head, "http://girl.src.oka.no/streams.xml");
	return gstreams;
}

GirlStreamsInfo *girl_streams_load_from_file(GirlStreamsInfo * head,
					     char *filename)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	GirlStreamsInfo *curr = NULL;
	char *version;
	GirlStreamsInfo *mem_streams;

	g_return_val_if_fail(filename != NULL, NULL);

	doc = xmlReadFile(filename, NULL, 0);

	if (doc == NULL) {
		perror("xmlParseFile");
		xmlFreeDoc(doc);
		return NULL;
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		fprintf(stderr, "Empty document\n");
		xmlFreeDoc(doc);
		return NULL;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "girl")) {
		fprintf(stderr,
			"Document of wrong type, root node != girl\n");
		xmlFreeDoc(doc);
		return NULL;
	}

	version = (gchar *)xmlGetProp(cur, (const xmlChar *)"version");

	GIRL_DEBUG_MSG("Valid Girl %s XML document... Parsing streams...\n",
	    version);

	free(version);

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {

		if ((!xmlStrcmp(cur->name, (const xmlChar *) "stream"))) {

			GIRL_DEBUG_MSG("Found a new stream.\n");

			curr = g_new0(GirlStreamsInfo, 1);
			mem_streams = g_new0(GirlStreamsInfo, 1);

			girl_streams_parser(curr, doc, cur);

			curr->next = head;

			head = curr;

			mem_streams = head;

			girl_streams = g_list_append(girl_streams, (GirlStreamsInfo *)mem_streams);

			GIRL_DEBUG_MSG("Done with parsing the streams.\n");

		}
		cur = cur->next;
	}

	GIRL_DEBUG_MSG("Finished parsing XML document.\n");

	xmlFreeDoc(doc);

	return curr;
}
