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

#include "preferences-dialog.h"
#include "prefs-manager.h"
#include "utils_gui.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>


#define GTR_PREFERENCES_DIALOG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_PREFERENCES_DIALOG,     \
						 	GtranslatorPreferencesDialogPrivate))


G_DEFINE_TYPE(GtranslatorPreferencesDialog, gtranslator_preferences_dialog, GTK_TYPE_DIALOG)

typedef struct _Parents Parents;
struct _Parents
{
	GtkTreeIter files;
	GtkTreeIter editor;
	GtkTreeIter po_header;
	GtkTreeIter functionality;
	GtkTreeIter autotranslation;
};

struct _GtranslatorPreferencesDialogPrivate
{
	GtkWidget *treeview;
	GtkTreeStore *store;
	Parents parents;
	
	GtkWidget *main_box;
	GtkWidget *treeview_box;
	GtkWidget *notebook;
	
	/*Files->General*/
	GtkWidget *warn_if_fuzzy;
	GtkWidget *sweep_compile_file;
	
	/*Files->Autosave*/
	GtkWidget *autosave;
	GtkWidget *autosave_timeout;
	GtkWidget *autosave_hbox;
	GtkWidget *autosave_with_suffix;
	GtkWidget *autosave_suffix;
	GtkWidget *suffix_hbox;
	
	/*Files->Recent files*/
	GtkWidget *max_history_entries;
	GtkWidget *check_recent_files;
	
	/*Editor->Text display*/
	GtkWidget *highlight;
	GtkWidget *use_dot_char;
	GtkWidget *own_fonts;
	GtkWidget *text_font;
	GtkWidget *font_hbox;
	
	/*Editor->Contents*/
	GtkWidget *unmark_fuzzy;
	GtkWidget *keep_obsolete;
	GtkWidget *instant_spell_checking;
	
	/*PO header->Personal information*/
	GtkWidget *authors_name;
	GtkWidget *authors_email;
	
	/*PO header->Language settings*/
	GtkWidget *authors_language;
	GtkWidget *lcode;
	GtkWidget *mime_type;
	GtkWidget *encoding;
	GtkWidget *lg_email;
	GtkWidget *number_plurals;
	GtkWidget *plural;
	GtkWidget *plural_note;
	
	/*Functionality->General*/
	GtkWidget *use_update_function;
	GtkWidget *rambo_function;
	GtkWidget *show_comment;
	GtkWidget *save_geometry_tb;
	
	/*Autotranslation->General*/
	//Here should be the neccessary widgets when we have this implemented
};


enum
{
	IMAGE_COLUMN = 0,
	TEXT_COLUMN,
	PAGENUM_COLUMN,
	N_COLUMNS
};

static GdkPixbuf *
create_pixbuf(const gchar *path)
{
	GdkPixbuf *icon;
	GError *error = NULL;
	
	icon = gdk_pixbuf_new_from_file(path, &error);
	
	if (error)
	{
		g_warning ("Could not load icon: %s\n", error->message);
		g_error_free(error);
		error = NULL;
		return NULL;
	}
	
	return icon;
}

static GtkTreePath* prev_path = NULL;

static void
gtranslator_control_table_node_expanded(GtkTreeView *treeview,
					GtkTreeIter *arg1,
					GtkTreePath *new_path,
					gpointer selection)
{	
	if (prev_path != NULL) {
		if(gtk_tree_path_get_depth(prev_path) > 1)
			gtk_tree_path_up(prev_path);
	}
	gtk_tree_path_down(new_path);
	gtk_tree_selection_select_path(selection,new_path);
}

static void 
gtranslator_control_table_selection_changed(GtkTreeSelection *selection,
					    GtranslatorPreferencesDialog *dlg)
{
	GtkTreePath *new_path, *new_path_parent = gtk_tree_path_new();
	GtkTreeIter iter;
	GtkTreeModel *model;
	gint page;
	GtkTreeView *view;
  
	view = gtk_tree_selection_get_tree_view(selection);
  
	if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
	{
    		gtk_tree_model_get(model, &iter, PAGENUM_COLUMN, &page, -1);

		/*compare old & new selected path*/
	/*	new_path = gtk_tree_model_get_path(model, &iter);
    		new_path_parent = gtk_tree_path_copy(new_path);*/
	
		/*if(prev_path != NULL)
		{		
			if(gtk_tree_path_get_depth(prev_path) > 1)
				gtk_tree_path_up(prev_path);
			if(gtk_tree_path_get_depth(new_path) > 1)
				gtk_tree_path_up(new_path_parent);
		*/	
		/*collapse old node*/
		/*	if(gtk_tree_path_compare(prev_path,new_path_parent) !=0)
				gtk_tree_view_collapse_row(view,prev_path);
		}*/
		/*expand new node*/
		//gtk_tree_view_expand_to_path(view,new_path);
	
		/*show new page*/
		if(page !=-1)
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(dlg->priv->notebook),
						      page);
		}/* else { 
	  		gtk_tree_path_down(new_path);
	  		gtk_tree_selection_select_path(selection,new_path);	  
		}*/
		/*prev_path = gtk_tree_path_copy(new_path);
		gtk_tree_path_free(new_path);
		gtk_tree_path_free(new_path_parent);*/
	}

}

static void
gtranslator_preferences_dialog_control_new(GtranslatorPreferencesDialog *dlg)
{
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;
	
	dlg->priv->store = gtk_tree_store_new (N_COLUMNS,
					       GDK_TYPE_PIXBUF,
					       G_TYPE_STRING,
					       G_TYPE_INT);
	
	dlg->priv->treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL(dlg->priv->store));
	gtk_tree_view_expand_all(GTK_TREE_VIEW(dlg->priv->treeview));
	
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(dlg->priv->treeview), TRUE);
	
	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(column, "Pixbuf + category");
	
	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(column, renderer, FALSE);
	gtk_tree_view_column_set_attributes(column,
					    renderer, "pixbuf",
					    IMAGE_COLUMN, NULL);
	
	renderer=gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column,
					    renderer, "text",
					    TEXT_COLUMN, NULL);
	
	gtk_tree_view_column_set_resizable(column, TRUE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(dlg->priv->treeview), column);
	
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(dlg->priv->treeview));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	
	g_signal_connect (G_OBJECT(selection), "changed", 
			  G_CALLBACK(gtranslator_control_table_selection_changed), 
			  dlg);
	
	/*g_signal_connect (G_OBJECT(dlg->priv->treeview), "row-expanded", 
			  G_CALLBACK(gtranslator_control_table_node_expanded), 
			  selection);*/
	
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(dlg->priv->treeview),FALSE);
}

/***************Files pages****************/

static void
warn_if_fuzzy_checkbutton_toggled(GtkToggleButton *button,
				  GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->warn_if_fuzzy));
	
	gtranslator_prefs_manager_set_warn_if_fuzzy(gtk_toggle_button_get_active(button));
}

static void
sweep_compile_file_checkbutton_toggled(GtkToggleButton *button,
				       GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->sweep_compile_file));
	
	gtranslator_prefs_manager_set_sweep_compile_file(gtk_toggle_button_get_active(button));
}

static void
setup_files_general_page(GtranslatorPreferencesDialog *dlg)
{
	GdkPixbuf *pixbuf;
	GtkTreeIter iter;
	
	/*Tree item*/
	pixbuf = create_pixbuf(PIXMAPSDIR"/files.png");
	gtk_tree_store_append(dlg->priv->store, &iter, &dlg->priv->parents.files);
	gtk_tree_store_set(dlg->priv->store, &iter,
			   IMAGE_COLUMN, pixbuf,
			   TEXT_COLUMN, _("General"),
			   PAGENUM_COLUMN, 0,
			   -1);
	g_object_unref(pixbuf);
	
	/*Set initial value*/
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->warn_if_fuzzy),
				     gtranslator_prefs_manager_get_warn_if_fuzzy());
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->sweep_compile_file),
				     gtranslator_prefs_manager_get_sweep_compile_file());
	
	/*Connect signals*/
	g_signal_connect(dlg->priv->warn_if_fuzzy, "toggled",
			 G_CALLBACK(warn_if_fuzzy_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->sweep_compile_file, "toggled",
			 G_CALLBACK(sweep_compile_file_checkbutton_toggled),
			 dlg);
}

static void
autosave_checkbutton_toggled(GtkToggleButton *button,
			     GtranslatorPreferencesDialog *dlg)
{
	gboolean autosave;
	
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->autosave));
	
	autosave = gtk_toggle_button_get_active(button);
	
	gtk_widget_set_sensitive(dlg->priv->autosave_hbox, autosave);
	gtranslator_prefs_manager_set_autosave(autosave);
}

static void
autosave_with_suffix_checkbutton_toggled(GtkToggleButton *button,
					 GtranslatorPreferencesDialog *dlg)
{
	gboolean aws;
	
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->autosave_with_suffix));
	
	aws = gtk_toggle_button_get_active(button);
	
	gtk_widget_set_sensitive(dlg->priv->suffix_hbox, aws);
	gtranslator_prefs_manager_set_autosave_with_suffix(aws);
}

static void
autosave_timeout_spinbutton_value_changed(GtkSpinButton *spin_button,
					   GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail (spin_button == GTK_SPIN_BUTTON (dlg->priv->autosave_timeout));
	
	gtranslator_prefs_manager_set_autosave_timeout(gtk_spin_button_get_value_as_int(spin_button));
}

static void
autosave_suffix_entry_changed(GObject    *gobject,
			      GParamSpec *arg1,
			      GtranslatorPreferencesDialog *dlg)
{
	const gchar *text;
	
	text = gtk_entry_get_text(GTK_ENTRY(gobject));
	
	gtranslator_prefs_manager_set_autosave_suffix(text);
}

static void
setup_files_autosave_page(GtranslatorPreferencesDialog *dlg)
{
	GdkPixbuf *pixbuf;
	GtkTreeIter iter;
	gboolean autosave, suffix;
	gint autosave_timeout;
	const gchar *autosave_suffix;
	
	/*Tree item*/
	pixbuf = create_pixbuf(PIXMAPSDIR"/files.png");
	gtk_tree_store_append(dlg->priv->store, &iter, &dlg->priv->parents.files);
	gtk_tree_store_set(dlg->priv->store, &iter,
			   IMAGE_COLUMN, pixbuf,
			   TEXT_COLUMN, _("Autosave"),
			   PAGENUM_COLUMN, 1,
			   -1);
	g_object_unref(pixbuf);	
	
	/*Set initial value*/
	autosave = gtranslator_prefs_manager_get_autosave();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->autosave),
				     autosave);
	suffix = gtranslator_prefs_manager_get_autosave_with_suffix();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->autosave_with_suffix),
				     suffix);
	
	autosave_timeout = gtranslator_prefs_manager_get_autosave_timeout();
	
	if(autosave_timeout <= 0)
		autosave_timeout = GPM_DEFAULT_AUTOSAVE_TIMEOUT;
	
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(dlg->priv->autosave_timeout),
				  autosave_timeout);
	
	autosave_suffix = gtranslator_prefs_manager_get_autosave_suffix();
	if(!autosave_suffix)
		autosave_suffix = GPM_DEFAULT_AUTOSAVE_SUFFIX;
	gtk_entry_set_text(GTK_ENTRY(dlg->priv->autosave_suffix),
			   autosave_suffix);
	
	
	/*Set sensitive*/
	gtk_widget_set_sensitive(dlg->priv->autosave_hbox,
				 autosave);
	gtk_widget_set_sensitive(dlg->priv->suffix_hbox,
				 suffix);
	
	/*Connect signals*/
	g_signal_connect(dlg->priv->autosave, "toggled",
			 G_CALLBACK(autosave_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->autosave_with_suffix, "toggled",
			 G_CALLBACK(autosave_with_suffix_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->autosave_timeout, "value-changed",
			 G_CALLBACK(autosave_timeout_spinbutton_value_changed),
			 dlg);
	g_signal_connect(dlg->priv->autosave_suffix, "notify::text",
			 G_CALLBACK(autosave_suffix_entry_changed),
			 dlg);
}

static void
setup_files_pages(GtranslatorPreferencesDialog *dlg)
{
	GdkPixbuf *pixbuf;
	
	/*Tree item*/
	pixbuf = create_pixbuf(PIXMAPSDIR"/files.png");
	gtk_tree_store_append(dlg->priv->store, &dlg->priv->parents.files, NULL);
	gtk_tree_store_set(dlg->priv->store, &dlg->priv->parents.files,
			   IMAGE_COLUMN, pixbuf,
			   TEXT_COLUMN, _("Files"),
			   PAGENUM_COLUMN, -1,
			   -1);
	g_object_unref(pixbuf);
	
	/*Children*/
	setup_files_general_page(dlg);
	setup_files_autosave_page(dlg);
}


/***************Editor pages****************/
static void
highlight_checkbutton_toggled(GtkToggleButton *button,
			      GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->highlight));
	
	gtranslator_prefs_manager_set_highlight(gtk_toggle_button_get_active(button));
}

static void
use_dot_char_checkbutton_toggled(GtkToggleButton *button,
				 GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->use_dot_char));
	
	gtranslator_prefs_manager_set_use_dot_char(gtk_toggle_button_get_active(button));
}

static void
own_fonts_checkbutton_toggled(GtkToggleButton *button,
			      GtranslatorPreferencesDialog *dlg)
{
	gboolean own_fonts;
	
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->own_fonts));

	own_fonts = gtk_toggle_button_get_active(button);
	
	gtranslator_prefs_manager_set_own_fonts(own_fonts);
	
	gtk_widget_set_sensitive(dlg->priv->font_hbox, own_fonts);
}

static void
text_font_setted(GtkFontButton *button,
		 GtranslatorPreferencesDialog *dlg)
{
	const gchar *text_font;
	
	g_return_if_fail(button == GTK_FONT_BUTTON(dlg->priv->text_font));
	
	text_font = gtk_font_button_get_font_name(GTK_FONT_BUTTON(dlg->priv->text_font));
	
	if(text_font)
		gtranslator_prefs_manager_set_text_font(text_font);
	//FIXME:else set default font
}

static void
setup_editor_text_display_page(GtranslatorPreferencesDialog *dlg)
{
	GdkPixbuf *pixbuf;
	GtkTreeIter iter;
	gboolean own_fonts;
	const gchar *text_font;
	
	/*Tree item*/
	pixbuf = create_pixbuf(PIXMAPSDIR"/text.png");
	gtk_tree_store_append(dlg->priv->store, &iter, &dlg->priv->parents.editor);
	gtk_tree_store_set(dlg->priv->store, &iter,
			   IMAGE_COLUMN, pixbuf,
			   TEXT_COLUMN, _("Text display"),
			   PAGENUM_COLUMN, 2,
			   -1);
	g_object_unref(pixbuf);
	
	/*Set initial value*/
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->highlight),
				     gtranslator_prefs_manager_get_highlight());
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->use_dot_char),
				     gtranslator_prefs_manager_get_use_dot_char());
	
	own_fonts = gtranslator_prefs_manager_get_own_fonts();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->own_fonts),
				     own_fonts);
	
	if(own_fonts)
	{
		text_font = gtranslator_prefs_manager_get_text_font();
		if(text_font)
			gtk_font_button_set_font_name(GTK_FONT_BUTTON(dlg->priv->text_font),
						      text_font);
		//else FIXME: I think i need to set the system font (maybe Sans 12?)
	}
	
	/*Set sensitive*/
	gtk_widget_set_sensitive(dlg->priv->font_hbox, own_fonts);
	
	/*Connect signals*/
	g_signal_connect(dlg->priv->highlight, "toggled",
			 G_CALLBACK(highlight_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->use_dot_char, "toggled",
			 G_CALLBACK(use_dot_char_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->own_fonts, "toggled",
			 G_CALLBACK(own_fonts_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->text_font, "font-set",
			 G_CALLBACK(text_font_setted),
			 dlg);
}

static void
unmark_fuzzy_checkbutton_toggled(GtkToggleButton *button,
				 GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->unmark_fuzzy));
	
	gtranslator_prefs_manager_set_unmark_fuzzy(gtk_toggle_button_get_active(button));
}

static void
keep_obsolete_checkbutton_toggled(GtkToggleButton *button,
				  GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->keep_obsolete));

	gtranslator_prefs_manager_set_keep_obsolete(gtk_toggle_button_get_active(button));
}

static void
instant_spell_checking_checkbutton_toggled(GtkToggleButton *button,
					   GtranslatorPreferencesDialog *dlg)
{
	g_return_if_fail(button == GTK_TOGGLE_BUTTON(dlg->priv->instant_spell_checking));
	
	gtranslator_prefs_manager_set_instant_spell_checking(gtk_toggle_button_get_active(button));
}

static void
setup_editor_contents(GtranslatorPreferencesDialog *dlg)
{
	GdkPixbuf *pixbuf;
	GtkTreeIter iter;
	
	/*Tree item*/
	pixbuf = create_pixbuf(PIXMAPSDIR"/content.png");
	gtk_tree_store_append(dlg->priv->store, &iter, &dlg->priv->parents.editor);
	gtk_tree_store_set(dlg->priv->store, &iter,
			   IMAGE_COLUMN, pixbuf,
			   TEXT_COLUMN, _("Contents"),
			   PAGENUM_COLUMN, 3,
			   -1);
	g_object_unref(pixbuf);
	
	/*Set initial values*/
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->unmark_fuzzy),
				     gtranslator_prefs_manager_get_unmark_fuzzy());
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->keep_obsolete),
				     gtranslator_prefs_manager_get_keep_obsolete());
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dlg->priv->instant_spell_checking),
				     gtranslator_prefs_manager_get_instant_spell_checking());
	
	/*Connect signals*/
	g_signal_connect(dlg->priv->unmark_fuzzy, "toggled",
			 G_CALLBACK(unmark_fuzzy_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->keep_obsolete, "toggled",
			 G_CALLBACK(keep_obsolete_checkbutton_toggled),
			 dlg);
	g_signal_connect(dlg->priv->instant_spell_checking, "toggled",
			 G_CALLBACK(instant_spell_checking_checkbutton_toggled),
			 dlg);
}

static void
setup_editor_pages(GtranslatorPreferencesDialog *dlg)
{
	GdkPixbuf *pixbuf;
	
	/*Tree item*/
	pixbuf = create_pixbuf(PIXMAPSDIR"/files.png");
	gtk_tree_store_append(dlg->priv->store, &dlg->priv->parents.editor, NULL);
	gtk_tree_store_set(dlg->priv->store, &dlg->priv->parents.editor,
			   IMAGE_COLUMN, pixbuf,
			   TEXT_COLUMN, _("Editor"),
			   PAGENUM_COLUMN, -1,
			   -1);
	g_object_unref(pixbuf);
	
	/*Children*/
	setup_editor_text_display_page(dlg);
	setup_editor_contents(dlg);
}

/***************PO header pages****************/
static void
authors_name_text_changed(GObject    *gobject,
			  GParamSpec *arg1,
			  GtranslatorPreferencesDialog *dlg)
{
	const gchar *text;
	
	g_return_if_fail(GTK_ENTRY(gobject) == GTK_ENTRY(dlg->priv->authors_name));

	text = gtk_entry_get_text(GTK_ENTRY(gobject));
	
	if(text)
		gtranslator_prefs_manager_set_authors_name(text);
}

static void
authors_email_text_changed(GObject    *gobject,
			  GParamSpec *arg1,
			  GtranslatorPreferencesDialog *dlg)
{
	const gchar *text;
	
	g_return_if_fail(GTK_ENTRY(gobject) == GTK_ENTRY(dlg->priv->authors_email));

	text = gtk_entry_get_text(GTK_ENTRY(gobject));
	
	if(text)
		gtranslator_prefs_manager_set_authors_email(text);
}

static void
setup_po_header_personal_information_page(GtranslatorPreferencesDialog *dlg)
{
	GdkPixbuf *pixbuf;
	GtkTreeIter iter;
	const gchar *value;
	
	/*Tree item*/
	pixbuf = create_pixbuf(PIXMAPSDIR"/about_me.png");
	gtk_tree_store_append(dlg->priv->store, &iter, &dlg->priv->parents.po_header);
	gtk_tree_store_set(dlg->priv->store, &iter,
			   IMAGE_COLUMN, pixbuf,
			   TEXT_COLUMN, _("Personal information"),
			   PAGENUM_COLUMN, 4,
			   -1);
	g_object_unref(pixbuf);
	
	/*Set initial value*/
	value = gtranslator_prefs_manager_get_authors_name();
	if(value)
		gtk_entry_set_text(GTK_ENTRY(dlg->priv->authors_name),
				   value);
	value = gtranslator_prefs_manager_get_authors_email();
	if(value)
		gtk_entry_set_text(GTK_ENTRY(dlg->priv->authors_email),
				   value);
	
	/*Connect signals*/
	g_signal_connect(dlg->priv->authors_name, "notify::text",
			 G_CALLBACK(authors_name_text_changed),
			 dlg);
	g_signal_connect(dlg->priv->authors_email, "notify::text",
			 G_CALLBACK(authors_email_text_changed),
			 dlg);
}

static void
setup_po_header_language_settings_page(GtranslatorPreferencesDialog *dlg)
{
	GdkPixbuf *pixbuf;
	GtkTreeIter iter;
	
	/*Tree item*/
	pixbuf = create_pixbuf(PIXMAPSDIR"/language.png");
	gtk_tree_store_append(dlg->priv->store, &iter, &dlg->priv->parents.po_header);
	gtk_tree_store_set(dlg->priv->store, &iter,
			   IMAGE_COLUMN, pixbuf,
			   TEXT_COLUMN, _("Language settings"),
			   PAGENUM_COLUMN, 5,
			   -1);
	g_object_unref(pixbuf);
	
	/*Set initial value*/
	
	/*FIXME: I will not code anything here becouse i thing this should
	change. I don't like combobox. Translator should edit anything he wants
	here*/
	
}

static void
setup_po_header_pages(GtranslatorPreferencesDialog *dlg)
{
	GdkPixbuf *pixbuf;
	
	/*Tree item*/
	pixbuf = create_pixbuf(PIXMAPSDIR"/header.png");
	gtk_tree_store_append(dlg->priv->store, &dlg->priv->parents.po_header, NULL);
	gtk_tree_store_set(dlg->priv->store, &dlg->priv->parents.po_header,
			   IMAGE_COLUMN, pixbuf,
			   TEXT_COLUMN, _("PO header"),
			   PAGENUM_COLUMN, -1,
			   -1);
	g_object_unref(pixbuf);
	
	/*Children*/
	setup_po_header_personal_information_page(dlg);
	setup_po_header_language_settings_page(dlg);
}


static void
dialog_response_handler (GtkDialog *dlg, 
			 gint       res_id)
{
	switch (res_id)
	{
		default:
			gtk_widget_destroy (GTK_WIDGET(dlg));
	}
}

static void
gtranslator_preferences_dialog_init (GtranslatorPreferencesDialog *dlg)
{
	gboolean ret;
	GtkWidget *error_widget;
	
	dlg->priv = GTR_PREFERENCES_DIALOG_GET_PRIVATE (dlg);
	
	gtk_dialog_add_buttons (GTK_DIALOG (dlg),
				GTK_STOCK_CLOSE,
				GTK_RESPONSE_CLOSE,
				NULL);
	
	gtk_window_set_title (GTK_WINDOW (dlg), _("gtranslator Preferences"));
	gtk_window_set_resizable (GTK_WINDOW (dlg), FALSE);
	gtk_dialog_set_has_separator (GTK_DIALOG (dlg), FALSE);
	gtk_window_set_destroy_with_parent (GTK_WINDOW (dlg), TRUE);
	
	/* HIG defaults */
	gtk_container_set_border_width (GTK_CONTAINER (dlg), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->vbox), 2); /* 2 * 5 + 2 = 12 */
	gtk_container_set_border_width (GTK_CONTAINER (GTK_DIALOG (dlg)->action_area), 5);
	gtk_box_set_spacing (GTK_BOX (GTK_DIALOG (dlg)->action_area), 4);
	
	g_signal_connect (dlg,
			  "response",
			  G_CALLBACK (dialog_response_handler),
			  NULL);
	
	/*Glade*/
	
	ret = gtranslator_utils_get_glade_widgets(DATADIR"/preferences.glade",
		"main_box",
		&error_widget,
		
		"main_box", &dlg->priv->main_box,
		"notebook", &dlg->priv->notebook,
		"scrolledwindow1", &dlg->priv->treeview_box,
		"warn_if_fuzzy", &dlg->priv->warn_if_fuzzy,
		"sweep_compile_file", &dlg->priv->sweep_compile_file,
		"autosave", &dlg->priv->autosave,
		"autosave_timeout", &dlg->priv->autosave_timeout,
		"autosave_hbox", &dlg->priv->autosave_hbox,
		"autosave_with_suffix", &dlg->priv->autosave_with_suffix,
		"autosave_suffix", &dlg->priv->autosave_suffix,
		"suffix_hbox", &dlg->priv->suffix_hbox,
		"highlight", &dlg->priv->highlight,
		"use_dot_char", &dlg->priv->use_dot_char,
		"own_fonts", &dlg->priv->own_fonts,
		"text_font", &dlg->priv->text_font,
		"font_hbox", &dlg->priv->font_hbox,
		"unmark_fuzzy", &dlg->priv->unmark_fuzzy,
		"keep_obsolete", &dlg->priv->keep_obsolete,
		"instant_spell_checking", &dlg->priv->instant_spell_checking,
		"authors_name", &dlg->priv->authors_name,
		"authors_email", &dlg->priv->authors_email,
		"authors_language", &dlg->priv->authors_language,
		"lcode", &dlg->priv->lcode,
		"mime_type", &dlg->priv->mime_type,
		"encoding", &dlg->priv->encoding,
		"lg_email", &dlg->priv->lg_email,
		"number_plurals", &dlg->priv->number_plurals,
		"plural", &dlg->priv->plural,
		"plural_note", &dlg->priv->plural_note,
		NULL);
	
	if(!ret)
	{
		gtk_widget_show(error_widget);
		gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dlg)->vbox),
					     error_widget);
		
		return;
	}
	
	
	
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox),
			    dlg->priv->main_box, FALSE, FALSE, 0);
	
	gtk_container_set_border_width (GTK_CONTAINER (dlg->priv->notebook), 5);
	
	gtranslator_preferences_dialog_control_new(dlg);
	setup_files_pages(dlg);
	setup_editor_pages(dlg);
	setup_po_header_pages(dlg);
	
	
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(dlg->priv->treeview_box),
					      dlg->priv->treeview);
	
	
}

static void
gtranslator_preferences_dialog_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_preferences_dialog_parent_class)->finalize (object);
}

static void
gtranslator_preferences_dialog_class_init (GtranslatorPreferencesDialogClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorPreferencesDialogPrivate));

	object_class->finalize = gtranslator_preferences_dialog_finalize;
}

void
gtranslator_show_preferences_dialog (GtranslatorWindow *window)
{
	static GtkWidget *dlg = NULL;
	
	g_return_if_fail(GTR_IS_WINDOW(window));
	
	if(dlg == NULL)
	{
		dlg = GTK_WIDGET (g_object_new (GTR_TYPE_PREFERENCES_DIALOG, NULL));
		g_signal_connect (dlg,
				  "destroy",
				  G_CALLBACK (gtk_widget_destroyed),
				  &dlg);
		gtk_widget_show_all(dlg);
	}
	
	if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (dlg)))
	{
		gtk_window_set_transient_for (GTK_WINDOW (dlg),
					      GTK_WINDOW (window));
	}

	gtk_window_present (GTK_WINDOW (dlg));
}
