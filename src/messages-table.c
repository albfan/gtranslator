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

#include "messages-table.h"
#include "msg.h"
#include "po.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define GTR_MESSAGE_TABLE_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_MESSAGE_TABLE,     \
						 GtranslatorMessageTablePrivate))

G_DEFINE_TYPE(GtranslatorMessageTable, gtranslator_message_table, GTK_TYPE_VBOX)

#define TABLE_FUZZY_COLOR "#ff0000"
#define TABLE_UNTRANSLATED_COLOR "#a7453e"
#define TABLE_TRANSLATED_COLOR "#00ff00"

struct _GtranslatorMessageTablePrivate
{
	GtkWidget *treeview;
	GtkTreeStore *store;
	
	GdkColor fuzzy;
	GdkColor translated;
	GdkColor untranslated;
};

enum
{
	ORIGINAL_COLUMN,
	TRANSLATION_COLUMN,
	COLOR_COLUMN,
	N_COLUMNS
};

static void 
gtranslator_tree_size_allocate(GtkTreeView *widget, 
			       GtkAllocation *allocation, 
			       gpointer data )
{
	/*
	 * Here we will change the column widths so that the message table will look nice.
	 * This function will be called when the size of treeview widget will be changed.
	 */
	GtkTreeViewColumn *col;
	gint width;
	width = allocation->width >> 1;
	col = gtk_tree_view_get_column( widget, ORIGINAL_COLUMN );
	gtk_tree_view_column_set_min_width( col, width );
	gtk_tree_view_column_set_max_width( col, width );
	col = gtk_tree_view_get_column( widget, TRANSLATION_COLUMN );
	gtk_tree_view_column_set_min_width( col, width );
	gtk_tree_view_column_set_max_width( col, width );
}

static void
gtranslator_message_table_draw(GtranslatorMessageTable *panel)
{
	GtranslatorMessageTablePrivate *priv = panel->priv;
	
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;
	
	priv->store = gtk_tree_store_new (N_COLUMNS,
					  G_TYPE_STRING,
					  G_TYPE_STRING,
					  GDK_TYPE_COLOR);
	
	priv->treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL(priv->store));
	gtk_tree_view_expand_all(GTK_TREE_VIEW(priv->treeview));
	
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(priv->treeview), TRUE);
	
	column = gtk_tree_view_column_new();
	
	renderer=gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Original text"),
							  renderer,
							  "text", ORIGINAL_COLUMN,
							  "foreground-gdk", COLOR_COLUMN,
							  NULL);

	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->treeview), column);
	
	renderer=gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Translation text"),
							  renderer,
							  "text", TRANSLATION_COLUMN,
							  "foreground-gdk", COLOR_COLUMN,
							  NULL);

	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->treeview), column);
	
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(priv->treeview));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);

	/*g_signal_connect (G_OBJECT(selection), "changed", 
			  G_CALLBACK(gtranslator_control_table_selection_changed), 
			  dlg);*/
	
	/*g_signal_connect (G_OBJECT(dlg->priv->treeview), "row-expanded", 
			  G_CALLBACK(gtranslator_control_table_node_expanded), 
			  selection);*/
	
	g_signal_connect(G_OBJECT(priv->treeview), "size-allocate",
		G_CALLBACK(gtranslator_tree_size_allocate), NULL );
	
}


static void
gtranslator_message_table_init (GtranslatorMessageTable *panel)
{
	GtkWidget *scrolledwindow;
	panel->priv = GTR_MESSAGE_TABLE_GET_PRIVATE (panel);
	
	/* Store the colors */
	gdk_color_parse(TABLE_FUZZY_COLOR, &panel->priv->fuzzy);
	gdk_color_parse(TABLE_TRANSLATED_COLOR, &panel->priv->translated);
	gdk_color_parse(TABLE_UNTRANSLATED_COLOR, &panel->priv->untranslated);
	
	gtranslator_message_table_draw(panel);
	
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(panel), scrolledwindow, TRUE, TRUE, 0);
	
	gtk_container_add(GTK_CONTAINER(scrolledwindow), panel->priv->treeview);
}

static void
gtranslator_message_table_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_message_table_parent_class)->finalize (object);
}

static void
gtranslator_message_table_class_init (GtranslatorMessageTableClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorMessageTablePrivate));

	object_class->finalize = gtranslator_message_table_finalize;
}

GtkWidget *
gtranslator_message_table_new (GtranslatorPo *po)
{
	return GTK_WIDGET(g_object_new (GTR_TYPE_MESSAGE_TABLE, NULL));
}

void 
gtranslator_messages_table_populate(GtranslatorMessageTable *table, 
				    GList *messages)
{
	const gchar *msgid, *msgstr;
	GtkTreeIter iter;
	/*Message table should store plural forms too*/
	
	g_return_if_fail(table != NULL);
	g_return_if_fail(messages != NULL);

	while (messages) 
	{
		msgid = gtranslator_msg_get_msgid(GTR_MSG(messages->data));
		msgstr = gtranslator_msg_get_msgstr(GTR_MSG(messages->data));
		
		//Fuzzy
		if(gtranslator_msg_is_fuzzy(GTR_MSG(messages->data)))
		{
			gtk_tree_store_append(table->priv->store, &iter, NULL);
			gtk_tree_store_set(table->priv->store, &iter,
					   ORIGINAL_COLUMN, msgid,
					   TRANSLATION_COLUMN, msgstr,
					   COLOR_COLUMN, &table->priv->fuzzy,
					   -1);
		}
		//Translated
		else if(gtranslator_msg_is_translated(GTR_MSG(messages->data))) 
		{
			gtk_tree_store_append(table->priv->store, &iter, NULL);
			gtk_tree_store_set(table->priv->store, &iter,
					   ORIGINAL_COLUMN, msgid,
					   TRANSLATION_COLUMN, msgstr,
					   COLOR_COLUMN, &table->priv->translated,
					   -1);
		}
		//Untranslated
		else {
			gtk_tree_store_append(table->priv->store, &iter, NULL);
			gtk_tree_store_set(table->priv->store, &iter,
					   ORIGINAL_COLUMN, msgid,
					   TRANSLATION_COLUMN, msgstr,
					   COLOR_COLUMN, &table->priv->untranslated,
					   -1);
		}

		messages = g_list_next(messages);
	}
}
