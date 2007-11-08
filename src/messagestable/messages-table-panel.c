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

#include "application.h"
#include "messages-table-panel.h"
#include "msg.h"
#include "po.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define GTR_MESSAGES_TABLE_PANEL_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_MESSAGES_TABLE_PANEL,     \
						 GtranslatorMessagesTablePanelPrivate))

G_DEFINE_TYPE(GtranslatorMessagesTablePanel, gtranslator_messages_table_panel, GTK_TYPE_VBOX)

#define TABLE_FUZZY_COLOR "#ff0000"
#define TABLE_UNTRANSLATED_COLOR "#a7453e"
#define TABLE_TRANSLATED_COLOR "#00ff00"

struct _GtranslatorMessagesTablePanelPrivate
{
	GtkWidget *treeview;
	GtkListStore *store;
	
	GtranslatorTab *tab;
	
	GdkColor fuzzy;
	GdkColor translated;
	GdkColor untranslated;
};

enum
{
	ID_COLUMN,
	ORIGINAL_COLUMN,
	TRANSLATION_COLUMN,
	COLOR_COLUMN,
	POINTER_COLUMN,
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
gtranslator_messages_table_selection_changed (GtkTreeSelection *selection,
					      GtranslatorMessagesTablePanel *panel)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GList *msg = NULL;
	
	g_return_if_fail(selection != NULL);

	if(gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
	{
		gtk_tree_model_get(model, &iter, POINTER_COLUMN, &msg, -1);
		if(msg != NULL)
			gtranslator_tab_message_go_to(panel->priv->tab, msg);
	}
}

static void
showed_message_cb(GtranslatorTab *tab,
		  GtranslatorMessagesTablePanel *panel)
{
	GtkTreeIter iter;
	GtranslatorPo *po;
	GList *current_item;
	GList *item = NULL;
	GtkTreeSelection *selection;
	
	po = gtranslator_tab_get_po(tab);
	
	current_item = gtranslator_po_get_current_message(po);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(panel->priv->treeview));

	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(panel->priv->store),
					 &iter))
	{
		do{
			gtk_tree_model_get(GTK_TREE_MODEL(panel->priv->store),
					   &iter, POINTER_COLUMN, &item, -1);
			if(item == current_item)
			{
				gtk_tree_selection_select_iter(selection,
							       &iter);
				break;
			}
			
		}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(panel->priv->store),
						&iter));
	}
}


static void
gtranslator_messages_table_panel_draw(GtranslatorMessagesTablePanel *panel)
{
	GtranslatorMessagesTablePanelPrivate *priv = panel->priv;
	
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;
	
	priv->store = gtk_list_store_new (N_COLUMNS,
					  G_TYPE_STRING,
					  G_TYPE_STRING,
					  G_TYPE_STRING,
					  GDK_TYPE_COLOR,
					  G_TYPE_POINTER);
	
	priv->treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL(priv->store));
	gtk_tree_view_expand_all(GTK_TREE_VIEW(priv->treeview));
	
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(priv->treeview), TRUE);
		
	renderer=gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("ID"),
							  renderer,
							  "text", ID_COLUMN,
							  "foreground-gdk", COLOR_COLUMN,
							  NULL);

	gtk_tree_view_column_set_resizable(column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->treeview), column);
		
	renderer=gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Original text"),
							  renderer,
							  "text", ORIGINAL_COLUMN,
							  "foreground-gdk", COLOR_COLUMN,
							  NULL);

	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->treeview), column);
	
	renderer=gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Translated text"),
							  renderer,
							  "text", TRANSLATION_COLUMN,
							  "foreground-gdk", COLOR_COLUMN,
							  NULL);

	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->treeview), column);
	
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(priv->treeview));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);

	g_signal_connect (G_OBJECT(selection), "changed", 
			  G_CALLBACK(gtranslator_messages_table_selection_changed), 
			  panel);
	
	/*g_signal_connect (G_OBJECT(dlg->priv->treeview), "row-expanded", 
			  G_CALLBACK(gtranslator_control_table_node_expanded), 
			  selection);*/
	
	g_signal_connect(G_OBJECT(priv->treeview), "size-allocate",
		G_CALLBACK(gtranslator_tree_size_allocate), NULL );
	
}


static void
gtranslator_messages_table_panel_init (GtranslatorMessagesTablePanel *panel)
{
	GtkWidget *scrolledwindow;
	panel->priv = GTR_MESSAGES_TABLE_PANEL_GET_PRIVATE (panel);
	
	/* Store the colors */
	gdk_color_parse(TABLE_FUZZY_COLOR, &panel->priv->fuzzy);
	gdk_color_parse(TABLE_TRANSLATED_COLOR, &panel->priv->translated);
	gdk_color_parse(TABLE_UNTRANSLATED_COLOR, &panel->priv->untranslated);
	
	gtranslator_messages_table_panel_draw(panel);
	
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(panel), scrolledwindow, TRUE, TRUE, 0);
	
	gtk_container_add(GTK_CONTAINER(scrolledwindow), panel->priv->treeview);
	
	/* Tab */
	panel->priv->tab = 
		gtranslator_window_get_active_tab(gtranslator_application_get_active_window(GTR_APP));
	
	g_signal_connect(panel->priv->tab,
			 "showed-message",
			 G_CALLBACK(showed_message_cb),
			 panel);
}

static void
gtranslator_messages_table_panel_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_messages_table_panel_parent_class)->finalize (object);
}

static void
gtranslator_messages_table_panel_class_init (GtranslatorMessagesTablePanelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorMessagesTablePanelPrivate));

	object_class->finalize = gtranslator_messages_table_panel_finalize;
}

GtkWidget *
gtranslator_messages_table_panel_new (void)
{
	return GTK_WIDGET(g_object_new (GTR_TYPE_MESSAGES_TABLE_PANEL, NULL));
}

void 
gtranslator_messages_table_panel_populate(GtranslatorMessagesTablePanel *table, 
					  GList *messages)
{
	const gchar *msgid, *msgstr;
	GtkTreeIter iter;
	gint id = 1;
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
			gtk_list_store_append(table->priv->store, &iter);
			gtk_list_store_set(table->priv->store, &iter,
					   ID_COLUMN, g_strdup_printf("%d", id),
					   ORIGINAL_COLUMN, msgid,
					   TRANSLATION_COLUMN, msgstr,
					   COLOR_COLUMN, &table->priv->fuzzy,
					   POINTER_COLUMN, messages,
					   -1);
		}
		//Translated
		else if(gtranslator_msg_is_translated(GTR_MSG(messages->data))) 
		{
			gtk_list_store_append(table->priv->store, &iter);
			gtk_list_store_set(table->priv->store, &iter,
					   ID_COLUMN, g_strdup_printf("%d", id),
					   ORIGINAL_COLUMN, msgid,
					   TRANSLATION_COLUMN, msgstr,
					   COLOR_COLUMN, &table->priv->translated,
					   POINTER_COLUMN, messages,
					   -1);
		}
		//Untranslated
		else {
			gtk_list_store_append(table->priv->store, &iter);
			gtk_list_store_set(table->priv->store, &iter,
					   ID_COLUMN, g_strdup_printf("%d", id),
					   ORIGINAL_COLUMN, msgid,
					   TRANSLATION_COLUMN, msgstr,
					   COLOR_COLUMN, &table->priv->untranslated,
					   POINTER_COLUMN, messages,
					   -1);
		}

		id++;
		messages = g_list_next(messages);
	}
}
