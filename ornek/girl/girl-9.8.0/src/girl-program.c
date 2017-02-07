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
#include "girl-program.h"

extern GirlData *girl;
extern GList *girl_channels;
extern GList *girl_programs;
extern GList *girl_listeners;

extern GtkWidget *girl_app;

static void
girl_program_parser(GirlProgramInfo * program, xmlDocPtr doc,
		    xmlNodePtr cur)
{
	xmlNodePtr sub;
	char *chans;

	g_return_if_fail(program != NULL);
	g_return_if_fail(doc != NULL);
	g_return_if_fail(cur != NULL);

	program->id = (gchar *)xmlGetProp(cur, (const xmlChar *)"id");
	GIRL_DEBUG_MSG("program->id = %s\n", program->id);
	program->name = (gchar *)xmlGetProp(cur, (const xmlChar *)"name");
	GIRL_DEBUG_MSG("program->name = %s\n", program->name);
	program->rank = (gchar *)xmlGetProp(cur, (const xmlChar *)"rank");
	GIRL_DEBUG_MSG("program->rank = %s\n", program->rank);
	program->type = (gchar *)xmlGetProp(cur, (const xmlChar *)"type");
	GIRL_DEBUG_MSG("program->type = %s\n", program->type);
	program->band = (gchar *)xmlGetProp(cur, (const xmlChar *)"band");
	GIRL_DEBUG_MSG("program->band = %s\n", program->band);

	sub = cur->xmlChildrenNode;

	while (sub != NULL) {

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "frequency"))) {
			program->frequency = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("program->frequency = %s\n",
			    program->frequency);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "location"))) {
			program->location = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("program->location = %s\n", program->location);
			/* fprintf(stdout, "%s (%s), ", program->name, program->location); */
		}

		if ((!xmlStrcmp
		     (sub->name, (const xmlChar *) "description"))) {
			program->description = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("program->description = %s\n", program->description);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "uri"))) {
			program->uri = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("program->uri = %s\n", program->uri);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "archive"))) {

			GirlArchiveInfo *archive = g_new0(GirlArchiveInfo, 1);
			program->archive = archive;

			program->archive->mimetype = (gchar *)xmlGetProp(sub, (const xmlChar *)"mime");
			GIRL_DEBUG_MSG("program->archive->mimetype = %s\n",
			    program->archive->mimetype);
			if (xmlGetProp(sub, (const xmlChar *)"bitrate") != NULL) {
				program->archive->bitrate = (glong)
					atol((const char *)xmlGetProp(sub, (const xmlChar *)"bitrate"));
				GIRL_DEBUG_MSG("program->archive->bitrate = %li\n",
				    program->archive->bitrate);
			}

			if (xmlGetProp(sub, (const xmlChar *)"samplerate") != NULL) {
				program->archive->samplerate = (glong) 
					atol((const char *)xmlGetProp(sub, (const xmlChar *)"samplerate"));
			}

			GIRL_DEBUG_MSG("program->archive->samplerate = %li\n",
			    program->archive->samplerate);
			program->archive->uri = (gchar *)xmlGetProp(sub, (const xmlChar *)"uri");
			GIRL_DEBUG_MSG("program->archive->uri = %s\n",
			    program->archive->uri);

			chans = (gchar *)xmlGetProp(sub, (const xmlChar *)"channels");

			if (chans != NULL) {
				if (strcmp(chans, "stereo") == 0) {
					program->archive->channels =
					    GIRL_CHANNELS_STEREO;
					GIRL_DEBUG_MSG("program->archive->channels = %d\n", program->archive->channels);
				} else if (strcmp(chans, "mono") == 0) {
					program->archive->channels =
					    GIRL_CHANNELS_MONO;
					GIRL_DEBUG_MSG("program->archive->channels = %d\n", program->archive->channels);
				} else if (strcmp(chans, "5:1") == 0) {
					program->archive->channels =
					    GIRL_CHANNELS_5_1;
					GIRL_DEBUG_MSG("program->archive->channels = %d\n", program->archive->channels);
				}
				g_free(chans);
			}

		}
		/* if... "archive" */
		sub = sub->next;
	}

	return;
}

GirlProgramInfo *girl_program_load_from_http(GirlProgramInfo * head,
					     gpointer data)
{
	GirlProgramInfo *gprogram;
	gprogram = girl_program_load_from_file (head, "http://girl.src.oka.no/programs.xml");
	return gprogram;
}

GirlProgramInfo *girl_program_load_from_file(GirlProgramInfo * head,
					     char *filename)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	GirlProgramInfo *curr = NULL;
	char *version;
	GirlProgramInfo *mem_program;

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

	GIRL_DEBUG_MSG("Valid Girl %s XML document... Parsing programs...\n",
	    version);

	free(version);

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {

		if ((!xmlStrcmp(cur->name, (const xmlChar *) "program"))) {

			GIRL_DEBUG_MSG("Found a new program.\n");

			curr = g_new0(GirlProgramInfo, 1);
			mem_program = g_new0(GirlProgramInfo, 1);

			girl_program_parser(curr, doc, cur);

			curr->next = head;

			head = curr;

			mem_program = head;

			girl_programs = g_list_append(girl_programs, (GirlProgramInfo *)mem_program);

			GIRL_DEBUG_MSG("Done with parsing the program.\n");

		}
		cur = cur->next;
	}

	GIRL_DEBUG_MSG("Finished parsing XML document.\n");

	xmlFreeDoc(doc);

	return curr;
}
