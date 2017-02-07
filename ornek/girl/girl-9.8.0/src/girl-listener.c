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

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libgnome/gnome-exec.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-application-registry.h>

#include "girl.h"
#include "girl-listener.h"

/* extern GirlListenerInfo *girllistener; */

extern GList *girl_channels;
extern GList *girl_listeners;

static void
girl_listener_parser(GirlListenerInfo * listener, xmlDocPtr doc,
		     xmlNodePtr cur)
{
#if 0
	xmlNodePtr sub;
#endif
	g_return_if_fail(listener != NULL);
	g_return_if_fail(doc != NULL);
	g_return_if_fail(cur != NULL);

	listener->name = (gchar *)xmlGetProp(cur, (const xmlChar *)"name");
	GIRL_DEBUG_MSG("listener->name = %s\n", listener->name);
	listener->location = (gchar *)xmlGetProp(cur, (const xmlChar *)"location");
	GIRL_DEBUG_MSG("listener->location = %s\n", listener->location);
	listener->uri = (gchar *)xmlGetProp(cur, (const xmlChar *)"uri");
	GIRL_DEBUG_MSG("listener->uri = %s\n", listener->uri);
	listener->band = (gchar *)xmlGetProp(cur, (const xmlChar *)"band");
	GIRL_DEBUG_MSG("listener->band = %s\n", listener->band);
	listener->description = (gchar *)xmlGetProp(cur, (const xmlChar *)"description");
	GIRL_DEBUG_MSG("listener->description = %s\n", listener->description);

#if 0
	sub = cur->xmlChildrenNode;

	while (sub != NULL) {

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "location"))) {

			listener->locationinfo =
			    g_new0(GirlLocationInfo, 1);

			listener->locationinfo->name =
			    xmlGetProp(sub, "name");
			GIRL_DEBUG_MSG("listener->locationinfo->name = %s\n",
			    listener->locationinfo->name);

			listener->locationinfo->link =
			    xmlGetProp(sub, "link");
			GIRL_DEBUG_MSG("listener->locationinfo->link = %s\n",
			    listener->locationinfo->link);

			listener->locationinfo->glat =
			    xmlGetProp(sub, "glat");
			GIRL_DEBUG_MSG("listener->locationinfo->glat = %s\n",
			    listener->locationinfo->glat);

			listener->locationinfo->glon =
			    xmlGetProp(sub, "glon");
			GIRL_DEBUG_MSG("listener->locationinfo->glon = %s\n",
			    listener->locationinfo->glon);

			listener->locationinfo->grad =
			    xmlGetProp(sub, "grad");
			GIRL_DEBUG_MSG("listener->locationinfo->grad = %s\n",
			    listener->locationinfo->grad);

			listener->locationinfo->vote =
			    xmlGetProp(sub, "vote");
			GIRL_DEBUG_MSG("listener->locationinfo->vote = %s\n",
			    listener->locationinfo->vote);

			listener->locationinfo->rack =
			    xmlGetProp(sub, "rack");
			GIRL_DEBUG_MSG("listener->locationinfo->rack = %s\n",
			    listener->locationinfo->rack);

		}
		/* if... "location" */
		sub = sub->next;
	}
#endif

	return;
}

GirlListenerInfo *girl_listener_load_from_http(GirlListenerInfo * head,
					       gpointer data)
{
	GirlListenerInfo *glistener;

	glistener = girl_listener_load_from_file(head, (gchar *) data);

	return glistener;
}

GirlListenerInfo *girl_listener_load_from_file(GirlListenerInfo * head,
					       char *filename)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	GirlListenerInfo *curr = NULL;
	char *version;

	g_return_val_if_fail(filename != NULL, NULL);

	doc = xmlParseFile(filename);

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

	version = (gchar *)xmlGetProp(cur, (const xmlChar *) "version");

	GIRL_DEBUG_MSG("Valid Girl Listener %s XML document... Parsing listeners...\n", version);

	free(version);

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {

		if ((!xmlStrcmp(cur->name, (const xmlChar *) "listener"))) {

			GIRL_DEBUG_MSG("Found a new listener tag...\n");

			curr = g_new0(GirlListenerInfo, 1);

			girl_listener_parser(curr, doc, cur);

			curr->next = head;

			head = curr;

			girl_listeners = g_list_append(girl_listeners, (GirlListenerInfo *) curr);

			GIRL_DEBUG_MSG("Done with parsing the listener..\n");

		}
		cur = cur->next;
	}

	GIRL_DEBUG_MSG("Finished parsing XML document.\n");

	xmlFreeDoc(doc);

	return curr;
}
