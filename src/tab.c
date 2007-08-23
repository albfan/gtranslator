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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "comment-panel.h"
#include "draw-spaces.h"
#include "io-error-message-area.h"
#include "message-area.h"
#include "msg.h"
#include "tab.h"
#include "panel.h"
#include "po.h"
#include "prefs.h"
#include "view.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#ifdef HAVE_GTKSPELL
#include <gtkspell/gtkspell.h>
#endif

#define GTR_TAB_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_TAB,     \
					 GtranslatorTabPrivate))

#define MAX_PLURALS 6

G_DEFINE_TYPE(GtranslatorTab, gtranslator_tab, GTK_TYPE_VBOX)

struct _GtranslatorTabPrivate
{
	GtranslatorPo *po;
	//GtrMessagesTable *message_table;
	
	GtkWidget *table_pane;
	GtkWidget *content_pane;
	GtranslatorPanel *panel;
	GtranslatorCommentPanel *comment;
	
	/*Message area*/
	GtkWidget *message_area;
	
	/*Original text*/
	GtkWidget *text_notebook;
	GtkWidget *text_msgid;
	GtkWidget *text_msgid_plural;
	
	/*Translated text*/
	GtkWidget *trans_notebook;
	GtkWidget *trans_msgstr[MAX_PLURALS];
	
	/*Status widgets*/
	GtkWidget *translated;
	GtkWidget *fuzzy;
	GtkWidget *untranslated;
	
	#ifdef HAVE_GTKSPELL
	GtkSpell *gtrans_spell[MAX_PLURALS];
	#endif
};

static void
gtranslator_page_dirty(GtkTextBuffer *textbuffer,
		       GtranslatorTab *tab) 
{
	g_assert(tab != NULL);
	
	//page->po->file_changed = TRUE;
	
	// TODO: make notebook tab go red with an asterisk to mark an unsaved page
	
	//Enable save and revert items
	/*gtk_widget_set_sensitive(gtranslator_menuitems->save, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->revert, TRUE);
	gtk_widget_set_sensitive(gtranslator_menuitems->t_save, TRUE);*/
}

/*
 * Write the change back to the gettext PO instance in memory and
 * mark the page dirty
 */
static void 
gtranslator_message_translation_update(GtkTextBuffer *textbuffer,
				       GtranslatorTab *tab)
{
	GtkTextIter start, end;
	GtkTextBuffer *buf;
	GList *msg_aux;
	GtranslatorMsg *msg;
	const gchar *check;
	gchar *translation;
	gint i;
	
	/* Work out which message this is associated with */
	
	msg_aux = gtranslator_po_get_current_message(tab->priv->po);
	msg = msg_aux->data;
	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tab->priv->trans_msgstr[0]));
	if(textbuffer == buf)
	{
		/* Get message as UTF-8 buffer */
		gtk_text_buffer_get_bounds(textbuffer, &start, &end);
		translation = gtk_text_buffer_get_text(textbuffer, &start, &end, TRUE);
		
		/* TODO: convert to file's own encoding if not UTF-8 */
		
		/* Write back to PO file in memory */
		if(!(check = gtranslator_msg_get_msgid_plural(msg))) {
			gtranslator_msg_set_msgstr(msg, translation);
		}
		else {
			gtranslator_msg_set_msgstr_plural(msg, 0, translation);
			//free(check);
		}
		g_free(translation);
		
		/* Activate 'save', 'revert' etc. */
		gtranslator_page_dirty(textbuffer, tab);
		return;
	}
	i=1;
	while(i < (gint)GtrPreferences.nplurals) {
		/* Know when to break out of the loop */
		if(!tab->priv->trans_msgstr[i]) {
			break;
		}
		
		/* Have we reached the one we want yet? */
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tab->priv->trans_msgstr[i]));
		if(textbuffer != buf) {
			i++;
			continue;
		}
		
		/* Get message as UTF-8 buffer */
		gtk_text_buffer_get_bounds(textbuffer, &start, &end);
		translation = gtk_text_buffer_get_text(textbuffer, &start, &end, TRUE);
		
		/* TODO: convert to file's own encoding if not UTF-8 */
		
		/* Write back to PO file in memory */
		gtranslator_msg_set_msgstr_plural(msg, i, translation);

		/* Activate 'save', 'revert' etc. */
		gtranslator_page_dirty(textbuffer, tab);
		return;
		
	}

	/* Shouldn't get here */
	g_return_if_reached();
}


#ifdef HAVE_GTKSPELL
void
gtranslator_attach_gskspell(GtranslatorTab *tab)
{
	gint i;
	/*
	 * Use instant spell checking via gtkspell only if the corresponding
	 *  setting in the preferences is set.
	 */
	if(GtrPreferences.instant_spell_check)
	{
		/*
		 * Start up gtkspell if not already done.
		 */ 
		GError *error = NULL;
		gchar *errortext = NULL;
		
		guint i;
		for(i = 0; i <= (gint)GtrPreferences.nplurals; i++) 
		{
			if(tab->priv->gtrans_spell[i] == NULL && tab->priv->trans_msgstr[i] != NULL)
			{
				tab->priv->gtrans_spell[i] = NULL;
				tab->priv->gtrans_spell[i] = 
					gtkspell_new_attach(GTK_TEXT_VIEW(tab->priv->trans_msgstr[i]),
							    NULL, &error);
				if (tab->priv->gtrans_spell[i] == NULL) {
					g_print(_("gtkspell error: %s\n"), error->message);
					errortext = g_strdup_printf(_("GtkSpell was unable to initialize.\n %s"),
								    error->message);
					g_error_free(error);
		    		}
			}
			else
				break;
		}
	} else {
		i = 0;
		do{
			if(tab->priv->gtrans_spell[i] != NULL) {
				gtkspell_detach(tab->priv->gtrans_spell[i]);
				tab->priv->gtrans_spell[i] = NULL;
			}
			i++;
		}while(i < (gint)GtrPreferences.nplurals);
	}
}
#endif

static GtkWidget *
gtranslator_tab_append_page(const gchar *tab_label,
			    GtkWidget *notebook)
{
	GtkWidget *scroll;
	GtkWidget *label;
	GtkWidget *widget;
	
	label = gtk_label_new(tab_label);
	
	scroll = gtk_scrolled_window_new(NULL, NULL);
	
	widget = gtranslator_view_new();
	
	g_signal_connect(widget, "event-after",
			 G_CALLBACK(on_event_after), NULL);
	
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll),
					      widget);
	
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	
	gtk_widget_show_all(scroll);
	
	
	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scroll, label);
	return widget;
}

static void
gtranslator_message_plural_forms(GtranslatorTab *tab,
				 GtranslatorMsg *msg)
{
	GtkTextBuffer *buf;
	const gchar *msgstr_plural;
	gint i;

	
	/*
	 * Should show the number of plural forms defined in header
	 */
	for(i = 0; i < (gint)GtrPreferences.nplurals ; i++)
	{
		msgstr_plural = gtranslator_msg_get_msgstr_plural(msg, i);
		if(msgstr_plural)
		{
			buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tab->priv->trans_msgstr[i]));
			gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(buf));
			gtk_text_buffer_set_text(buf, (gchar*)msgstr_plural, -1);
			gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(buf));
		}
	}
}

static void
status_widgets(GtkWidget *buffer,
	       GtranslatorTab *tab)
{
	GList *msg = gtranslator_po_get_current_message(tab->priv->po);
	
	if(gtranslator_msg_is_fuzzy(msg->data))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tab->priv->fuzzy), TRUE);
	
	else if(gtranslator_msg_is_translated(msg->data))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tab->priv->translated), TRUE);
	
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tab->priv->untranslated), TRUE);

}


static void
set_message_area (GtranslatorTab  *tab,
                  GtkWidget *message_area)
{
        if (tab->priv->message_area == message_area)
                return;

        if (tab->priv->message_area != NULL)
                gtk_widget_destroy (tab->priv->message_area);

        tab->priv->message_area = message_area;

        if (message_area == NULL)
                return;

        gtk_box_pack_start (GTK_BOX (tab),
                            tab->priv->message_area,
                            FALSE,
                            FALSE,
                            0);         

        g_object_add_weak_pointer (G_OBJECT (tab->priv->message_area), 
                                   (gpointer *)&tab->priv->message_area);
}


static void
gtranslator_tab_draw (GtranslatorTab *tab)
{
	GtkWidget *vertical_box;
	GtkWidget *status_box;
	GtkWidget *status_label;
	GtkTextBuffer *buf;
	
	gchar *label;
	gint i = 0;
	
	GtranslatorTabPrivate *priv = tab->priv;
	
	
	/*
	 * Content pane
	 */
	priv->content_pane = gtk_vpaned_new();
	
	/*
	 * Panel
	 */
	priv->panel = GTR_PANEL(gtranslator_panel_new(GTK_ORIENTATION_HORIZONTAL));
	
	/*
	 * Table pane
	 */
	priv->table_pane = gtk_hpaned_new();
	
	gtk_paned_set_position(GTK_PANED(priv->content_pane), 0);

	/*
	 * Pack the comments pane and the main content
	 */
	vertical_box=gtk_vbox_new(FALSE, 0);
	gtk_paned_pack1(GTK_PANED(priv->content_pane), GTK_WIDGET(priv->panel), TRUE, FALSE);
	gtk_paned_pack2(GTK_PANED(priv->content_pane), vertical_box, FALSE, TRUE);
	
	/*
	 * Orignal text widgets
	 */
	priv->text_notebook = gtk_notebook_new();
	priv->text_msgid = gtranslator_tab_append_page(_("Singular"),
						       priv->text_notebook);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->text_msgid), FALSE);
	priv->text_msgid_plural = gtranslator_tab_append_page(_("Plural"),
							      priv->text_notebook);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->text_msgid_plural), FALSE);
	gtk_box_pack_start(GTK_BOX(vertical_box), priv->text_notebook, TRUE, TRUE, 0);

	/*
	 * Status widgets
	 */
	status_box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vertical_box), status_box, FALSE, FALSE, 0);
	status_label = gtk_label_new(_("Translation status:"));
	gtk_box_pack_start(GTK_BOX(status_box), status_label, FALSE, FALSE, 0);
	
	priv->translated = gtk_radio_button_new_with_label(NULL, _("Translated"));
	gtk_widget_set_sensitive(priv->translated, FALSE);
	gtk_box_pack_start(GTK_BOX(status_box), priv->translated, FALSE, FALSE, 0);
	priv->fuzzy = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->translated),
								  _("Fuzzy"));
	gtk_widget_set_sensitive(priv->fuzzy, FALSE);
	gtk_box_pack_start(GTK_BOX(status_box), priv->fuzzy, FALSE, FALSE, 0);
	priv->untranslated = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(priv->translated),
									 _("Untranslated"));
	gtk_widget_set_sensitive(priv->untranslated, FALSE);
	gtk_box_pack_start(GTK_BOX(status_box), priv->untranslated, FALSE, FALSE, 0);
	
	/*
	 * Translation widgets
	 */
	priv->trans_notebook = gtk_notebook_new();
	do{
		label = g_strdup_printf(_("Plural %d"), i+1);
		priv->trans_msgstr[i] = gtranslator_tab_append_page(label,
								    priv->trans_notebook);
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->trans_msgstr[i]));
		g_signal_connect(buf, "end-user-action",
				 G_CALLBACK(gtranslator_message_translation_update),
				 tab);
		
		/*I need to create my own signal to manage the status widgets*/
		if(i == 0)
		g_signal_connect(buf, "end-user-action",
			 G_CALLBACK(status_widgets), tab);
		i++;
		g_free(label);
	}while(i < (gint)GtrPreferences.nplurals);
	
	gtk_box_pack_start(GTK_BOX(vertical_box), priv->trans_notebook, TRUE, TRUE, 0);	
	
	gtk_paned_pack2(GTK_PANED(priv->table_pane), priv->content_pane, FALSE, FALSE);
}

static void
gtranslator_tab_init (GtranslatorTab *tab)
{
	GtkWidget *image;
	
	tab->priv = GTR_TAB_GET_PRIVATE (tab);
	
	gtranslator_tab_draw(tab);
	
	/* Comment panel */
	tab->priv->comment = GTR_COMMENT_PANEL(gtranslator_comment_panel_new());
	image = gtk_image_new_from_stock(GTK_STOCK_INDEX,
					 GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtranslator_panel_add_item(tab->priv->panel, GTK_WIDGET(tab->priv->comment),
				   _("Comment"), image);
	
	#ifdef HAVE_GTKSPELL
	gtranslator_attach_gskspell(tab);
	#endif
	
	gtk_box_pack_start(GTK_BOX(tab), tab->priv->table_pane, TRUE, TRUE, 0);
}

static void
gtranslator_tab_finalize (GObject *object)
{
	GtranslatorTab *tab = GTR_TAB(object);
	gint i;
	
	if(tab->priv->po)
		g_object_unref(tab->priv->po);
	
	G_OBJECT_CLASS (gtranslator_tab_parent_class)->finalize (object);
}

static void
gtranslator_tab_class_init (GtranslatorTabClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorTabPrivate));

	object_class->finalize = gtranslator_tab_finalize;
}

/***************************** Public funcs ***********************************/

GtranslatorTab *
gtranslator_tab_new (GtranslatorPo *po)
{
	GtranslatorTab *tab;
	
	tab = g_object_new (GTR_TYPE_TAB, NULL);
	
	if(po)
		tab->priv->po = po;
	
	gtk_widget_show_all(GTK_WIDGET(tab));
	return tab;
}

GtranslatorPo *
gtranslator_tab_get_po(GtranslatorTab *tab)
{
	return tab->priv->po;
}

gint
gtranslator_tab_get_active_text_tab(GtranslatorTab *tab)
{
	return gtk_notebook_get_current_page(GTK_NOTEBOOK(tab->priv->text_notebook));
}

gint
gtranslator_tab_get_active_trans_tab(GtranslatorTab *tab)
{
	return gtk_notebook_get_current_page(GTK_NOTEBOOK(tab->priv->trans_notebook));
}

GtranslatorView *
gtranslator_tab_get_active_view(GtranslatorTab *tab)
{
	gint num;
	
	num = gtk_notebook_get_current_page(GTK_NOTEBOOK(tab->priv->trans_notebook));
	return GTR_VIEW(tab->priv->trans_msgstr[num]);
}

void
gtranslator_tab_show_message(GtranslatorTab *tab,
			     GtranslatorMsg *msg)
{
	GtranslatorTabPrivate *priv = tab->priv;
	GtranslatorPo *po;
	GtkTextBuffer *buf;
	const gchar *msgid, *msgid_plural;
	const gchar *msgstr, *msgstr_plural;
	g_return_if_fail(GTR_IS_TAB(tab));
	
	po = priv->po;
	
	gtranslator_po_update_current_message(po, msg);
	msgid = gtranslator_msg_get_msgid(msg);
	if(msgid) {
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->text_msgid));
		gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(buf));
		gtk_text_buffer_set_text(buf, (gchar*)msgid, -1);
		gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(buf));
	}
	msgid_plural = gtranslator_msg_get_msgid_plural(msg);
	if(!msgid_plural) {
		msgstr = gtranslator_msg_get_msgstr(msg);
		/*
		 * Disable notebook tabs
		 */
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(priv->text_notebook), FALSE);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->text_notebook), 0);
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(priv->trans_notebook), FALSE);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(priv->trans_notebook), 0);
		if(msgstr) 
		{
			buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->trans_msgstr[0]));
			gtk_source_buffer_begin_not_undoable_action(GTK_SOURCE_BUFFER(buf));
			gtk_text_buffer_set_text(buf, (gchar*)msgstr, -1);
			gtk_source_buffer_end_not_undoable_action(GTK_SOURCE_BUFFER(buf));
		}
	}
	else {
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(tab->priv->text_notebook), TRUE);
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(tab->priv->trans_notebook), TRUE);
		buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tab->priv->text_msgid_plural));
		gtk_text_buffer_set_text(buf, (gchar*)msgid_plural, -1);
		gtranslator_message_plural_forms(tab, msg);
	}
	
	/* Comment */
	gtranslator_comment_panel_set_text(tab->priv->comment,
					   gtranslator_msg_get_extracted_comments(msg));
}


void 
gtranslator_message_go_to(GtranslatorTab *tab,
			  GList * to_go)
{
	GtranslatorPo *po;
	static gint pos = 0;
	GList *current_msg;
	const gchar *message_error;
	GtkWidget *message_area;
 
	g_return_if_fail (to_go!=NULL);
		
	po = tab->priv->po;
	
	//gtranslator_message_update();
	
	
	current_msg = gtranslator_po_get_current_message(po);
	message_error = gtranslator_msg_check(current_msg->data);
	if(message_error == NULL)
	{
		gtranslator_tab_show_message(tab, to_go->data);
		set_message_area(tab, NULL);
	}
	else
	{
		message_area = create_error_message_area(_("There is an error in the message:"),
							 message_error);
		set_message_area(tab, message_area);
		return;
	}
}
