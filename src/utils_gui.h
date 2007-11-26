/*
 * (C) 2001 	Fatih Demir <kabalak@kabalak.net>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef GTR_UTILS_GUI_H
#define GTR_UTILS_GUI_H 1

#include "po.h"

#include <gtk/gtkwidget.h>
#include <gtk/gtksignal.h>

/*
 * GUI related utility functions for gtranslator.
 */



/*
 * Routines for saving/restoring/setting geometry of the main window.
 */
//void gtranslator_utils_save_geometry(GtranslatorWindow *window);

/*
 * Convenience functions for adding items 
 */
GtkWidget *gtranslator_utils_attach_combo_with_label(GtkWidget * table, 
	gint row, const char *label_text, GList * list, const char *value,
	gboolean editable, GCallback callback, gpointer user_data);

GtkWidget *gtranslator_utils_attach_entry_with_label(GtkWidget * table, 
	gint row, const char *label_text, const char *value,
	GCallback callback);

GtkWidget *gtranslator_utils_attach_toggle_with_label(GtkWidget * table, 
	gint row, const char *label_text, gboolean value,
	GCallback callback);

GtkWidget *gtranslator_utils_attach_text_with_label(GtkWidget * table, 
	gint row, const char *label_text, const char *value,
	GCallback callback);

GtkWidget *gtranslator_utils_attach_spin_with_label(GtkWidget *table,
	gint row, const gchar *label_text, gfloat minimum, gfloat maximum,
	gfloat value, GCallback callback);
	
GtkWidget *gtranslator_utils_attach_font_with_label(GtkWidget *table,
	gint row, const gchar *label_text, const gchar *title_text,
	const gchar *fontspec, GCallback callback);
	
/*
 * Checks the given file for the right permissions for
 *  open/writing the files.
 */
gboolean gtranslator_utils_check_file_permissions(GtranslatorPo *po_file);

/*
 * Checks if we've been started with the given file yet.
 */
gboolean gtranslator_utils_check_file_being_open(const gchar *filename);

/*
 * Check for a given program being in the user's path -- should be useful
 *  for the "differently" opening/saving functions...
 */
gboolean gtranslator_utils_check_program(const gchar *program_name,
	const gint type_int);



gboolean       gtranslator_utils_get_glade_widgets (const gchar *filename,
						    const gchar *root_node,
						    GtkWidget **error_widget,
						    const gchar *widget_name,
						    ...)G_GNUC_NULL_TERMINATED;

gchar **gtranslator_utils_drop_get_uris (GtkSelectionData *selection_data);

gchar *gtranslator_utils_escape_search_text (const gchar* text);

gchar *gtranslator_utils_unescape_search_text (const gchar *text);

gboolean             g_utf8_caselessnmatch         (const gchar *s1,
						    const gchar *s2,
						    gssize n1,
						    gssize n2);

#endif
