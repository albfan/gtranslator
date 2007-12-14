/*
 * Copyright (C) 2007   Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *			Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Kevin Vandersloot <kfv101@psu.edu>
 *			Thomas Ziehmer <thomas@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
			Seán de Búrca <leftmostcat@gmail.com>
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
#include "message-table.h"
#include "msg.h"
#include "po.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define GTR_MESSAGE_TABLE_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE (	\
							(object),		\
							GTR_TYPE_MESSAGE_TABLE,	\
							GtranslatorMessageTablePrivate))

G_DEFINE_TYPE(GtranslatorMessageTable, gtranslator_message_table, GTK_TYPE_VBOX)

#define TABLE_FUZZY_COLOR "#ff0000"
#define TABLE_UNTRANSLATED_COLOR "#a7453e"
#define TABLE_TRANSLATED_COLOR "#00ff00"

struct _GtranslatorMessageTablePrivate
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
gtranslator_tree_size_allocate (GtkTreeView *widget, 
				GtkAllocation *allocation, 
				GtranslatorMessageTable *table)
{
	/*
	 * Here we will change the column widths so that the message table will look nice.
	 * This function will be called when the size of treeview widget will be changed.
	 */
	GtkTreeViewColumn *col1, *col2;
	gint width;
	GList *cells;
	GtkTreeIter iter;
	GtkTreePath *path;

	width = allocation->width >> 1;
	col1 = gtk_tree_view_get_column( widget, ORIGINAL_COLUMN );
	gtk_tree_view_column_set_min_width( col1, width );
	gtk_tree_view_column_set_max_width( col1, width );
	col2 = gtk_tree_view_get_column( widget, TRANSLATION_COLUMN );
	gtk_tree_view_column_set_min_width( col2, width );
	gtk_tree_view_column_set_max_width( col2, width );

	/*
	 * We need to set the wrap width of the renderer.
	 */
	cells = gtk_tree_view_column_get_cell_renderers(col1);
	while(cells != NULL)
	{
		g_object_set(cells->data, "wrap-width", width, NULL);
		cells = cells->next;
	}
	g_list_free(cells);
	
	cells = gtk_tree_view_column_get_cell_renderers(col2);
	while(cells != NULL)
	{
		g_object_set(cells->data, "wrap-width", width, NULL);
		cells = cells->next;
	}
	g_list_free(cells);
	
	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(table->priv->store),
					 &iter))
	{
		do{
			path = gtk_tree_model_get_path(GTK_TREE_MODEL(table->priv->store), &iter);
			gtk_tree_model_row_changed(GTK_TREE_MODEL(table->priv->store),
						   path,
						   &iter);
			gtk_tree_path_free(path);
			
		}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(table->priv->store),
						&iter));
	}
}

static void
gtranslator_message_table_selection_changed (GtkTreeSelection *selection,
					     GtranslatorMessageTable *table)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GList *msg = NULL;
	GList *current_msg = NULL;
	GtranslatorPo *po;
	
	g_return_if_fail(selection != NULL);

	po = gtranslator_tab_get_po(table->priv->tab);
	current_msg = gtranslator_po_get_current_message(po);
	
	if(gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
	{
		gtk_tree_model_get(model, &iter, POINTER_COLUMN, &msg, -1);
		if(msg != NULL && msg != current_msg)
			gtranslator_tab_message_go_to(table->priv->tab, msg);
	}
}

static void
showed_message_cb (GtranslatorTab *tab,
		   GtranslatorMessageTable *table)
{
	GtkTreeIter iter;
	GtranslatorPo *po;
	GList *current_item;
	GList *item = NULL;
	GtkTreeSelection *selection;
	GtkTreePath *path;
	
	po = gtranslator_tab_get_po(tab);
	
	current_item = gtranslator_po_get_current_message(po);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(table->priv->treeview));

	if(gtk_tree_model_get_iter_first(GTK_TREE_MODEL(table->priv->store),
					 &iter))
	{
		do{
			gtk_tree_model_get(GTK_TREE_MODEL(table->priv->store),
					   &iter, POINTER_COLUMN, &item, -1);
			if(item == current_item)
			{
				gtk_tree_selection_select_iter(selection,
							       &iter);
				break;
			}
			
		}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(table->priv->store),
						&iter));
	}
	path = gtk_tree_model_get_path(GTK_TREE_MODEL(table->priv->store),
				       &iter);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(table->priv->treeview),
				     path,
				     NULL,
				     FALSE,
				     0.0, 0.0);
	gtk_tree_path_free(path);
}


static void
gtranslator_message_table_draw (GtranslatorMessageTable *table)
{
	GtranslatorMessageTablePrivate *priv = table->priv;
	
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
		
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("ID"),
							  renderer,
							  "text", ID_COLUMN,
							  "foreground-gdk", COLOR_COLUMN,
							  NULL);

	gtk_tree_view_column_set_resizable(column, FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->treeview), column);
		
	renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
	
	column = gtk_tree_view_column_new_with_attributes(_("Original Text"),
							  renderer,
							  "text", ORIGINAL_COLUMN,
							  "foreground-gdk", COLOR_COLUMN,
							  NULL);

	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->treeview), column);
	
	renderer=gtk_cell_renderer_text_new();
	g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
	
	column = gtk_tree_view_column_new_with_attributes(_("Translated Text"),
							  renderer,
							  "text", TRANSLATION_COLUMN,
							  "foreground-gdk", COLOR_COLUMN,
							  NULL);

	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(priv->treeview), column);
	
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(priv->treeview));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);

	g_signal_connect (G_OBJECT(selection), "changed", 
			  G_CALLBACK(gtranslator_message_table_selection_changed), 
			  table);
	
	g_signal_connect(G_OBJECT(priv->treeview), "size-allocate",
		G_CALLBACK(gtranslator_tree_size_allocate), table );
	
}


static void
gtranslator_message_table_init (GtranslatorMessageTable *table)
{
	GtkWidget *scrolledwindow;

	table->priv = GTR_MESSAGE_TABLE_GET_PRIVATE (table);

	/* Store the colors */
	gdk_color_parse(TABLE_FUZZY_COLOR, &table->priv->fuzzy);
	gdk_color_parse(TABLE_TRANSLATED_COLOR, &table->priv->translated);
	gdk_color_parse(TABLE_UNTRANSLATED_COLOR, &table->priv->untranslated);

	gtranslator_message_table_draw(table);

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(table), scrolledwindow, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  table->priv->treeview);
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
gtranslator_message_table_new (GtkWidget *tab)
{
	GtranslatorMessageTable *obj = GTR_MESSAGE_TABLE(g_object_new (GTR_TYPE_MESSAGE_TABLE, NULL));
	obj->priv->tab = GTR_TAB(tab);
	g_signal_connect(obj->priv->tab,
			 "showed-message",
			 G_CALLBACK(showed_message_cb),
			 obj);
	return GTK_WIDGET(obj);
}

void 
gtranslator_message_table_populate(GtranslatorMessageTable *table, 
				   GList *messages)
{
	const gchar *msgid, *msgstr;
	GdkColor *status_color;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreePath *path;
	GtkTreeRowReference *row;
	gint id = 1;

	g_return_if_fail(table != NULL);
	g_return_if_fail(messages != NULL);

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(table->priv->treeview));

	while (messages)
	{
		msgid = gtranslator_msg_get_msgid(GTR_MSG(messages->data));
		msgstr = gtranslator_msg_get_msgstr(GTR_MSG(messages->data));

		if (gtranslator_msg_is_fuzzy(GTR_MSG(messages->data)))
			status_color = &table->priv->fuzzy;
		else if (gtranslator_msg_is_translated(GTR_MSG(messages->data)))
			status_color = &table->priv->translated;
		else
			status_color = &table->priv->untranslated;

		gtk_list_store_append(table->priv->store, &iter);
		gtk_list_store_set(table->priv->store, &iter,
				   ID_COLUMN, g_strdup_printf("%d", id),
				   ORIGINAL_COLUMN, msgid,
				   TRANSLATION_COLUMN, msgstr,
				   COLOR_COLUMN, status_color,
				   POINTER_COLUMN, messages,
				   -1);

		path = gtk_tree_model_get_path(model, &iter);
		row = gtk_tree_row_reference_new(model, path);
		gtk_tree_path_free(path);

		gtranslator_msg_set_row_reference(GTR_MSG(messages->data), row);

		id++;
		messages = g_list_next(messages);
	}
}

void
gtranslator_message_table_update_translation (GtranslatorMessageTable *table,
					      GtranslatorMsg *msg,
					      gchar *translation) {
	GdkColor *status_color;
	GtkTreeModel *model;
	GtkTreePath *path;
	GtkTreeRowReference *row;
	GtkTreeIter iter;

	/* Set appropriate color for row */
	if (!strcmp(translation,""))
		status_color = &table->priv->untranslated;
	else
		status_color = &table->priv->translated;

	row = gtranslator_msg_get_row_reference(msg);
	model = gtk_tree_row_reference_get_model(row);
	path = gtk_tree_row_reference_get_path(row);
	gtk_tree_model_get_iter(model, &iter, path);

	gtk_list_store_set(table->priv->store, &iter,
			   TRANSLATION_COLUMN, translation,
			   COLOR_COLUMN, status_color,
			   -1);
}
