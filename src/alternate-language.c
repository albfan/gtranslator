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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "alternate-language.h"
#include "po.h"
#include "view.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define GTR_ALTERNATE_LANG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_ALTERNATE_LANG,     \
						 GtranslatorAlternateLangPrivate))

G_DEFINE_TYPE(GtranslatorAlternateLang, gtranslator_alternate_lang, GTK_TYPE_VBOX)


struct _GtranslatorAlternateLangPrivate
{
	GtkWidget *box;
	GtkWidget *notebook;
	
	GtkWidget *open;
	GtkWidget *close;
	GList *views;
	
	GList *messages;
	GList *current;
	gint nplurals : 1;
};

static void gtranslator_alternate_lang_create_views(GtranslatorAlternateLang *panel);


/* This part of the code should is duplicated from header_stuff.c
 * Grab the header string
 */
static const gchar *
gtranslator_header_get_header(po_file_t gettext_po_file)
{
	const gchar *headerstr;
	if(!(headerstr = 
	     po_file_domain_header(gettext_po_file, NULL)))
	{
		GtkWidget *dialog;

		/*dialog = gtk_message_dialog_new(
			GTK_WINDOW(gtranslator_application),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING,
			GTK_BUTTONS_OK,
			_("No header for this file/domain"));
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);*/
	}
	return headerstr;
}

/*
 * Get plural forms from header
 */
static gchar *
gtranslator_header_get_plural_forms(po_file_t gettext_po_file)
{
	gchar *plural_form;
	const gchar *header;
	
	header = gtranslator_header_get_header(gettext_po_file);
	plural_form = po_header_field(header, "Plural-Forms");
	
	return plural_form;
}

/*
 * Get nplurals variable
 */
static gushort
gtranslator_header_get_nplurals(po_file_t gettext_po_file)
{
	gchar *plural_form;
	gchar *pointer;
	
	plural_form = gtranslator_header_get_plural_forms(gettext_po_file);
	pointer = plural_form;
	
	while(*pointer != '=')
		pointer++;
	pointer++;
	
	//if there are any space between '=' and nplural number pointer++
	while(*pointer == ' ')
		pointer++;
	
	return g_ascii_digit_value(*pointer);
}//duplicated code



static void
gtranslator_alternate_lang_open_file(GtkWidget *dialog,
				     GtranslatorAlternateLang *panel)
{
	gchar *filename;
	GtranslatorPo *po;
	
	filename = g_strdup(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
	po = gtranslator_po_new();
	
	gtranslator_po_parse(po, filename, NULL);
	
	panel->priv->messages = g_list_copy(gtranslator_po_get_messages(po));
	panel->priv->current = panel->priv->messages;
	
	panel->priv->nplurals = gtranslator_header_get_nplurals(
					gtranslator_po_get_po_file(po));
	
	gtranslator_alternate_lang_create_views(panel);
	
	gtk_widget_set_sensitive(panel->priv->close, TRUE);
	
	g_object_unref(po);
	gtk_widget_destroy(dialog);
}

/*
 * File chooser dialog
 */
static GtkWindow *
gtranslator_file_chooser_new (GtkWindow *parent,
			      gchar *title)
{
	GtkWidget *dialog;
	GtkFileFilter *filter;
	
	dialog = gtk_file_chooser_dialog_new(title,
					     parent,
					     GTK_FILE_CHOOSER_ACTION_OPEN,
					     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					     GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					     NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog),GTK_RESPONSE_ACCEPT);
	
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
			
//	gtranslator_file_dialogs_set_directory(&dialog);
		
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));
	gtk_widget_show_all(GTK_WIDGET(dialog));
	return GTK_WINDOW(dialog);
}

static void
on_dialog_response(GtkWidget *dialog,
		   gint response_id,
		   GtranslatorAlternateLang *panel)
{
	switch(response_id)
	{
		case GTK_RESPONSE_ACCEPT: 
			gtranslator_alternate_lang_open_file(dialog, panel);
		break;
		case GTK_RESPONSE_CANCEL:
			gtk_widget_hide(dialog);
		break;
	}
}

/*
 * The "Open file" dialog.
 */
static void
gtranslator_alternate_lang_open_file_dialog(GtkWidget * widget,
					    GtranslatorAlternateLang *panel)
{
	GtkWindow *dialog = NULL;
	
	if(dialog != NULL) {
		gtk_window_present(GTK_WINDOW(dialog));
		return;
	}
	dialog = gtranslator_file_chooser_new (NULL,
					       _("Open alternate PO file"));	

	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialog), TRUE);

	g_signal_connect(GTK_DIALOG(dialog), "response",
			 G_CALLBACK(on_dialog_response), panel);
}

static void
gtranslator_alternate_lang_create_views(GtranslatorAlternateLang *panel)
{
	GtranslatorAlternateLangPrivate * priv = panel->priv;
	GtkWidget *view;
	GtkWidget *label;
	gchar *label_text;
	gint i = 0;
	
	/*Views*/
	do{
		view = gtranslator_view_new();
		label_text = g_strdup_printf(_("Plural %d"), i);
		label = gtk_label_new(label_text);
		gtk_notebook_append_page(GTK_NOTEBOOK(priv->notebook), view, label);
		priv->views = g_list_append(priv->views, view);
		g_free(label_text);
		i++;
	}while(i < priv->nplurals);
}

static void
gtranslator_alternate_lang_close_file(GtkWidget *button,
				      GtranslatorAlternateLang* panel)
{
	gint nviews;
	nviews = g_list_length(panel->priv->views);
	if(nviews <= 0)
		return;
	do{
		nviews--;
		gtk_notebook_remove_page(GTK_NOTEBOOK(panel->priv->notebook),
					 nviews);
	}while(nviews != 0);
	
	gtk_widget_set_sensitive(button, FALSE);
}

static void
gtranslator_alternate_lang_draw (GtranslatorAlternateLang *panel)
{
	GtranslatorAlternateLangPrivate *priv = panel->priv;	
	GtkWidget *view;
	
	/*Box*/
	priv->box = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(priv->box), GTK_BUTTONBOX_START);
	
	/*Buttons*/
	priv->open = gtk_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(GTK_BUTTON(priv->open), "clicked",
			 G_CALLBACK(gtranslator_alternate_lang_open_file_dialog),
			 panel);
	
	priv->close = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_widget_set_sensitive(priv->close, FALSE);
	/*g_signal_connect(GTK_BUTTON(priv->close), "clicked",
			 G_CALLBACK(gtranslator_alternate_lang_close_file),
			 panel);*/
	
	gtk_box_pack_start(GTK_BOX(priv->box), priv->open, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(priv->box), priv->close, FALSE, FALSE, 0);
	
	/*Notebook*/
	priv->notebook = gtk_notebook_new();
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(priv->notebook), FALSE);
	
	/*Add widgets to panel*/
	gtk_box_pack_start(GTK_BOX(panel), priv->box, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(panel), priv->notebook, FALSE, FALSE, 0);
	
}

/*
 * 1. Make a search system
 * 2. Show the message
 */

static void
gtranslator_alternate_lang_init (GtranslatorAlternateLang *panel)
{
	panel->priv = GTR_ALTERNATE_LANG_GET_PRIVATE (panel);
	
	
}

static void
gtranslator_alternate_lang_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_alternate_lang_parent_class)->finalize (object);
}

static void
gtranslator_alternate_lang_class_init (GtranslatorAlternateLangClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorAlternateLangPrivate));

	object_class->finalize = gtranslator_alternate_lang_finalize;
}

GtkWidget *
gtranslator_alternate_lang_new (void)
{
	return GTK_WIDGET (g_object_new (GTR_TYPE_ALTERNATE_LANG, NULL));
}

void
gtranslator_alternate_lang_show_message(GtranslatorAlternateLang *panel,
					po_message_t msg)
{
	if(!msg)
		return;
}

