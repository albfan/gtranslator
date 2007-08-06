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

#include "about.h"
#include "compile.h"
#include "dialogs.h"
#include "message.h"
#include "notebook.h"
#include "find.h"
#include "header_stuff.h"
#include "tab.h"
#include "prefs.h"
#include "window.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#define GTR_WINDOW_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_WINDOW,     \
					 GtranslatorWindowPrivate))

static void gtranslator_utils_show_home_page(GtkWidget *widget, gpointer useless);


G_DEFINE_TYPE(GtranslatorWindow, gtranslator_window, GTK_TYPE_WINDOW)

struct _GtranslatorWindowPrivate
{
	GtkWidget *window;
	GtkWidget *main_box;
	GtkWidget *hpaned;
	
	GtkWidget *menubar;
	GtkWidget *toolbar;
	GtkActionGroup *action_group;
	
	GtkWidget *notebook;
	GList *pages;
	
	GtkWidget *statusbar;
	GtkWidget *progressbar;
	
	GtkUIManager *ui_manager;
};
	


/* Normal items */
static const GtkActionEntry entries[] = {
	{ "File", NULL, N_("_File") },
        { "Edit", NULL, N_("_Edit") },
	{ "View", NULL, N_("_View") },
	{ "Actions", NULL, N_("_Actions") },
        { "Go", NULL, N_("_Go") },
	{ "Help", NULL, N_("_Help") },

	/* File menu */
	{ "FileOpen", GTK_STOCK_OPEN, NULL, "<control>O",
	  N_("Open a po file"),
	  G_CALLBACK (gtranslator_open_file_dialog) },
	{ "FileOpenUri", NULL, N_("Open from _URI..."), NULL,
	  N_("Open a po file from a given URI"),
	  G_CALLBACK (gtranslator_open_uri_dialog) },
	{ "FileRecentFiles", NULL, N_("_Recent files"), NULL,
	  NULL, NULL },
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
	{ "FileQuitWindow", GTK_STOCK_QUIT, NULL, "<control>Q", 
	  N_("Quit the program"),
	  G_CALLBACK (gtranslator_menu_quit_cb) },
	
        /* Edit menu */
        { "EditUndo", GTK_STOCK_UNDO, NULL, "<control>Z", 
	  N_("Undo last operation"),
	  NULL },
	{ "EditRedo", GTK_STOCK_REDO, NULL, NULL,
	  N_("Redo last undo operation"),
	  NULL },
	{ "EditCut", GTK_STOCK_CUT, NULL, "<control>X",
	  N_("Cut the selected text"),
	  G_CALLBACK (gtranslator_clipboard_cut) },	
	{ "EditCopy", GTK_STOCK_COPY, NULL, "<control>C",
	  N_("Copy the selected text"),
	  G_CALLBACK (gtranslator_clipboard_copy) },
	{ "EditPaste", GTK_STOCK_PASTE, NULL, "<control>V",
	  N_("   "),
	  G_CALLBACK (gtranslator_clipboard_paste) },
	{ "EditClear", GTK_STOCK_CLEAR, NULL, NULL,
	  N_("Clear the translation text box"),
	  G_CALLBACK (gtranslator_selection_clear) },
	{ "EditFind", GTK_STOCK_FIND, NULL, "<control>F",
	  N_("   "),
	  G_CALLBACK (gtranslator_find_dialog) },
	{ "EditFindNext", GTK_STOCK_FIND, N_("Search ne_xt"), NULL,
	  N_("   "),
	  G_CALLBACK (gtranslator_find) },
	{ "EditReplace", GTK_STOCK_FIND_AND_REPLACE, NULL, "<control>H",
	  N_("   "),
	  G_CALLBACK (gtranslator_replace_dialog) },
	{ "EditHeader", NULL, N_("_Header..."), NULL,
	  N_("Edit po file header"),
	  G_CALLBACK (gtranslator_header_edit_dialog) },
	{ "EditComment", NULL, N_("C_omment..."), NULL,
	  N_("Edit message comment"),
	  G_CALLBACK (gtranslator_edit_comment_dialog) },
	{ "EditMessage2Trans", NULL, N_("Copy _message -> translation"), "<control>space",
	  N_("Copy the original message contents and paste them as translation"),
	  G_CALLBACK (gtranslator_message_copy_to_translation) },
	{ "EditFuzzy", NULL, N_("_Fuzzy"), "<control>U",
	  N_("Toggle fuzzy status of a message"),
	  G_CALLBACK (gtranslator_message_status_toggle_fuzzy) },
	{ "EditPreferences", NULL, N_("_Preferences"), NULL,
	  N_("Edit gtranslator preferences"),
	  G_CALLBACK (gtranslator_preferences_dialog_create) },
	
	/* View menu */
	{ "ViewBookmarks", NULL, N_("_Bookmarks"), NULL,
          NULL, NULL},
	{ "ViewSchemes", NULL, N_("_ColorSchemes"), NULL,
          NULL, NULL},
	
	/* Action menu */
	{ "ActionsCompile", GTK_STOCK_CONVERT, N_("_Compile"), NULL,
          N_("Compile the po file"), G_CALLBACK(compile) },
	{ "ActionsRefresh", GTK_STOCK_REFRESH, NULL, NULL,
          N_("  "), NULL },
	{ "ActionsAddBookmark", GTK_STOCK_ADD, N_("_Add bookmark"), NULL,
          N_("Add a bookmark for this message in this po file"),
	  G_CALLBACK(gtranslator_bookmark_adding_dialog) },
	{ "ActionsAutotranslate", NULL, N_("Aut_otranslate..."), NULL,
          N_("Autotranslate the file with information from your learn buffer"),
	  G_CALLBACK(gtranslator_auto_translation_dialog) },
	{ "ActionsRemoveTranslations", GTK_STOCK_REMOVE, N_("Remo_ve all translations..."), NULL,
          N_("Remove all existing translations from the po file"),
	  G_CALLBACK(gtranslator_remove_all_translations_dialog) },
	
        /* Go menu */
        { "GoFirst", GTK_STOCK_GOTO_FIRST, NULL, NULL,
          N_("Go to the first message"),
          G_CALLBACK (gtranslator_message_go_to_first) },
	{ "GoPrevious", GTK_STOCK_GO_BACK, NULL, "Page_Down",
          N_("Move back one message"),
          G_CALLBACK (gtranslator_message_go_to_previous) },
	{ "GoForward", GTK_STOCK_GO_FORWARD, NULL, "Page_Up",
          N_("Move forward one message"),
          G_CALLBACK (gtranslator_message_go_to_next) },
	{ "GoLast", GTK_STOCK_GOTO_LAST, NULL, NULL,
          N_("Go to the last message"),
          G_CALLBACK (gtranslator_message_go_to_last) },
	{ "GoJumpTo", GTK_STOCK_JUMP_TO, NULL, NULL,
          N_("Go to especified message number"),
          G_CALLBACK (gtranslator_go_to_dialog) },
	{ "GoNextFuzzy", GTK_STOCK_GO_FORWARD, N_("Next fuz_zy"),
	  "<control>Page_Up", N_("Go to the next fuzzy message"),
          G_CALLBACK (gtranslator_message_go_to_next_fuzzy) },
	{ "GoPreviousFuzzy", GTK_STOCK_GO_BACK, N_("Previous fuzz_y"),
	  "<control>Page_Down", N_("Go to the previous fuzzy message"),
          NULL },
	{ "GoNextUntranslated", GTK_STOCK_GO_FORWARD, N_("Next _untranslated"),
	  "<alt>Page_Up", N_("Go to the next untranslated message"),
          G_CALLBACK (gtranslator_message_go_to_next_untranslated) },
	{ "GoPreviousUntranslated", GTK_STOCK_GO_FORWARD, N_("Previ_ous untranslated"),
	  "<alt>Page_Down", N_("Go to the previous untranslated message"),
          NULL },
	
	

	/* Help menu */
	{ "HelpContents", GTK_STOCK_HELP, N_("_Contents"), "F1", NULL,
	  NULL },
	{ "HelpWebSite", NULL, N_("_Website"), NULL,
	  N_("gtranslator's homepage on the web"),
	  G_CALLBACK(gtranslator_utils_show_home_page) },
	{ "HelpAbout", GTK_STOCK_ABOUT, N_("_About"), NULL, NULL,
	  G_CALLBACK (gtranslator_about_dialog) },
};


/*
 * Update the progress bar
 */
static void 
gtranslator_window_update_progress_bar(GtranslatorWindow *window)
{
	gdouble percentage;
	GtranslatorTab *current_page;
	GtrPo *po;
	
	current_page = gtranslator_notebook_get_page(GTR_NOTEBOOK(window->priv->notebook));
	po = gtranslator_tab_get_po(GTR_TAB(current_page));
	
	/*
	 * Calculate the percentage.
	 */
	percentage = (gdouble)(po->translated / (gdouble)g_list_length(po->messages));
	
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

/*
 * Shows the gtranslator homepage on the web.
 */
static void
gtranslator_utils_show_home_page(GtkWidget *widget,
				 gpointer useless)
{
	gnome_url_show("http://gtranslator.sourceforge.net", NULL);
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
	 * Set the main window's geometry from the preferences.
	 */
	if (gstr == NULL)
	{
		if(GtrPreferences.save_geometry == TRUE)
		{
			x=gtranslator_config_get_int("geometry/x");
			y=gtranslator_config_get_int("geometry/y");
			width=gtranslator_config_get_int("geometry/width");
			height=gtranslator_config_get_int("geometry/height");
		}
		else
		{
			return;
		}
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

static void
gtranslator_window_set_action_sensitive (GtranslatorWindow   *window,
					 const char *name,
					 gboolean    sensitive)
{
	GtkAction *action = gtk_action_group_get_action (window->priv->action_group,
							 name);
	gtk_action_set_sensitive (action, sensitive);
}

static void
gtranslator_window_draw (GtranslatorWindow *window)
{
	GtkWidget *hbox; //Statusbar and progressbar
	GtkActionGroup *action_group;
	GtkAccelGroup *accel_group;
	GError *error = NULL;
	
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
	action_group = gtk_action_group_new ("MenuActions");
	priv->action_group = action_group;
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_actions (action_group, entries,
				      G_N_ELEMENTS (entries), window);

	priv->ui_manager = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (priv->ui_manager,
					    action_group, 0);


	if (!gtk_ui_manager_add_ui_from_file (priv->ui_manager,
					      "gtranslator-ui.xml",
					      &error)) {
		g_warning ("building menus failed: %s", error->message);
		g_error_free (error);
	}
	
	priv->menubar =	 gtk_ui_manager_get_widget (priv->ui_manager,
							   "/MainMenu");
	gtk_box_pack_start (GTK_BOX (priv->main_box),
			    priv->menubar,
			    FALSE, FALSE, 0);
	
	
	/*
	 * Toolbar
	 */
	/*priv->toolbar = GTK_WIDGET 
	  (g_object_new (EGG_TYPE_EDITABLE_TOOLBAR,
			 "ui-manager", priv->ui_manager,
			 "popup-path", "/ToolbarPopup",
			 "model", ev_application_get_toolbars_model (EV_APP),
			 NULL));

	egg_editable_toolbar_show (EGG_EDITABLE_TOOLBAR (priv->toolbar),
				   "DefaultToolBar");
	gtk_box_pack_start (GTK_BOX (priv->main_box),
			    priv->toolbar,
			    FALSE, FALSE, 0);
	gtk_widget_show (priv->toolbar);*/
	
	
	/*
	 * hpaned
	 */
	priv->hpaned = gtk_hpaned_new ();
	/*g_signal_connect (ev_window->priv->hpaned,
			  "notify::position",
			  G_CALLBACK (ev_window_sidebar_position_change_cb),
			  ev_window);*/
	
	//gtk_paned_set_position (GTK_PANED (priv->hpaned), SIDEBAR_DEFAULT_SIZE);
	gtk_box_pack_start (GTK_BOX (priv->main_box), priv->hpaned,
			    TRUE, TRUE, 0);
	gtk_widget_show (priv->hpaned);
	
	
	/*
	 * notebook
	 */
	priv->notebook = GTK_WIDGET(gtranslator_notebook_new());
	gtk_paned_pack1(GTK_PANED(priv->hpaned), priv->notebook, FALSE, FALSE);
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
	gtk_box_pack_start( GTK_BOX(hbox), priv->statusbar, TRUE, TRUE, 0);
	gtk_widget_show(priv->statusbar);
}

static void
gtranslator_window_init (GtranslatorWindow *window)
{
	window->priv = GTR_WINDOW_GET_PRIVATE (window);
	
	gtranslator_window_draw(window);
	
	gtranslator_window_restore_geometry(window, NULL);
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
	G_OBJECT_CLASS (gtranslator_window_parent_class)->dispose (object);
}

static void
gtranslator_window_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_window_parent_class)->finalize (object);
}

static void
gtranslator_window_class_init (GtranslatorWindowClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorWindowPrivate));

	object_class->finalize = gtranslator_window_finalize;
	object_class->dispose = gtranslator_window_dispose;
}

GtkWidget *
gtranslator_window_new (void)
{
	static GtkWidget *window = NULL;
	
	if(!window)
	{
		window = GTK_WIDGET (g_object_new (GTR_TYPE_WINDOW, NULL));
		gtk_widget_show_all(window);
	}
	return window;
}


GtkWidget *
gtranslator_window_get_notebook(GtranslatorWindow *window)
{
	return window->priv->notebook;
}
