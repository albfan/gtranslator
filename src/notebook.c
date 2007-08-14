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

#include "notebook.h"
#include "tab.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define GTR_NOTEBOOK_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_NOTEBOOK,     \
					 GtranslatorNotebookPrivate))

G_DEFINE_TYPE(GtranslatorNotebook, gtranslator_notebook, GTK_TYPE_NOTEBOOK)

struct _GtranslatorNotebookPrivate
{
	GList *pages;
	
};


static GtkWidget *
build_tab_label (GtranslatorNotebook *nb, 
		 GtranslatorTab      *tab)
{
	GtkWidget *hbox, *label_hbox, *label_ebox;
	GtkWidget *label, *dummy_label;
	GtkWidget *close_button;
	GtkRcStyle *rcstyle;
	GtkWidget *image;
	GtkWidget *spinner;
	GtkWidget *icon;

	hbox = gtk_hbox_new (FALSE, 4);

	label_ebox = gtk_event_box_new ();
	gtk_event_box_set_visible_window (GTK_EVENT_BOX (label_ebox), FALSE);
	gtk_box_pack_start (GTK_BOX (hbox), label_ebox, TRUE, TRUE, 0);

	label_hbox = gtk_hbox_new (FALSE, 4);
	gtk_container_add (GTK_CONTAINER (label_ebox), label_hbox);

	/* setup close button */
	close_button = gtk_button_new ();
	gtk_button_set_relief (GTK_BUTTON (close_button),
			       GTK_RELIEF_NONE);
	/* don't allow focus on the close button */
	gtk_button_set_focus_on_click (GTK_BUTTON (close_button), FALSE);

	/* make it as small as possible */
	rcstyle = gtk_rc_style_new ();
	rcstyle->xthickness = rcstyle->ythickness = 0;
	gtk_widget_modify_style (close_button, rcstyle);
	gtk_rc_style_unref (rcstyle),

	image = gtk_image_new_from_stock (GTK_STOCK_CLOSE,
					  GTK_ICON_SIZE_MENU);
	gtk_container_add (GTK_CONTAINER (close_button), image);
	gtk_box_pack_start (GTK_BOX (hbox), close_button, FALSE, FALSE, 0);

	/*gedit_tooltips_set_tip (nb->priv->title_tips, close_button,
			      _("Close document"), NULL);*/

	/*g_signal_connect (close_button,
			  "clicked",
			  G_CALLBACK (close_button_clicked_cb),
			  tab);*/

	/* setup spinner */
	/*spinner = gedit_spinner_new ();
	gedit_spinner_set_size (GEDIT_SPINNER (spinner), GTK_ICON_SIZE_MENU);
	gtk_box_pack_start (GTK_BOX (label_hbox), spinner, FALSE, FALSE, 0);*/

	/* setup site icon, empty by default */
	icon = gtk_image_new ();
	gtk_box_pack_start (GTK_BOX (label_hbox), icon, FALSE, FALSE, 0);
	
	/* setup label */
	label = gtk_label_new ("");
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_misc_set_padding (GTK_MISC (label), 0, 0);
	gtk_box_pack_start (GTK_BOX (label_hbox), label, FALSE, FALSE, 0);

	dummy_label = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (label_hbox), dummy_label, TRUE, TRUE, 0);
	
	/* Set minimal size */
	/*g_signal_connect (hbox, "style-set",
			  G_CALLBACK (tab_label_style_set_cb), NULL);*/
	
	gtk_widget_show (hbox);
	gtk_widget_show (label_ebox);
	gtk_widget_show (label_hbox);
	gtk_widget_show (label);
	gtk_widget_show (dummy_label);	
	gtk_widget_show (image);
	gtk_widget_show (close_button);
	gtk_widget_show (icon);
	
	g_object_set_data (G_OBJECT (hbox), "label", label);
	g_object_set_data (G_OBJECT (hbox), "label-ebox", label_ebox);
	g_object_set_data (G_OBJECT (hbox), "spinner", spinner);
	g_object_set_data (G_OBJECT (hbox), "icon", icon);
	g_object_set_data (G_OBJECT (hbox), "close-button", close_button);
	g_object_set_data (G_OBJECT (tab), "close-button", close_button);
	//g_object_set_data (G_OBJECT (hbox), "tooltips", nb->priv->title_tips);

	return hbox;
}

static void
gtranslator_notebook_init (GtranslatorNotebook *notebook)
{
	notebook->priv = GTR_NOTEBOOK_GET_PRIVATE (notebook);
	GtranslatorNotebookPrivate *priv = notebook->priv;
	
	priv->pages = NULL;
}

static void
gtranslator_notebook_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_notebook_parent_class)->finalize (object);
}

static void
gtranslator_notebook_class_init (GtranslatorNotebookClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorNotebookPrivate));

	object_class->finalize = gtranslator_notebook_finalize;
}

/***************************** Public funcs ***********************************/

GtkWidget *
gtranslator_notebook_new()
{
	return GTK_WIDGET(g_object_new(GTR_TYPE_NOTEBOOK, NULL));
}

void
gtranslator_notebook_add_page(GtranslatorNotebook *notebook,
			      GtranslatorTab *tab)
{
	GtranslatorNotebookPrivate *priv = notebook->priv;
	GtkWidget *label;

	g_return_if_fail(GTR_IS_NOTEBOOK(notebook));
	g_return_if_fail(GTR_IS_TAB(tab));
	
	label = build_tab_label(notebook, GTR_TAB(tab));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
				 GTK_WIDGET(tab), label);
	priv->pages = g_list_append(priv->pages, tab);
}

GtranslatorTab *
gtranslator_notebook_get_page(GtranslatorNotebook *notebook)
{
	gint num;
	
	num = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
	
	return GTR_TAB(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), num));
}


