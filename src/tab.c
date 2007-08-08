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

#include "draw-spaces.h"
#include "tab.h"
#include "po.h"
#include "prefs.h"
#include "view.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define GTR_TAB_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_TAB,     \
					 GtranslatorTabPrivate))

#define MAX_PLURALS 9

G_DEFINE_TYPE(GtranslatorTab, gtranslator_tab, GTK_TYPE_VBOX)

struct _GtranslatorTabPrivate
{
	GtranslatorPo *po;
	//GtrMessagesTable *message_table;
	
	GtkWidget *table_pane;
	GtkWidget *content_pane;
	
	GtkWidget *comment;
	GtkWidget *edit_button;
	
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
	
};




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
gtranslator_tab_draw (GtranslatorTab *tab)
{
	GtkWidget *vertical_box;
	GtkWidget *horizontal_box;
	GtkWidget *comments_scrolled_window;
	GtkWidget *comments_viewport;
	GtkWidget *status_box;
	GtkWidget *status_label;
	
	gchar *label;
	gint i = 0;
	
	GtranslatorTabPrivate *priv = tab->priv;

	/*
	 * Content pane
	 */
	priv->content_pane = gtk_vpaned_new();
	
	/*
	 * Table pane
	 */
	priv->table_pane = gtk_hpaned_new();
	
	horizontal_box=gtk_hbox_new(FALSE, 1);
	/*
	 * Set up the scrolling window for the comments display
	 */	
	comments_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(comments_scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(horizontal_box), comments_scrolled_window, TRUE, TRUE, 0);
	
	comments_viewport = gtk_viewport_new(NULL, NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(comments_scrolled_window),
					      comments_viewport);
	
	priv->comment=gtk_label_new("");
	gtk_container_add(GTK_CONTAINER(comments_viewport), priv->comment);
	
	priv->edit_button=gtk_button_new_with_label(_("Edit comment"));
	gtk_widget_set_sensitive(priv->edit_button, FALSE);
	gtk_box_pack_end(GTK_BOX(horizontal_box), priv->edit_button,
		FALSE, FALSE, 0);
	
	gtk_paned_set_position(GTK_PANED(priv->content_pane), 0);

	/*
	 * Pack the comments pane and the main content
	 */
	vertical_box=gtk_vbox_new(FALSE, 0);
	gtk_paned_pack1(GTK_PANED(priv->content_pane), horizontal_box, TRUE, FALSE);
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
		i++;
		g_free(label);
	}while(i < (gint)GtrPreferences.nplurals);
	gtk_box_pack_start(GTK_BOX(vertical_box), priv->trans_notebook, TRUE, TRUE, 0);	
	
	gtk_paned_pack2(GTK_PANED(priv->table_pane), priv->content_pane, FALSE, FALSE);
}

static void
gtranslator_tab_init (GtranslatorTab *tab)
{
	tab->priv = GTR_TAB_GET_PRIVATE (tab);
	
	gtranslator_tab_draw(tab);
	
	gtk_box_pack_start(GTK_BOX(tab), tab->priv->table_pane, TRUE, TRUE, 0);
}

static void
gtranslator_tab_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_tab_parent_class)->finalize (object);
}

static void
gtranslator_tab_class_init (GtranslatorTabClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorTabPrivate));

	object_class->finalize = gtranslator_tab_finalize;
}

static void
gtranslator_tab_set_po(GtranslatorTab *tab,
		       GtranslatorPo *po)
{
	tab->priv->po = po;
}

GtkWidget *
gtranslator_tab_new (GtranslatorPo *po)
{
	GtranslatorTab *tab;
	
	tab = g_object_new (GTR_TYPE_TAB, NULL);
	
	if(po)
		gtranslator_tab_set_po(tab, po);
	
	gtk_widget_show_all(GTK_WIDGET(tab));
	return GTK_WIDGET(tab);
}

GtranslatorPo *
gtranslator_tab_get_po(GtranslatorTab *tab)
{
	return tab->priv->po;
}

void
gtranslator_tab_set_trans_text(GtranslatorTab *tab,
			       gchar *text,
			       GCallback func,
			       gint index)
{
	GtkTextBuffer *buf;
	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tab->priv->trans_msgstr[index]));
	gtk_text_buffer_set_text(buf, (gchar*)text, -1);
	/*This should connected once*/
	g_signal_connect(buf, "end-user-action",
			 G_CALLBACK(func), tab);
}
