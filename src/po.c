/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "file-dialogs.h"
#include "po.h"
#include "prefs.h"
#include "msg.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gettext-po.h>

#define GTR_PO_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_PO,     \
					 GtranslatorPoPrivate))


G_DEFINE_TYPE(GtranslatorPo, gtranslator_po, G_TYPE_OBJECT)

struct _GtranslatorPoPrivate
{
	/*
	 * Absolute file name
	 */
	gchar *filename;
	
	/*
	 * Gettext's file handle
	 */
	po_file_t gettext_po_file;

	/*
	 * The message domains in this file
	 */
	GList *domains;
	
	/*
	 * Parsed list of GtrMsgs for the current domains' messagelist
	 */
	GList *messages;
	
	/*
	 * A pointer to the currently displayed message 
	 */
	GList *current;
	
	/*
	 * The obsolete messages are stored within this gchar.
	 */
	gchar *obsolete;
	
	/*
	 * Marks if the file was changed; 
	 */
	guint file_changed : 1;
	
	/*
	 * Is the file write-permitted? (read-only)
	 */
	gboolean no_write_perms;

	/*
	 * Translated entries count
	 */
	guint translated;
	
	/*
	 * Fuzzy entries count
	 */
	guint fuzzy;

	/*
	 * Autosave timeout timer
	 */
	guint autosave_timeout;
};

static gboolean parser_errors;

static void
gtranslator_po_init (GtranslatorPo *po)
{
	po->priv = GTR_PO_GET_PRIVATE (po);
}

static void
gtranslator_po_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_po_parent_class)->finalize (object);
}

static void
gtranslator_po_class_init (GtranslatorPoClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorPoPrivate));

	object_class->finalize = gtranslator_po_finalize;
}

/***************************** Public funcs ***********************************/

/**
 * gtranslator_po_new:
 *
 * Return value: a new #GtranslatorPo object
 **/
GtranslatorPo *
gtranslator_po_new(void)
{
	GtranslatorPo *po;
	
	po = g_object_new(GTR_TYPE_PO, NULL);
	
	return po;
}

/**
 * gtranslator_po_parse:
 * @po: a #GtranslatorPo
 * @filename: the filename path to open
 * @error: a variable to store the errors
 *
 * 
 **/
void
gtranslator_po_parse(GtranslatorPo *po,
		     const gchar *filename,
		     GError **error)
{
	GtranslatorPoPrivate *priv = po->priv;
	GtranslatorMsg *msg;
	po_xerror_handler_t gettext_error_handler;
	po_message_t message;
	po_message_iterator_t iter;
	const gchar *msgstr;
	const gchar * const *domains;
	gchar *base;
	gint i = 0;
	
	g_return_if_fail(filename!=NULL);

	base=g_path_get_basename(filename);
	g_return_if_fail(base[0]!='\0');
	g_free(base);
	
	/*
	 * Get absolute filename.
	 */
	if (!g_path_is_absolute(filename)) 
	{
		/*gchar absol[MAXPATHLEN + 1];
		realpath(filename, absol);
		priv->filename = g_strdup(absol);*/
	}
	else
	{
		priv->filename = g_strdup(filename);
	}
	
	po->priv->gettext_po_file = po_file_read(priv->filename,
						 gettext_error_handler);
	if(priv->gettext_po_file == NULL) {
		/*g_set_error(error,
			GTR_PARSER_ERROR,
			GTR_PARSER_ERROR_GETTEXT,
			_("Failed opening file '%s': %s"),
			priv->filename, strerror(errno));*/
		//FIXME
		//gtranslator_po_free(priv);
		return;
	}
	
	/*
	 * If there were errors, abandon this page
	 */
	if(parser_errors) {
		//FIXME
		//gtranslator_po_free(po);
		return;
	}
	
	/*
	 * Determine the message domains to track
	 */
	if(!(domains = po_file_domains(priv->gettext_po_file))) {
		/*g_set_error(error,
			GTR_PARSER_ERROR,
			GTR_PARSER_ERROR_GETTEXT,
			_("Gettext returned a null message domain list."));*/
		
		//FIXME
		//gtranslator_po_free(po);
		return;
	}
	while(domains[i]) {
		priv->domains = g_list_append(priv->domains, g_strdup(domains[i]));
		i++;
	}

	/*
	 * Determine whether first message is the header or not, and
	 * if so, process it seperately. Otherwise, treat as a normal
	 * message.
	 */
	priv->messages = NULL;
	iter = po_message_iterator(priv->gettext_po_file, NULL);
	message = po_next_message(iter);
	msgstr = po_message_msgstr(message);
	if(!strncmp(msgstr, "Project-Id-Version: ", 20)) {
		/* TODO: parse into our header structure */
	}
	else {
		/* Reset our pointer */
		iter = po_message_iterator(priv->gettext_po_file, NULL);
	}
		
	/*
	 * Post-process these into a linked list of GtrMsgs.
	 */
	while((message = po_next_message(iter)))
	{
		/* Unpack into a GtrMsg */
		msg = gtranslator_msg_new();
		gtranslator_msg_set_message(msg, message);
  
		/* Build up messages */
		priv->messages = g_list_append(priv->messages, msg);
	}
	if(priv->messages == NULL) {
		/*g_set_error(error,
			GTR_PARSER_ERROR,
			GTR_PARSER_ERROR_OTHER,
			_("No messages obtained from parser."));*/
		//FIXME: g_object_unref??
		//gtranslator_po_free(po);
		return;
	}
	po_message_iterator_free(iter);

	/*
	 * Set the current message to the first message.
	 */
	priv->current = g_list_first(priv->messages);
	
}

/**
 * gtranslator_po_get_filename:
 * @po: a #GtranslatorPo
 * 
 * Return value: the file name string
 **/
gchar *
gtranslator_po_get_filename(GtranslatorPo *po)
{
	return po->priv->filename;
}

/**
 * gtranslator_po_set_filename:
 * @po: a #GtranslatorPo
 * @data: The file name text you want to set
 *
 * Sets the text path within the #GtranslatorPo object. It overwrites any text
 * that was there before.
 **/
void
gtranslator_po_set_filename(GtranslatorPo *po,
			    gchar *data)
{
	g_return_if_fail(GTR_IS_PO(po));
	
	if(po->priv->filename)
		g_free(po->priv->filename);
	po->priv->filename = g_strdup(data);
}

gboolean
gtranslator_po_get_write_perms(GtranslatorPo *po)
{
	return po->priv->no_write_perms;
}

/**
 * gtranslator_po_get_messages:
 * @po: a #GtranslatorPo
 *
 * Return value: a pointer to the messages list
 **/
GList *
gtranslator_po_get_messages(GtranslatorPo *po)
{
	return po->priv->messages;
}

/**
 * gtranslator_po_get_domains:
 * @po: a #GtranslatorPo
 *
 * Return value: a pointer to the domains list
 **/
GList *
gtranslator_po_get_domains(GtranslatorPo *po)
{
	return po->priv->domains;
}

/**
 * gtranslator_po_get_po_file:
 * @po: a #GtranslatorPo
 * 
 * Return value: the gettext file
 **/
po_file_t
gtranslator_po_get_po_file(GtranslatorPo *po)
{
	return po->priv->gettext_po_file;
}

