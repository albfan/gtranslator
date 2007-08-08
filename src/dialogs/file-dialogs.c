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
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <string.h>

#include "file-dialogs.h"
#include "tab.h"
#include "preferences.h"
#include "po.h"
#include "window.h"

typedef enum {
	FILESEL_OPEN,
	FILESEL_SAVE
} FileselMode;



/*
  * Store the given filename's directory for our file dialogs completion-routine.
 */
void
gtranslator_file_dialogs_store_directory(const gchar *filename)
{
	gchar *directory;

	g_return_if_fail(filename!=NULL);
	
	directory=g_path_get_dirname(filename);
	
	gtranslator_config_set_string("informations/last_directory", directory);

	g_free(directory);
}

/*
 * File chooser dialog
 */
static GtkWindow *
gtranslator_file_chooser_new (GtkWindow *parent,
			      FileselMode mode,
			      gchar *title)
{
	GtkWidget *dialog;
	GtkFileFilter *filter;
	
	/*This should change and use libglade*/
	dialog = gtk_file_chooser_dialog_new(title,
				      parent,
				      (mode == FILESEL_SAVE) ? GTK_FILE_CHOOSER_ACTION_SAVE : GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      (mode == FILESEL_SAVE) ? GTK_STOCK_SAVE : GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog),GTK_RESPONSE_ACCEPT);
	if (mode != FILESEL_SAVE)
		{
			filter = gtk_file_filter_new();
			gtk_file_filter_set_name(filter,_("Gettext translation"));
			gtk_file_filter_add_mime_type(filter,"text/x-gettext-translation");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),filter);
	
			filter = gtk_file_filter_new();
			gtk_file_filter_set_name(filter,_("Gettext translation template"));
			gtk_file_filter_add_mime_type(filter,"text/x-gettext-translation-template");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),filter);
			
			filter = gtk_file_filter_new();
			gtk_file_filter_set_name(filter,_("All files"));
			gtk_file_filter_add_pattern(filter,"*");
			gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),filter);
			
//			gtranslator_file_dialogs_set_directory(&dialog);
		} 
		
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));
	gtk_widget_show_all(GTK_WIDGET(dialog));
	return GTK_WINDOW(dialog);
}

/*
 * Gtkfilechooser response analyser
 */
static void
gtranslator_file_chooser_analyse(gpointer dialog,
				 FileselMode mode,
				 GtranslatorWindow *window)
{	
	gint reply;

	reply = gtk_dialog_run(GTK_DIALOG (dialog));
	switch (reply){
		case GTK_RESPONSE_ACCEPT:
			if (mode == FILESEL_OPEN){
				//gtranslator_po_parse_file_from_dialog(GTK_WIDGET(dialog),
				//				      window);
			} else {
				gtranslator_save_file_dialog(GTK_WIDGET(dialog), window);
			}
			break;
		case GTK_RESPONSE_CANCEL:
			gtk_widget_hide(GTK_WIDGET(dialog));
			break;
		case GTK_RESPONSE_DELETE_EVENT:
			gtk_widget_hide(GTK_WIDGET(dialog));
			break;
		default:
			break;
	}
}

/*
 * A callback for Overwrite in Save as
 */
static void
gtranslator_overwrite_file(GtkWidget * widget,
			   GtranslatorWindow *window)
{
	GError *error;
	//gtranslator_save_file(current_page->po,current_page->po->filename, &error);
	/*
	 * TODO: Should close the file and open the new saved file
	 */
	//gtranslator_open_file(current_page->po->filename);
	//gtranslator_open(current_page->po->filename, window, &error);
}

/*
 * A callback for OK in Save as... dialog 
 */
void 
gtranslator_save_file_dialog(GtkWidget *widget,
			     GtranslatorWindow *window)
{
	gchar *po_file,
	      *po_file_normalized;
	GtranslatorPo *po;
	
	po = gtranslator_window_get_current_po(window);
	
 	po_file = g_strdup(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget)));
	po_file_normalized = g_utf8_normalize( po_file, -1, G_NORMALIZE_DEFAULT_COMPOSE);
	g_free(po_file);
	po_file = po_file_normalized;

	if (g_file_test(po_file, G_FILE_TEST_EXISTS))
	{
		gtranslator_po_set_filename(po, po_file);
		
		GtkWidget *dialog, *button;
	
		dialog = gtk_message_dialog_new (NULL,
						 GTK_DIALOG_MODAL,
						 GTK_MESSAGE_QUESTION,
						 GTK_BUTTONS_CANCEL,
						 _("The file '%s' already exists. Do you want overwrite it?"),
						 po_file);
		
		button = gtk_dialog_add_button (GTK_DIALOG (dialog), "Overwrite", 1);
		
		g_signal_connect (G_OBJECT (button), "clicked",
			G_CALLBACK (gtranslator_overwrite_file), window);
		
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
	}
	g_free(po_file);
	gtk_widget_destroy(GTK_WIDGET(widget));
}

/*
 * The "Open file" dialog.
 */
void
gtranslator_open_file_dialog(GtkWidget * widget,
			     GtranslatorWindow *window)
{
	GtkWindow *dialog = NULL;
	/*if (current_page && !gtranslator_should_the_file_be_saved_dialog(current_page)) {
		if (dialog)
			gtk_widget_destroy(GTK_WIDGET(dialog));
		return;
	}*/
	
	if(dialog != NULL) {
		gtk_window_present(GTK_WINDOW(dialog));
		return;
	}
	dialog = gtranslator_file_chooser_new (GTK_WINDOW(window), 
					       FILESEL_OPEN,
					       _("Open file for translation"));	

	/*
	 * With the gettext parser/writer API, we can't currently read/write
	 * to remote files with gnome-vfs. Eventually, we should intercept
	 * remote requests and use gnome-vfs to retrieve a temporary file to 
	 * work on, and transmit it back when saved.
	 */
	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialog), TRUE);

	gtranslator_file_chooser_analyse((gpointer) dialog, FILESEL_OPEN, window);
}


/*
 * "Save as" dialog.
 */
void
gtranslator_save_file_as_dialog(GtkWidget * widget,
				GtranslatorWindow *window)
{
	GtkWindow *dialog = NULL;
	GtranslatorTab *current_page;
	GtranslatorPo *po;
	
	if(dialog != NULL) {
		gtk_window_present(GTK_WINDOW(dialog));
		return;
	}
  
	current_page = gtranslator_window_get_current_tab(window);
	po = gtranslator_tab_get_po(current_page);
	if(gtranslator_po_get_write_perms(po)==FALSE ||
	   strstr((const char*)gtranslator_po_get_filename, "/.gtranslator/"))
	{
		dialog = gtranslator_file_chooser_new(NULL, FILESEL_SAVE,
							_("Save file as..."));
	}
	else
	{
		dialog = gtranslator_file_chooser_new(NULL, FILESEL_SAVE,
						_("Save local copy of file as..."));
		
		/*
		 * Set a local filename in the users home directory with the 
		 *  same filename as the original but with a project prefix
		 *   (e.g. "gtranslator-tr.po").
		 */ 
		gtk_file_chooser_select_filename(GTK_FILE_CHOOSER(dialog),
						 gtranslator_po_get_filename(po));
		gtranslator_file_dialogs_store_directory(gtranslator_po_get_filename(po));
	}

	/*
	 * With the gettext parser/writer API, we can't currently read/write
	 * to remote files with gnome-vfs. Eventually, we should intercept
	 * remote requests and use gnome-vfs to retrieve a temporary file to 
	 * work on, and transmit it back when saved.
	 */
	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialog), TRUE);

	gtranslator_file_chooser_analyse((gpointer) dialog, FILESEL_SAVE, window);
	
	//gtranslator_dialog_show(&dialog, "gtranslator -- save");
}



