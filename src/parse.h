/*
 * (C) 2000-2003 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef GTR_PARSE_H
#define GTR_PARSE_H 1

#include <stdio.h>

#include <glib.h>
#include <gtk/gtkwidget.h>

#include <gettext-0.0/config.h>
#include <gettext-0.0/message.h>
#include <gettext-0.0/read-po.h>

#include "messages.h"
#include "header_stuff.h"

typedef enum 
{
  GTR_PARSER_ERROR_GETTEXT,
  GTR_PARSER_ERROR_OTHER,
} GtrParserError;

#define GTR_PARSER_ERROR gtranslator_parser_error_quark()
GQuark gtranslator_parser_error_quark (void);

/*
 * The general po-file structure
 */
typedef struct {
	/*
	 * Absolute file name
	 */
	gchar *filename;
	
	/*
	 * The header 
	 */
	GtrHeader *header;
	
	/*
	 * The whole gettext multi-domain list-of-messages pointer (uses
	 * libgettextpo directly)
	 */
	msgdomain_list_ty *mdlp;
	
	/*
	 * A gettext list-of-messages pointer (uses libgettextpo directly)
	 */
	message_list_ty *messagelist;

 	/*
 	 * All the po->messages are stored here
	 */
	GList *messages;
	
	/*
	 * A pointer to the currently displayed message 
	 */
	GList *current;
	
	/*
	 * The length of messages list (can use 'messagelist->nitems')
	 */
	guint length;

	/*
	 * The obsolete messages are stored within this gchar.
	 */
	gchar *obsolete;
	
	/*
	 * Marks if the file was changed; 
	 */
	guint file_changed : 1;
	
	/*
	 * Is the file write-permitted?
	 */
	gboolean no_write_perms;

	/*
	 * Did we already create the table for the po file.
	 */
	gboolean table_already_created;
	
	/*
	 * The translated entries. 
	 */
	guint translated;
	
	/*
	 * The fuzzy entries. 
	 */
	guint fuzzy;

	/*
	 * The locale charset name.
	 */
	gchar *locale_charset;
} GtrPo;

#define GTR_PO(x) ((GtrPo *)x)

/*
 * The main variable for storing info about file 
 */
extern GtrPo *po;

/*
 * Marks if any file was opened 
 */
extern gboolean file_opened;

/*
 * Marks if the current message was changed; 
 */
extern gboolean message_changed;

/*
 * Functions for parsing a po file into a GtrPo and freeing it.
 */ 
GtrPo *gtranslator_po_parse(const gchar *filename, GError **error);
void gtranslator_po_free(GtrPo *po);

/*
 * Save the file with the given filename.
 */
gboolean gtranslator_save_file(const gchar *name);

/*
 * Callbacks for the widgets
 */
void gtranslator_parse_the_file_from_file_dialog(GtkWidget  * widget, gpointer of_dlg);
void gtranslator_save_file_dialog(GtkWidget  * widget, gpointer sfa_dlg);
void gtranslator_save_current_file_dialog(GtkWidget  * widget, gpointer useless);
void gtranslator_file_revert(GtkWidget  * widget, gpointer useless);
void gtranslator_file_close(GtkWidget  * widget, gpointer useless);

/*
 * Remove all translations from the po file.
 */
void gtranslator_remove_all_translations(void);

/*
 * The compile function.
 */
void compile(GtkWidget  * widget, gpointer useless);

/*
 * The update function (now outsources to update.c).
 */
void update(GtkWidget *widget, gpointer useless);

/*
 * Set the progress bar status/determine the po file status.
 */
void gtranslator_set_progress_bar(void);
void gtranslator_get_translated_count(void);

/*
 * Callback to receive an error message from the gettext parser
 */
void gtranslator_parser_report_error(abstract_po_reader_ty *pop, lex_pos_ty *pos, char *errstr);

#endif
