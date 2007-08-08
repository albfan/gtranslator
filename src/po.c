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


#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gettext-po.h>

#include "po.h"
#include "prefs.h"
#include "file-dialogs.c"
#include "message.h"
#include "window.h"

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
	po->priv = GTR_PO_GET_PRIVATE(po);
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

/*
 * The core parsing function for the given po file.
 */ 
GtranslatorPo *
gtranslator_po_new(const gchar *filename,
		   GError **error)
{
	GtranslatorPo *po;
	GtranslatorPoPrivate *priv;
	GtrMsg *msg;
	gchar *base;
	int i = 0;
	po_message_iterator_t iter;
	po_message_t message;
	const gchar * const *domains;
	const gchar *msgstr;
	po_xerror_handler_t gettext_error_handler;
	
	g_return_val_if_fail(filename!=NULL, NULL);

	base=g_path_get_basename(filename);
	g_return_val_if_fail(base[0]!='\0', NULL);
	g_free(base);

	/*
	 * Start a new PO file record
	 */
	po = g_object_new(GTR_TYPE_PO, NULL);
	priv = po->priv;

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
		
	/*
	 * Open the PO file, using gettext's utility function
	 */
	gchar *errno;
	parser_errors = FALSE;
	priv->gettext_po_file = po_file_read(priv->filename, gettext_error_handler);
	if(priv->gettext_po_file == NULL) {
		/*g_set_error(error,
			GTR_PARSER_ERROR,
			GTR_PARSER_ERROR_GETTEXT,
			_("Failed opening file '%s': %s"),
			priv->filename, strerror(errno));*/
		//FIXME
		//gtranslator_po_free(priv);
		return NULL;
	}
	
	
	/*
	 * If there were errors, abandon this page
	 */
	if(parser_errors) {
		//FIXME
		//gtranslator_po_free(po);
		return NULL;
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
		return NULL;
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
		msg = g_new0(GtrMsg, 1);
		msg->message = message;
  
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
		return NULL;
	}
	po_message_iterator_free(iter);

	/*
	 * Set the current message to the first message.
	 */
	priv->current = g_list_first(priv->messages);

	return po;
}

gchar *
gtranslator_po_get_filename(GtranslatorPo *po)
{
	return po->priv->filename;
}

void
gtranslator_po_set_filename(GtranslatorPo *po,
			    gchar *data)
{
	po->priv->filename = g_strdup(data);
}

gboolean
gtranslator_po_get_write_perms(GtranslatorPo *po)
{
	return po->priv->no_write_perms;
}


/*
 * The main file opening function. Checks that the file isn't already open,
 * and if not, opens it in a new tab.
 */
gboolean 
gtranslator_open(const gchar *filename,
		 GtranslatorWindow *window,
		 GError **error)
{
	gchar	*base;
	gchar *title;
	GList	*pagelist;
	//GtrPage	*page;
	GtranslatorPo	*po;
	GtkWidget *page_label;
	
	/*
	 * If the filename can't be opened, pass the error back to the caller
	 * to handle.
	 */
	if(!(po = gtranslator_po_new(filename, error)))
		return FALSE;

	/*
	 * If not a crash/temporary file, add to the history.
	 */
	/*base = g_path_get_basename(po->filename);
	if(nautilus_strcasecmp(base, gtranslator_runtime_config->temp_filename) || 
	   nautilus_strcasecmp(base, gtranslator_runtime_config->crash_filename))
	{
		const char *header = po_file_domain_header(po->gettext_po_file, NULL);
		if(header) {
			const char *project_id = po_header_field(header, "Project-Id-Version");
			if(project_id) {
				gtranslator_history_add(po->filename, (gchar *)project_id);
			}
			else {
				gtranslator_history_add(po->filename, _("N/A"));
			}
		}
		else {
			gtranslator_history_add(po->filename, _("N/A"));
		}
	}
	g_free(base);*/

	/*
	 * Create a page to add to our list of open files
	 */
	/*if(current_page != NULL)
		gtranslator_file_close(NULL, NULL);
	gtranslator_page_new(po);*/
	gtranslator_window_new_tab(window, po);
	
	/*
	 * Set window title
	 */
	/*title=g_strdup_printf(_("gtranslator -- %s"), po->filename);
	gtk_window_set_title(GTK_WINDOW(window), title);
	g_free(title);*/

	
	/*
	 * Show the current message.
	 */
	//gtranslator_message_show(po->current->data);
	
	/*
	 * Select the current row
	 */
	/*if(current_page->messages_table)
	{
		gtranslator_messages_table_select_row(current_page->messages_table, GTR_MSG(po->current->data));
	}*/

	/*
	 * Enable/disable application bar options
	 */
	//gtranslator_application_bar_update(0);

	/*
	 * Iterate to the main GUI thread -- well, no locks for the GUI should
	 *  be visible -- avoids "the clean my gtranslator window with dialog"
	 *   party game ,-)
	 */
	while(gtk_events_pending())
	{
		gtk_main_iteration();
	}

	/*
	 * Update the recent files list.
	 */
//	gtranslator_history_show();

	/*gtranslator_actions_set_up_file_opened();

	gtranslator_update_translated_count(po);
	gtranslator_update_progress_bar();*/
		
	/*
	 * Is there any fuzzy message ?
	 */
	if(po->priv->fuzzy>0)
	{
		/*
		 * Then enable the Fuzzy buttons/entries in the menus
		 */
		//gtk_widget_set_sensitive(gtranslator_menuitems->next_fuzzy, TRUE);

		/*
		 * If there is the corresponding pref and a fuzzy message, then
		 *  we'd enable the corresponding menu entries for "remove all
		 *   translations.
		 */
		if(GtrPreferences.rambo_function)
		{
		//	gtk_widget_set_sensitive(gtranslator_menuitems->remove_translations, TRUE);
		}
	}
	
	/*
	 * Is there any untranslated message ?
	 */
	if((g_list_length(po->priv->messages) - po->priv->translated) > 0)
	{
		/*
		 * Then enable the Untranslated buttons/entries in the menus
		 */
		//gtk_widget_set_sensitive(gtranslator_menuitems->next_untranslated, TRUE);
	}

	/*
	 * If there are any translated messages, enable the "remove all
	 *  translations" functionality if needed.
	 */
	if((po->priv->translated > 1) && GtrPreferences.rambo_function)
	{
		//gtk_widget_set_sensitive(gtranslator_menuitems->remove_translations, TRUE);
	}
	
	/*
	 * Hook up the autosaving function if wished.
	 */
	if(GtrPreferences.autosave)
	{
		/*po->autosave_timeout = g_timeout_add(
			(GtrPreferences.autosave_timeout * 60000),
			(GSourceFunc) gtranslator_utils_autosave, po);*/
	}
	
	/*
	 * Set go back buttons sensitive to FALSE
	 */
	/*gtk_widget_set_sensitive(gtranslator_menuitems->first, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->go_back, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->t_first, FALSE);
	gtk_widget_set_sensitive(gtranslator_menuitems->t_go_back, FALSE);*/
	g_warning("hola");
	return TRUE;
}


void 
gtranslator_po_parse_file_from_dialog(GtkWidget * dialog,
				      GtranslatorWindow *window)
{
	gchar *po_file;
	GError *error;
	po_file = g_strdup(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));

	gtranslator_file_dialogs_store_directory(po_file);

	/*
	 * Open the file via our centralized opening function.
	 */
	if(!gtranslator_open(po_file, window, &error)) {
		if(error) {
			//gtranslator_show_message(error->message, NULL);
			g_error_free(error);
		}
	}

	/*
	 * Destroy the dialog 
	 */
	gtk_widget_destroy(dialog);
}
