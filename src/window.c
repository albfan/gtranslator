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

#include "actions.h"
#include "application.h"
#include "charmap.h"
//#include "dictionary.h"
#include "notebook.h"
#include "tab.h"
#include "panel.h"
#include "po.h"
#include "window.h"

#include "egg-toolbars-model.h"
#include "egg-toolbar-editor.h"
#include "egg-editable-toolbar.h"


#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define GTR_WINDOW_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_WINDOW,     \
					 GtranslatorWindowPrivate))

static void gtranslator_window_cmd_edit_toolbar (GtkAction *action, GtranslatorWindow *window);


G_DEFINE_TYPE(GtranslatorWindow, gtranslator_window, GTK_TYPE_WINDOW)

struct _GtranslatorWindowPrivate
{
	GtkWidget *main_box;
	GtkWidget *hpaned;
	
	GtkWidget *menubar;
	GtkWidget *toolbar;
	GtkActionGroup *always_sensitive_action_group;
	GtkActionGroup *action_group;
	
	GtkWidget *notebook;
	GtkWidget *sidebar;
	gint sidebar_size;
	
	GtkWidget *statusbar;
	gint context_id;
	GtkWidget *progressbar;
	
	GtkUIManager *ui_manager;
	GtkRecentManager *recent_manager;
	GtkWidget *recent_menu;
	
	gint            width;
        gint            height; 
	GdkWindowState  window_state;
	
	gboolean destroy_has_run : 1;
};
	

static const GtkActionEntry always_sensitive_entries[] = {
	
	{ "File", NULL, N_("_File") },
        { "Edit", NULL, N_("_Edit") },
	{ "View", NULL, N_("_View") },
	{ "Actions", NULL, N_("_Actions") },
	{ "Search", NULL, N_("_Search") },
        { "Go", NULL, N_("_Go") },
	{ "Help", NULL, N_("_Help") },
	
	/* File menu */
	{ "FileOpen", GTK_STOCK_OPEN, NULL, "<control>O",
	  N_("Open a po file"),
	  G_CALLBACK (gtranslator_open_file_dialog) },
	{ "FileOpenUri", NULL, N_("Open from _URI..."), NULL,
	  N_("Open a po file from a given URI"), NULL},
	//  G_CALLBACK (gtranslator_open_uri_dialog) },
	{ "FileRecentFiles", NULL, N_("_Recent files"), NULL,
	  NULL, NULL },
	{ "FileQuitWindow", GTK_STOCK_QUIT, NULL, "<control>Q", 
	  N_("Quit the program"),
	  G_CALLBACK (gtranslator_file_quit) },
	
	/* Edit menu */
	{ "EditToolbar", NULL, N_("T_oolbar"), NULL, NULL,
          G_CALLBACK (gtranslator_window_cmd_edit_toolbar) },
	{ "EditPreferences", GTK_STOCK_PREFERENCES, NULL, NULL,
	  N_("Edit gtranslator preferences"),
	  G_CALLBACK (gtranslator_actions_edit_preferences) },
	
	/* Help menu */
	{ "HelpContents", GTK_STOCK_HELP, N_("_Contents"), "F1", NULL,
	  NULL },
	{ "HelpWebSite", GTK_STOCK_HOME, N_("_Website"), NULL,
	  N_("gtranslator's homepage on the web"),
	  G_CALLBACK(gtranslator_window_show_home_page) },
	{ "HelpAbout", GTK_STOCK_ABOUT, N_("_About"), NULL, NULL,
	  G_CALLBACK (gtranslator_about_dialog) },
};
	

/* Normal items */
static const GtkActionEntry entries[] = {
	
	/* File menu */
       	{ "FileSave", GTK_STOCK_SAVE, NULL, "<control>S",
	  N_("Save the file"),
	  G_CALLBACK (gtranslator_save_current_file_dialog) },
	{ "FileSaveAs", GTK_STOCK_SAVE_AS, NULL, NULL,
	  N_("Save the file with another name"), 
	  G_CALLBACK (gtranslator_save_file_as_dialog) },
	/*{ "FileRevert", GTK_STOCK_REVERT_TO_SAVED, N_("_Revert"), NULL,
	  N_(),
	  G_CALLBACK (gtranslator_save_file_as_dialog) },*/
	{ "FileCloseWindow", GTK_STOCK_CLOSE, NULL, "<control>W", 
	  N_("Close the current file"),
	  G_CALLBACK (gtranslator_file_close) },
	
        /* Edit menu */
        { "EditUndo", GTK_STOCK_UNDO, NULL, "<control>Z", 
	  N_("Undo last operation"),
	  G_CALLBACK(gtranslator_actions_edit_undo) },
	{ "EditRedo", GTK_STOCK_REDO, NULL, NULL,
	  N_("Redo last undo operation"),
	  G_CALLBACK(gtranslator_actions_edit_redo) },
	{ "EditCut", GTK_STOCK_CUT, NULL, "<control>X",
	  N_("Cut the selected text"), 
	  G_CALLBACK (gtranslator_actions_edit_cut) },	
	{ "EditCopy", GTK_STOCK_COPY, NULL, "<control>C",
	  N_("Copy the selected text"),
	  G_CALLBACK (gtranslator_actions_edit_copy) },
	{ "EditPaste", GTK_STOCK_PASTE, NULL, "<control>V",
	  N_("   "),
	  G_CALLBACK (gtranslator_actions_edit_paste) },
	{ "EditClear", GTK_STOCK_CLEAR, NULL, NULL,
	  N_("Clear the translation text box"), NULL},
	//  G_CALLBACK (gtranslator_selection_clear) },
	{ "EditHeader", GTK_STOCK_PROPERTIES, N_("_Header..."), NULL,
	  N_("Edit po file header"), NULL},
	  //G_CALLBACK (gtranslator_header_edit_dialog) },
	{ "EditComment", GTK_STOCK_INDEX, N_("C_omment..."), NULL,
	  N_("Edit message comment"), NULL},
	  //G_CALLBACK (gtranslator_edit_comment_dialog) },
	{ "EditMessage2Trans", NULL, N_("Copy _message -> translation"), "<control>space",
	  N_("Copy the original message contents and paste them as translation"),
	  G_CALLBACK (gtranslator_message_copy_to_translation) },
	{ "EditFuzzy", NULL, N_("_Fuzzy"), "<control>U",
	  N_("Toggle fuzzy status of a message"),
	  G_CALLBACK (gtranslator_message_status_toggle_fuzzy) },
	
	/* View menu */
	{ "ViewBookmarks", NULL, N_("_Bookmarks"), NULL,
          NULL, NULL},
	{ "ViewSchemes", NULL, N_("_ColorSchemes"), NULL,
          NULL, NULL},
	
	/* Action menu */
	{ "ActionsCompile", GTK_STOCK_CONVERT, N_("_Compile"), NULL,
          N_("Compile the po file"), NULL },
	{ "ActionsRefresh", GTK_STOCK_REFRESH, NULL, NULL,
          N_("  "), NULL },
	{ "ActionsAddBookmark", GTK_STOCK_ADD, N_("_Add bookmark"), NULL,
          N_("Add a bookmark for this message in this po file"), NULL},
	  //G_CALLBACK(gtranslator_bookmark_adding_dialog) },
	{ "ActionsAutotranslate", NULL, N_("Aut_otranslate..."), NULL,
          N_("Autotranslate the file with information from your learn buffer"), NULL},
	  //G_CALLBACK(gtranslator_auto_translation_dialog) },
	{ "ActionsRemoveTranslations", GTK_STOCK_REMOVE, N_("Remo_ve all translations..."), NULL,
          N_("Remove all existing translations from the po file"), NULL},
	  //G_CALLBACK(gtranslator_remove_all_translations_dialog) },
	
        /* Go menu */
        { "GoFirst", GTK_STOCK_GOTO_FIRST, NULL, NULL,
          N_("Go to the first message"),
          G_CALLBACK (gtranslator_message_go_to_first) },
	{ "GoPrevious", GTK_STOCK_GO_BACK, NULL, "Page_Up",
          N_("Move back one message"),
          G_CALLBACK (gtranslator_message_go_to_previous) },
	{ "GoForward", GTK_STOCK_GO_FORWARD, NULL, "Page_Down",
          N_("Move forward one message"),
          G_CALLBACK (gtranslator_message_go_to_next) },
	{ "GoLast", GTK_STOCK_GOTO_LAST, NULL, NULL,
          N_("Go to the last message"),
          G_CALLBACK (gtranslator_message_go_to_last) },
	{ "GoJumpTo", GTK_STOCK_JUMP_TO, NULL, NULL,
          N_("Go to especified message number"), NULL},
          //G_CALLBACK (gtranslator_go_to_dialog) },
	{ "GoNextFuzzy", GTK_STOCK_GO_FORWARD, N_("Next fuz_zy"),
	  "<control>Page_Down", N_("Go to the next fuzzy message"),
          G_CALLBACK (gtranslator_message_go_to_next_fuzzy) },
	{ "GoPreviousFuzzy", GTK_STOCK_GO_BACK, N_("Previous fuzz_y"),
	  "<control>Page_Up", N_("Go to the previous fuzzy message"),
          G_CALLBACK (gtranslator_message_go_to_prev_fuzzy) },
	{ "GoNextUntranslated", GTK_STOCK_GO_FORWARD, N_("Next _untranslated"),
	  "<alt>Page_Down", N_("Go to the next untranslated message"),
          G_CALLBACK (gtranslator_message_go_to_next_untranslated) },
	{ "GoPreviousUntranslated", GTK_STOCK_GO_BACK, N_("Previ_ous untranslated"),
	  "<alt>Page_Up", N_("Go to the previous untranslated message"),
          G_CALLBACK (gtranslator_message_go_to_prev_untranslated) },

	/* Search menu*/
	{ "SearchFind", GTK_STOCK_FIND, NULL, "<control>F",
	  N_("   "), NULL},
	//  G_CALLBACK (gtranslator_find_dialog) },
	{ "SearchFindNext", NULL, N_("Search ne_xt"), NULL,
	  N_("   "), NULL},
	 // G_CALLBACK (gtranslator_find) },
	{ "SearchFindPrevious", NULL, N_("Search _previous"), NULL,
	  N_("   "), NULL},
	 // G_CALLBACK (gtranslator_find) },
	{ "SearchReplace", GTK_STOCK_FIND_AND_REPLACE, NULL, NULL,
	  N_("   "), NULL},
	 // G_CALLBACK (gtranslator_replace_dialog) },
	
};


void
set_sensitive_according_to_message(GtranslatorWindow *window,
				   GtranslatorPo *po)
{
	GList *current;
	GtkAction *action;
		
	current = gtranslator_po_get_current_message(po);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoForward");
	gtk_action_set_sensitive (action, 
				  g_list_next(current) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoPrevious");
	gtk_action_set_sensitive (action, 
				  g_list_previous(current) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoFirst");
	gtk_action_set_sensitive (action, 
				  g_list_first(current) != current);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoLast");
	gtk_action_set_sensitive (action, 
				  g_list_last(current) != current);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoNextFuzzy");
	gtk_action_set_sensitive (action, 
				  gtranslator_po_get_next_fuzzy(po) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoPreviousFuzzy");
	gtk_action_set_sensitive (action, 
				  gtranslator_po_get_prev_fuzzy(po) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoNextUntranslated");
	gtk_action_set_sensitive (action, 
				  gtranslator_po_get_next_untrans(po) != NULL);
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "GoPreviousUntranslated");
	gtk_action_set_sensitive (action, 
				  gtranslator_po_get_prev_untrans(po) != NULL);
}

static void
set_sensitive_according_to_tab(GtranslatorWindow *window,
			       GtranslatorTab *tab)
{
	GtranslatorView *view;
	GtranslatorPo *po;
	GtkSourceBuffer *buf;
	GtkAction *action;
	GList *current;
	
	view = gtranslator_tab_get_active_view(tab);
	po = gtranslator_tab_get_po(tab);
	current = gtranslator_po_get_current_message(po);
	buf = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)));
	
	if(gtk_action_group_get_sensitive(window->priv->action_group) == FALSE)
		gtk_action_group_set_sensitive(window->priv->action_group, TRUE);
	
	/*Edit*/
	action = gtk_action_group_get_action(window->priv->action_group,
					     "EditUndo");
	gtk_action_set_sensitive (action, 
				  gtk_source_buffer_can_undo (buf));
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "EditRedo");
	gtk_action_set_sensitive (action, 
				  gtk_source_buffer_can_redo (buf));
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "EditCut");
	gtk_action_set_sensitive (action, 
				  gtk_text_buffer_get_has_selection (GTK_TEXT_BUFFER (buf)));
	
	action = gtk_action_group_get_action(window->priv->action_group,
					     "EditCopy");
	gtk_action_set_sensitive (action, 
				  gtk_text_buffer_get_has_selection (GTK_TEXT_BUFFER (buf)));
	
	/*Go*/
	set_sensitive_according_to_message(window, po);
}

void
set_sensitive_according_to_window(GtranslatorWindow *window)
{
	gint pages;
	
	pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(window->priv->notebook));
	
	gtk_action_group_set_sensitive(window->priv->action_group,
				       pages > 0);
				       
}

static void
notebook_switch_page(GtkNotebook *nb,
		     GtkNotebookPage *page,
		     gint page_num,
		     GtranslatorWindow *window)
{
	GtranslatorTab *current_tab;
	
	current_tab = gtranslator_window_get_active_tab(window);
	
	set_sensitive_according_to_tab(window, current_tab);
	
	gtranslator_window_update_statusbar(window);
}

static void
can_undo(GtkSourceBuffer *doc,
	 GParamSpec *pspec,
	 GtranslatorWindow *window)
{
	GtkAction *action;
	gboolean sensitive;
	GtranslatorView *view;
	GtkSourceBuffer *buf;

	view = gtranslator_window_get_active_view (window);
	buf = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)));
	
	sensitive = gtk_source_buffer_can_undo(buf);
	
	if (doc != buf)
		return;

	action = gtk_action_group_get_action (window->priv->action_group,
					     "EditUndo");
	gtk_action_set_sensitive (action, sensitive);
}

static void
can_redo(GtkSourceBuffer *doc,
	 GParamSpec *spec,
	 GtranslatorWindow *window)
{
	GtkAction *action;
	gboolean sensitive;
	GtranslatorView *view;
	GtkSourceBuffer *buf;

	view = gtranslator_window_get_active_view (window);
	buf = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)));
	
	sensitive = gtk_source_buffer_can_redo(buf);
	
	if (doc != buf)
		return;

	action = gtk_action_group_get_action (window->priv->action_group,
					     "EditRedo");
	gtk_action_set_sensitive (action, sensitive);
}

static void
notebook_tab_added(GtkNotebook *notebook,
		   GtkWidget   *child,
		   guint        page_num,
		   GtranslatorWindow *window)
{
	GtranslatorView *view;
	GtranslatorTab *tab = GTR_TAB(child);
	GtkTextBuffer *buffer;
	
	g_return_if_fail(GTR_IS_TAB(tab));
	
	view = gtranslator_tab_get_active_view(tab);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
	
	
	g_signal_connect(GTK_SOURCE_BUFFER(buffer),
			 "notify::can-undo",
			 G_CALLBACK(can_undo),
			 window);
	
	g_signal_connect(GTK_SOURCE_BUFFER(buffer),
			 "notify::can-redo",
			 G_CALLBACK(can_redo),
			 window);
}


/*
 * Restore the geometry.
 */
static void
gtranslator_window_restore_geometry(GtranslatorWindow *window,
				    gchar* gstr)
{
	gint x=0, y=0, width=0, height=0;

	/*
	 * Set the main application's geometry from the preferences.
	 */
	if (gstr == NULL)
	{
		/*if(GtrPreferences.save_geometry == TRUE)
		{
			x=gtranslator_config_get_int("geometry/x");
			y=gtranslator_config_get_int("geometry/y");
			width=gtranslator_config_get_int("geometry/width");
			height=gtranslator_config_get_int("geometry/height");
		}
		else
		{
			return;
		}*/
	}
	/*
	 * If a geometry definition had been defined try to parse it.
	 */
	else
	{
		if(!gtk_window_parse_geometry(GTK_WINDOW(window), gstr))
		{
			g_warning(_("The geometry string \"%s\" couldn't be parsed!"), gstr);
			return;
		}
	}
	if (x != -1)
		gtk_window_move(GTK_WINDOW(window), x, y);
	if ((width > 0) && (height > 0))
		gtk_window_resize(GTK_WINDOW(window), width, height);
}

void
gtranslator_recent_add (GtranslatorWindow *window,
			const gchar *path)
{
	GtkRecentData *recent_data;
	gchar *uri;
	GError *error = NULL;

	uri = g_filename_to_uri (path, NULL, &error);
	if (error)
	{	
		g_warning ("Could not convert uri \"%s\" to a local path: %s",
			   uri, error->message);
		g_error_free (error);
		return;
	}

	recent_data = g_slice_new (GtkRecentData);

	recent_data->display_name   = NULL;
	recent_data->description    = NULL;
	recent_data->mime_type      = "text/x-gettext-translation";
	recent_data->app_name       = (gchar *) g_get_application_name ();
	recent_data->app_exec       = g_strjoin (" ", g_get_prgname (), "%u", NULL);
	recent_data->groups         = NULL;
	recent_data->is_private     = FALSE;

	if (!gtk_recent_manager_add_full (window->priv->recent_manager,
				          uri,
				          recent_data))
	{
      		g_warning ("Unable to add '%s' to the list of recently used documents", uri);
	}

	g_free (uri);
	g_free (recent_data->app_exec);
	g_slice_free (GtkRecentData, recent_data);

}

void
gtranslator_recent_remove (GtranslatorWindow *window,
			   const gchar *path)
{
	gchar *uri;
	GError *error = NULL;

	uri = g_filename_to_uri (path, NULL, &error);
	if (error)
	{	
		g_warning ("Could not convert uri \"%s\" to a local path: %s",
			   uri, error->message);
		g_error_free (error);
		return;
	}
	
	gtk_recent_manager_remove_item (window->priv->recent_manager, uri, &error);
	if (error)
	{
		g_warning ("Could not remove recent-files uri \"%s\": %s",
			   uri, error->message);
		g_error_free (error);
	}
	
	g_free (uri);
}

static void
gtranslator_recent_chooser_item_activated_cb (GtkRecentChooser *chooser,
					      GtranslatorWindow *window)
{
	gchar *uri, *path;
	GError *error = NULL;

	uri = gtk_recent_chooser_get_current_uri (chooser);

	path = g_filename_from_uri (uri, NULL, NULL);
	if (error)
	{
		g_warning ("Could not convert uri \"%s\" to a local path: %s",
			   uri, error->message);
		g_error_free (error);
		return;
	}
	
	
	gtranslator_open (path, window, &error);
	if(error)
	{
		gtranslator_recent_remove (window, path);
	}

	g_free (uri);
	g_free (path);
}

static GtkWidget *
create_recent_chooser_menu (GtranslatorWindow *window,
			    GtkRecentManager *manager)
{
	GtkWidget *recent_menu;
	GtkRecentFilter *filter;

	recent_menu = gtk_recent_chooser_menu_new_for_manager (manager);

	gtk_recent_chooser_set_local_only (GTK_RECENT_CHOOSER (recent_menu), TRUE);
	gtk_recent_chooser_set_show_icons (GTK_RECENT_CHOOSER (recent_menu), FALSE);
	gtk_recent_chooser_set_sort_type (GTK_RECENT_CHOOSER (recent_menu), GTK_RECENT_SORT_MRU);
	gtk_recent_chooser_menu_set_show_numbers (GTK_RECENT_CHOOSER_MENU (recent_menu), TRUE);

	filter = gtk_recent_filter_new ();
	gtk_recent_filter_add_application (filter, g_get_application_name());
	gtk_recent_chooser_set_filter (GTK_RECENT_CHOOSER (recent_menu), filter);

	return recent_menu;
}

static void
gtranslator_window_set_action_sensitive (GtranslatorWindow *window,
					 const gchar *name,
					 gboolean sensitive)
{
	GtkAction *action = gtk_action_group_get_action (window->priv->action_group,
							 name);
	gtk_action_set_sensitive (action, sensitive);
}

static void
gtranslator_window_cmd_edit_toolbar_cb (GtkDialog *dialog,
					gint response,
					gpointer data)
{
	GtranslatorWindow *window = GTR_WINDOW (data);
        egg_editable_toolbar_set_edit_mode
			(EGG_EDITABLE_TOOLBAR (window->priv->toolbar), FALSE);
	gtranslator_application_save_toolbars_model (GTR_APP);
        gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
gtranslator_window_cmd_edit_toolbar (GtkAction *action,
				     GtranslatorWindow *window)
{
	GtkWidget *dialog;
	GtkWidget *editor;

	dialog = gtk_dialog_new_with_buttons (_("Toolbar Editor"),
					      GTK_WINDOW (window), 
				              GTK_DIALOG_DESTROY_WITH_PARENT, 
					      GTK_STOCK_CLOSE,
					      GTK_RESPONSE_CLOSE, 
					      NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_CLOSE);
	gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dialog)), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dialog)->vbox), 2);
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
	gtk_window_set_default_size (GTK_WINDOW (dialog), 500, 400);
	  
	editor = egg_toolbar_editor_new (window->priv->ui_manager,
					 gtranslator_application_get_toolbars_model (GTR_APP));
	gtk_container_set_border_width (GTK_CONTAINER (editor), 5);
	gtk_box_set_spacing (GTK_BOX (EGG_TOOLBAR_EDITOR (editor)), 5);
             
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), editor);

	egg_editable_toolbar_set_edit_mode
		(EGG_EDITABLE_TOOLBAR (window->priv->toolbar), TRUE);

	g_signal_connect (dialog, "response",
			  G_CALLBACK (gtranslator_window_cmd_edit_toolbar_cb),
			  window);
	gtk_widget_show_all (dialog);
}

static void
window_sidebar_position_change_cb(GObject    *gobject,
				  GParamSpec *arg1,
				  GtranslatorWindow *window)
{
	window->priv->sidebar_size = gtk_paned_get_position(GTK_PANED(gobject));
}

static void
gtranslator_window_draw (GtranslatorWindow *window)
{
	GtkWidget *hbox; //Statusbar and progressbar
	GtkWidget *widget;
	GError *error = NULL;
	gint table_pane_position;
	
	GtranslatorWindowPrivate *priv = window->priv;
	
	/*
	 * Main box
	 */
	priv->main_box = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), priv->main_box);
	gtk_widget_show (priv->main_box);
	
	/*
	 * Menus
	 */
	priv->ui_manager = gtk_ui_manager_new ();
	
	gtk_window_add_accel_group(GTK_WINDOW(window), 
				   gtk_ui_manager_get_accel_group(priv->ui_manager));
	
	priv->always_sensitive_action_group = gtk_action_group_new ("AlwaysSensitiveMenuActions");
	gtk_action_group_set_translation_domain(priv->always_sensitive_action_group, NULL);
	gtk_action_group_add_actions (priv->always_sensitive_action_group,
				      always_sensitive_entries,
				      G_N_ELEMENTS(always_sensitive_entries),
				      window);
	
	gtk_ui_manager_insert_action_group (priv->ui_manager,
					    priv->always_sensitive_action_group, 0);
	
	priv->action_group = gtk_action_group_new ("MenuActions");
	gtk_action_group_set_translation_domain (priv->action_group, NULL);
	gtk_action_group_add_actions (priv->action_group, entries,
				      G_N_ELEMENTS (entries), window);

	gtk_ui_manager_insert_action_group (priv->ui_manager,
					    priv->action_group, 0);


	if (!gtk_ui_manager_add_ui_from_file (priv->ui_manager,
					      DATADIR"/gtranslator-ui.xml",
					      &error)) {
		g_warning ("building menus failed: %s", error->message);
		g_error_free (error);
	}
	
	priv->menubar =	 gtk_ui_manager_get_widget (priv->ui_manager,
							   "/MainMenu");
	gtk_box_pack_start (GTK_BOX (priv->main_box),
			    priv->menubar,
			    FALSE, FALSE, 0);
	
	/* recent files */	
	priv->recent_manager = gtk_recent_manager_get_default();

	priv->recent_menu = create_recent_chooser_menu (window, priv->recent_manager);

	g_signal_connect (priv->recent_menu,
			  "item-activated",
			  G_CALLBACK (gtranslator_recent_chooser_item_activated_cb),
			  window);
			  
	widget = gtk_ui_manager_get_widget (priv->ui_manager,
					    "/MainMenu/FileMenu/FileRecentFilesMenu");
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (widget), priv->recent_menu);
	

	/*
	 * Toolbar
	 */
	priv->toolbar = GTK_WIDGET 
	  (g_object_new (EGG_TYPE_EDITABLE_TOOLBAR,
			 "ui-manager", priv->ui_manager,
			 "model", gtranslator_application_get_toolbars_model(GTR_APP),
			 NULL));

	egg_editable_toolbar_show (EGG_EDITABLE_TOOLBAR (priv->toolbar),
				   "DefaultToolBar");
	gtk_box_pack_start (GTK_BOX (priv->main_box),
			    priv->toolbar,
			    FALSE, FALSE, 0);
	gtk_widget_show (priv->toolbar);
	
	
	/*
	 * hpaned
	 */
	priv->hpaned = gtk_hpaned_new ();
	g_signal_connect (priv->hpaned,
			  "notify::position",
			  G_CALLBACK (window_sidebar_position_change_cb),
			  window);
	
	gtk_paned_set_position (GTK_PANED (priv->hpaned),
				gtranslator_prefs_manager_get_side_panel_size());
	
	gtk_box_pack_start (GTK_BOX (priv->main_box), priv->hpaned,
			    TRUE, TRUE, 0);
	gtk_widget_show (priv->hpaned);
	
	
	/*
	 * sidebar
	 */
	priv->sidebar = gtranslator_panel_new(GTK_ORIENTATION_VERTICAL);
	gtk_paned_pack1(GTK_PANED(priv->hpaned), priv->sidebar, FALSE, FALSE);
	gtk_widget_show(priv->sidebar);
	
	/*
	 * notebook
	 */
	priv->notebook = GTK_WIDGET(gtranslator_notebook_new());
	g_signal_connect(priv->notebook, "switch-page",
			 G_CALLBACK(notebook_switch_page), window);
	g_signal_connect(priv->notebook, "page-added",
			 G_CALLBACK(notebook_tab_added), window);
	
	gtk_paned_pack2(GTK_PANED(priv->hpaned), priv->notebook, FALSE, FALSE);
	gtk_widget_show(priv->notebook);


	/*
	 * hbox
	 */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start( GTK_BOX(priv->main_box), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);
	
	/*
	 * progressbar
	 */
	priv->progressbar = gtk_progress_bar_new();
	gtk_box_pack_start( GTK_BOX(hbox), priv->progressbar, FALSE, FALSE, 0);
	gtk_widget_show(priv->progressbar);
			
	/*
	 * statusbar
	 */
	priv->statusbar = gtk_statusbar_new();
	priv->context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(priv->statusbar),
							"status_message");
	gtk_box_pack_start( GTK_BOX(hbox), priv->statusbar, TRUE, TRUE, 0);
	gtk_widget_show(priv->statusbar);
}

static void
gtranslator_window_init (GtranslatorWindow *window)
{
	window->priv = GTR_WINDOW_GET_PRIVATE (window);
	
	window->priv->destroy_has_run = FALSE;
	
	gtranslator_window_draw(window);
	
	gtranslator_window_restore_geometry(window, NULL);
	
	set_sensitive_according_to_window(window);
	
	/* Charmap panel */
	impl_activate(window);
	
	/* Dictionary panel */
	//dictionary_activate(window);
	
}

static void
gtranslator_window_dispose (GObject *object)
{
	GtranslatorWindow *window = GTR_WINDOW(object);
	GtranslatorWindowPrivate *priv = window->priv;
	
	if(priv->ui_manager)
	{
		g_object_unref(priv->ui_manager);
		priv->ui_manager = NULL;
	}
	if(priv->action_group)
	{
		g_object_unref(priv->action_group);
		priv->action_group = NULL;
	}
	
	/* Deactivate Panels */
	impl_deactivate(window);
//dictionary_deactivate(window);
	
	G_OBJECT_CLASS (gtranslator_window_parent_class)->dispose (object);
}

static void
gtranslator_window_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_window_parent_class)->finalize (object);
}

static void
save_panes_state(GtranslatorWindow *window)
{
	gint pane_page;

        if (gtranslator_prefs_manager_window_size_can_set ())
        	gtranslator_prefs_manager_set_window_size (window->priv->width,
							   window->priv->height);

        if (gtranslator_prefs_manager_window_state_can_set ())
		gtranslator_prefs_manager_set_window_state (window->priv->window_state);

        if ((window->priv->sidebar_size > 0) &&
	    gtranslator_prefs_manager_side_panel_size_can_set ())
		gtranslator_prefs_manager_set_side_panel_size (
                                        window->priv->sidebar_size);

	pane_page = _gtranslator_panel_get_active_item_id (GTR_PANEL (window->priv->sidebar));
	if (pane_page != 0 &&
	    gtranslator_prefs_manager_side_panel_active_page_can_set ())
		gtranslator_prefs_manager_set_side_panel_active_page (pane_page);
}

static void
gtranslator_window_destroy (GtkObject *object)
{
	g_warning("destroy window");
        GtranslatorWindow *window;

        window = GTR_WINDOW (object);

        if (!window->priv->destroy_has_run)
        {
                save_panes_state (window);
                window->priv->destroy_has_run = TRUE;
        }

        GTK_OBJECT_CLASS (gtranslator_window_parent_class)->destroy (object);
}

static gboolean 
gtranslator_window_configure_event (GtkWidget         *widget,
				    GdkEventConfigure *event)
{
        GtranslatorWindow *window = GTR_WINDOW (widget);

        window->priv->width = event->width;
        window->priv->height = event->height;
	
        return GTK_WIDGET_CLASS (gtranslator_window_parent_class)->configure_event (widget, event);
}

static void
gtranslator_window_class_init (GtranslatorWindowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkObjectClass *gobject_class = GTK_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	
	g_type_class_add_private (klass, sizeof (GtranslatorWindowPrivate));

	object_class->finalize = gtranslator_window_finalize;
	object_class->dispose = gtranslator_window_dispose;
	
	gobject_class->destroy = gtranslator_window_destroy;
	
	widget_class->configure_event = gtranslator_window_configure_event;
}

/***************************** Public funcs ***********************************/

GtranslatorTab *
gtranslator_window_create_tab(GtranslatorWindow *window,
			      GtranslatorPo *po)
{
	GtranslatorTab *tab;
	
	tab = gtranslator_tab_new(po);
	gtk_widget_show(GTK_WIDGET(tab));
	
	gtranslator_notebook_add_page(GTR_NOTEBOOK(window->priv->notebook),
				      tab);
	return tab;
}

GtranslatorTab *
gtranslator_window_get_active_tab(GtranslatorWindow *window)
{
	return gtranslator_notebook_get_page(GTR_NOTEBOOK(window->priv->notebook));
}

GtranslatorNotebook *
gtranslator_window_get_notebook(GtranslatorWindow *window)
{
	return GTR_NOTEBOOK(window->priv->notebook);
}


GtranslatorPanel *
gtranslator_window_get_side_panel(GtranslatorWindow *window)
{
	return GTR_PANEL(window->priv->sidebar);
}

GtkStatusbar *
gtranslator_window_get_statusbar(GtranslatorWindow *window)
{
	return GTK_STATUSBAR(window->priv->statusbar);
}

GtkUIManager *
gtranslator_window_get_ui_manager(GtranslatorWindow *window)
{
	return window->priv->ui_manager;
}

GtkWidget *
gtranslator_window_get_paned(GtranslatorWindow *window)
{
	return window->priv->hpaned;
}

/**
 * gtranslator_window_get_active_view:
 * @window: a #GtranslationWindow
 *
 * Return value: the active translation view in the #GtranslationWindow
 **/
GtranslatorView *
gtranslator_window_get_active_view(GtranslatorWindow *window)
{
	GtranslatorTab *current_tab;
	current_tab = gtranslator_window_get_active_tab(window);
	
	g_return_if_fail(current_tab != NULL);
	
	return gtranslator_tab_get_active_view(current_tab);
}


/**
 * gtranslator_window_get_all_views:
 * @window: the #GtranslationWindow
 * @all_views: TRUE if you want original TextViews too.
 *
 * Returns all the views currently present in #GtranslationWindow
 *
 * Return value: a newly allocated list of #GtranslationWindow objects
 **/
GList *
gtranslator_window_get_all_views(GtranslatorWindow *window,
				 gboolean all_views)
{
	gint numtabs;
	gint i;
	GList *views = NULL;
	GtkWidget *tab;
	
	g_return_val_if_fail(GTR_IS_WINDOW(window), NULL);
	
	numtabs = gtk_notebook_get_n_pages(GTK_NOTEBOOK(window->priv->notebook));
	i = numtabs - 1;
	
	while(i >= 0 && numtabs != 0)
	{
		tab = gtk_notebook_get_nth_page(GTK_NOTEBOOK(window->priv->notebook),
						i);
		views = g_list_concat(views, gtranslator_tab_get_all_views(GTR_TAB(tab), all_views));
		i--;
	}
	
	return views;
}

void
gtranslator_window_update_statusbar(GtranslatorWindow *window)
{
	GtranslatorTab *tab;
	GtranslatorPo *po;
	gchar *msg;
	gint pos, message_count, fuzzy, untranslated;
	GList *current_msg;
	
	tab = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(tab);
	current_msg = gtranslator_po_get_current_message(po);
	
	message_count = gtranslator_po_get_messages_count(po);
	pos = gtranslator_po_get_message_position(po);
	fuzzy = gtranslator_po_get_fuzzy_count(po);
	untranslated = gtranslator_po_get_untranslated_count(po);
	
	msg = g_strdup_printf(_("    Current: %d    Total: %d    Fuzzies: %d    Untranslated: %d"), 
			      pos+1, message_count, fuzzy, untranslated);
	
	gtk_statusbar_pop(GTK_STATUSBAR(window->priv->statusbar),
			  window->priv->context_id);
	
	gtk_statusbar_push(GTK_STATUSBAR(window->priv->statusbar),
			   window->priv->context_id,
			   msg);
	g_free(msg);			 
}

/*
 * Update the progress bar
 */
void 
gtranslator_window_update_progress_bar(GtranslatorWindow *window)
{
	gdouble percentage;
	GtranslatorTab *current_page;
	GtranslatorPo *po;
	
	current_page = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current_page);
	
	/*
	 * Calculate the percentage.
	 */
	percentage = (gdouble)(gtranslator_po_get_translated_count(po)
			       / (gdouble)gtranslator_po_get_messages_count(po));
	
	/*
	 * Set the progress only if the values are reasonable.
	 */
	if(percentage > 0.0 || percentage < 1.0)
	{
		/*
		 * Set the progressbar status.
		 */
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(window->priv->progressbar),
					      percentage);
	}
}
