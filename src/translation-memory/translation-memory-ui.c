/*
 * Copyright (C) 2008
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
#include "po.h"
#include "prefs-manager.h"
#include "translation-memory.h"
#include "translation-memory-ui.h"
#include "tab.h"
#include "window.h"

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#define GTR_TRANSLATION_MEMORY_UI_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
							(object),	               \
							GTR_TYPE_TRANSLATION_MEMORY_UI,	       \
							GtranslatorTranslationMemoryUiPrivate))

G_DEFINE_TYPE(GtranslatorTranslationMemoryUi, gtranslator_translation_memory_ui, GTK_TYPE_SCROLLED_WINDOW)

struct _GtranslatorTranslationMemoryUiPrivate
{
        GtkWidget *tree_view;
	GtranslatorTab *tab;
};

enum {
  SHORTCUT_COLUMN,
  LEVEL_COLUMN,
  STRING_COLUMN,
  N_COLUMNS
};

static void
tree_view_size_cb (GtkWidget     *widget,
		   GtkAllocation *allocation,
		   gpointer user_data);

static void                
on_activate_item_cb (GtkMenuItem *menuitem,
		     GtranslatorWindow *window)
{
 
  GtkWidget *label;
  GtranslatorView *view;
  GtkTextBuffer *buffer;
  GtranslatorTab *tab;
  GtranslatorPo *po;
  GList *current_msg = NULL;
  GtranslatorMsg *msg;
  const gchar *name;
  gchar **array;
  gint index;
  gchar **data;

  label = gtk_bin_get_child (GTK_BIN (menuitem));
  name = gtk_label_get_text (GTK_LABEL (label));
  
  view = gtranslator_window_get_active_view (window);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

  array = g_strsplit (name, "nº ", 2);

  index = atoi (array[1]);

  tab = gtranslator_window_get_active_tab (window);
  po = gtranslator_tab_get_po (tab);
  current_msg = gtranslator_po_get_current_message (po);

  msg = (GtranslatorMsg *)current_msg->data;

  data = gtranslator_msg_get_tm_list (msg);
  
  gtranslator_msg_set_msgstr (msg, data[index-1]);

  gtk_text_buffer_set_text (buffer,
			    data[index-1],
			    strlen (data[index-1]));

  gtranslator_po_set_state (po, GTR_PO_STATE_MODIFIED);
}

static void
showed_message_cb (GtranslatorTab *tab,
		   GtranslatorMsg *msg,
		   GtranslatorTranslationMemoryUi *tm_ui)
{
  GtranslatorTranslationMemory *tm;
  GtkListStore *model;
  GtkTreeIter iter;
  GtkTreeViewColumn *level_column;
  const gchar *msgid;
  gint i = 1;
  gint j = 1;
  gint k = 0;
  GList *tm_list = NULL;
  GList *l = NULL;
  GList *renderers_list = NULL;
  GtkWidget *tm_item;
  GtkWidget *tm_menu;
  GtkWidget *items_menu;
  GtranslatorWindow *window;
  GtkUIManager *manager;

  model = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (tm_ui->priv->tree_view)));
  
  window = gtranslator_application_get_active_window (GTR_APP);
  tm_menu = gtranslator_window_get_tm_menu (window);

  if (!gtranslator_prefs_manager_get_show_tm_options () && gtranslator_msg_is_translated (msg)
      && !gtranslator_msg_is_fuzzy (msg)) {
    gtk_widget_set_sensitive (tm_menu, FALSE);
    gtk_list_store_clear (model);
    return;    
  }else {  
    g_signal_connect (tm_ui->priv->tree_view,
		      "size_allocate",
		      G_CALLBACK (tree_view_size_cb),
		      tm_ui->priv->tree_view);
    
    msgid = gtranslator_msg_get_msgid (msg);
    
    tm = (GtranslatorTranslationMemory *)gtranslator_application_get_translation_memory (GTR_APP);
    
    tm_list = gtranslator_translation_memory_lookup (tm, msgid);
    if (tm_list == NULL) {
      gtk_widget_set_sensitive (tm_menu, FALSE);
    } else {
      gtk_widget_set_sensitive (tm_menu, TRUE);
    }
    
    gtk_list_store_clear (model);
    
    gchar *item_name;
    gchar **data;
    
    data = (gchar **)malloc ((size_t) g_list_length (tm_list)*sizeof(gchar*));
    
    for (l = tm_list; l; l = l->next) {
      GtranslatorTranslationMemoryMatch *match;
      match = (GtranslatorTranslationMemoryMatch *)l->data;
      data[i-1] = match->match; 
      
      level_column = gtk_tree_view_get_column (GTK_TREE_VIEW (tm_ui->priv->tree_view), 0);
      renderers_list = gtk_tree_view_column_get_cell_renderers (level_column);
      
      g_object_set (renderers_list->data,
		    "accel-mods", GDK_CONTROL_MASK,
		    NULL);
      
      gtk_list_store_append (model, &iter);
      gtk_list_store_set (model,
			  &iter,
			  SHORTCUT_COLUMN,
			  GDK_0+k,
			  STRING_COLUMN,
			  match->match,
			  LEVEL_COLUMN,
			  match->level,
			  -1);
      i++;
      k++;
      if (k>9)
	break;
    }
    
    gtranslator_msg_set_tm_list (msg, data);
    
    /* MenuBar stuff */
    
    items_menu = gtk_menu_new();
    
    manager = gtranslator_window_get_ui_manager (window);
    
    gtk_menu_set_accel_group (GTK_MENU (items_menu),
			      gtk_ui_manager_get_accel_group(manager));
    
    do{
      gchar *accel_path;
      
      item_name = g_strdup_printf (_("Insert Option nº %d"), j);
      
      tm_item = gtk_menu_item_new_with_label (item_name);
      gtk_widget_show (tm_item);
      
      accel_path = g_strdup_printf ("<Gtranslator-sheet>/Edit/_Insert Tags/%s", item_name);
      
      gtk_menu_item_set_accel_path (GTK_MENU_ITEM (tm_item), accel_path);
      gtk_accel_map_add_entry (accel_path, GDK_0+(j-1), GDK_CONTROL_MASK);
      
      g_free (accel_path);
      g_free (item_name);
      
      g_signal_connect (tm_item, "activate",
			G_CALLBACK (on_activate_item_cb), window);
      
      gtk_menu_shell_append (GTK_MENU_SHELL (items_menu), tm_item);
    
      j++;
      if (j>10)
	break;

    }while (tm_list = g_list_next (tm_list));
    
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (tm_menu), items_menu);
  }
}

static void
tree_view_size_cb (GtkWidget     *widget,
		   GtkAllocation *allocation,
		   gpointer       user_data)
{
        GtkTreeView *treeview;
	GtkTreeViewColumn *column;
	GList *renderers_list = NULL;
	gint size;

	treeview = GTK_TREE_VIEW (user_data);	
     
	column = gtk_tree_view_get_column (treeview, 2);
	renderers_list = gtk_tree_view_column_get_cell_renderers (column);

	size = gtk_tree_view_column_get_width (column);

	g_object_set (renderers_list->data,
		      "wrap-width", size-10,
		      NULL);
}


static void
gtranslator_translation_memory_ui_draw(GtranslatorTranslationMemoryUi *tm_ui)
{
	GtranslatorTranslationMemoryUiPrivate *priv = tm_ui->priv;
	GtkListStore *model;
	GtkCellRenderer *level_renderer, *string_renderer, *shortcut_renderer;
	GtkTreeViewColumn *shortcut, *string, *level;

	priv->tree_view = gtk_tree_view_new();
	gtk_widget_show (priv->tree_view);
	
	gtk_container_add (GTK_CONTAINER(tm_ui),
			   priv->tree_view);
	
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(tm_ui),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	model = gtk_list_store_new (N_COLUMNS, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (priv->tree_view), GTK_TREE_MODEL (model));

	shortcut_renderer = gtk_cell_renderer_accel_new ();
	shortcut = gtk_tree_view_column_new_with_attributes ("Shortcut",
							     shortcut_renderer,
							     "accel-key", SHORTCUT_COLUMN,
							     NULL);
	g_object_set (shortcut_renderer,
		      "width", 80,
		      NULL);
	
	gtk_tree_view_append_column (GTK_TREE_VIEW (priv->tree_view), shortcut);

	level_renderer = gtk_cell_renderer_progress_new ();
	level = gtk_tree_view_column_new_with_attributes ("Level",
							  level_renderer,
							  "value", LEVEL_COLUMN,
							  NULL);
	g_object_set (level_renderer,
		      "width", 80,
		      NULL);
	
	gtk_tree_view_append_column (GTK_TREE_VIEW (priv->tree_view), level);
	
	string_renderer = gtk_cell_renderer_text_new ();
	string = gtk_tree_view_column_new_with_attributes ("String",
							   string_renderer,
							   "text", STRING_COLUMN,
							   NULL);
	gtk_tree_view_column_set_sizing (string,
					 GTK_TREE_VIEW_COLUMN_FIXED);

	g_object_set (string_renderer,
		      "ypad", 0,
		      "xpad", 5,
		      "yalign", 0.0,
		      "wrap-mode", PANGO_WRAP_WORD_CHAR,
		      NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW (priv->tree_view), string);
}

static void
gtranslator_translation_memory_ui_init (GtranslatorTranslationMemoryUi *tm_ui)
{
	tm_ui->priv = GTR_TRANSLATION_MEMORY_UI_GET_PRIVATE (tm_ui);
	
	gtranslator_translation_memory_ui_draw (tm_ui);
}

static void
gtranslator_translation_memory_ui_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_translation_memory_ui_parent_class)->finalize (object);
}

static void
gtranslator_translation_memory_ui_class_init (GtranslatorTranslationMemoryUiClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorTranslationMemoryUiPrivate));

	object_class->finalize = gtranslator_translation_memory_ui_finalize;
}

GtkWidget *
gtranslator_translation_memory_ui_new (GtkWidget *tab)
{
	GtranslatorTranslationMemoryUi *tm_ui;
	tm_ui = g_object_new (GTR_TYPE_TRANSLATION_MEMORY_UI, NULL);
	
	tm_ui->priv->tab = GTR_TAB(tab);
	g_signal_connect(tab,
			 "showed-message",
			 G_CALLBACK(showed_message_cb),
			 tm_ui);
	
	return GTK_WIDGET(tm_ui);
}