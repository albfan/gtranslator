/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 * 		 2006  Steve Frécinaux
 *
 * Based in gedit charmap plugin.
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     Based on Gedit charmap plugin
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "charmap.h"
#include "charmap-panel.h"
#include "window.h"
#include "panel.h"

#include <glib/gi18n-lib.h>
#include <gucharmap/gucharmap-table.h>
#include <gucharmap/gucharmap-unicode-info.h>

#define WINDOW_DATA_KEY	"CharmapWindowData"

#define GTR_CHARMAP_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_CHARMAP,		\
				GtranslatorCharmapPrivate))

G_DEFINE_TYPE(GtranslatorCharmap, gtranslator_charmap, G_TYPE_OBJECT)

typedef struct
{
	GtkWidget	*panel;
	guint		 context_id;
} WindowData;



static void
gtranslator_charmap_init (GtranslatorCharmap *charmap)
{
}

static void
gtranslator_charmap_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_charmap_parent_class)->finalize (object);
}

static void
free_window_data (WindowData *data)
{
	g_return_if_fail (data != NULL);

	if (G_IS_OBJECT(data->panel))
		g_object_unref (data->panel);

	g_free (data);
}

static void
on_table_status_message (GucharmapTable *chartable,
			 const gchar    *message,
			 GtranslatorWindow    *window)
{
	GtkStatusbar *statusbar;
	WindowData *data;

	statusbar = GTK_STATUSBAR (gtranslator_window_get_statusbar (window));
	data = (WindowData *) g_object_get_data (G_OBJECT (window),
						 WINDOW_DATA_KEY);
	g_return_if_fail (data != NULL);

	gtk_statusbar_pop (statusbar, data->context_id);

	if (message)
		gtk_statusbar_push (statusbar, data->context_id, message);
}

static void
on_table_set_active_char (GucharmapTable *chartable,
			  gunichar        wc,
			  GtranslatorWindow    *window)
{
	GString *gs;
	const gchar *temp;
	const gchar **temps;
	gint i;

	gs = g_string_new (NULL);
	g_string_append_printf (gs, "U+%4.4X %s", wc, 
				gucharmap_get_unicode_name (wc));

	temps = gucharmap_get_nameslist_equals (wc);
	if (temps)
	{
		g_string_append_printf (gs, "   = %s", temps[0]);
		for (i = 1;  temps[i];  i++)
			g_string_append_printf (gs, "; %s", temps[i]);
		g_free (temps);
	}

	temps = gucharmap_get_nameslist_stars (wc);
	if (temps)
	{
		g_string_append_printf (gs, "   \342\200\242 %s", temps[0]);
		for (i = 1;  temps[i];  i++)
			g_string_append_printf (gs, "; %s", temps[i]);
		g_free (temps);
	}

	on_table_status_message (chartable, gs->str, window);
	g_string_free (gs, TRUE);
}

static gboolean
on_table_focus_out_event (GtkWidget      *drawing_area,
			  GdkEventFocus  *event,
			  GtranslatorWindow    *window)
{
	GucharmapTable *chartable;
	WindowData *data;
	
	data = (WindowData *) g_object_get_data (G_OBJECT (window),
						 WINDOW_DATA_KEY);
	g_return_val_if_fail (data != NULL, FALSE);

	chartable = gtranslator_charmap_panel_get_table
					(GTR_CHARMAP_PANEL (data->panel));

	on_table_status_message (chartable, NULL, window);
	return FALSE;
}

static void
on_table_activate (GucharmapTable *chartable, 
		   gunichar        wc, 
		   GtranslatorWindow    *window)
{
	GtkTextView   *view;
	GtkTextBuffer *document;
	GtkTextIter start, end;
	gchar buffer[6];
	gchar length;
	
	g_return_if_fail (gucharmap_unichar_validate (wc));
	
	view = GTK_TEXT_VIEW (gtranslator_window_get_active_view (window));
	
	if (!view || !gtk_text_view_get_editable (view))
		return;
	
	document = gtk_text_view_get_buffer (view);
	
	g_return_if_fail (document != NULL);
	
	length = g_unichar_to_utf8 (wc, buffer);

	gtk_text_buffer_begin_user_action (document);
		
	gtk_text_buffer_get_selection_bounds (document, &start, &end);

	gtk_text_buffer_delete_interactive (document, &start, &end, TRUE);
	if (gtk_text_iter_editable (&start, TRUE))
		gtk_text_buffer_insert (document, &start, buffer, length);
	
	gtk_text_buffer_end_user_action (document);
}

static GtkWidget *
create_charmap_panel (GtranslatorWindow *window)
{
	GtkWidget      *panel;
	GucharmapTable *table;
	gchar          *font;

	panel = gtranslator_charmap_panel_new ();
	table = gtranslator_charmap_panel_get_table (GTR_CHARMAP_PANEL (panel));

	/* Use the same font as the document */
	/*font = gedit_prefs_manager_get_editor_font ();
	gucharmap_table_set_font (table, font);
	g_free (font);*/

	g_signal_connect (table,
			  "status-message",
			  G_CALLBACK (on_table_status_message),
			  window);

	g_signal_connect (table,
			  "set-active-char",
			  G_CALLBACK (on_table_set_active_char),
			  window);

	/* Note: GucharmapTable does not provide focus-out-event ... */
	g_signal_connect (table->drawing_area,
			  "focus-out-event",
			  G_CALLBACK (on_table_focus_out_event),
			  window);

	g_signal_connect (table,
			  "activate", 
			  G_CALLBACK (on_table_activate),
			  window);

	gtk_widget_show_all (panel);

	return panel;
}

void
impl_activate (GtranslatorWindow *window)
{
	GtranslatorPanel *panel;
	GtkWidget *image;
	GtkIconTheme *theme;
	GtkStatusbar *statusbar;
	WindowData *data;

	//gedit_debug (DEBUG_PLUGINS);

	panel = gtranslator_window_get_side_panel (window);

	data = g_new (WindowData, 1);

	theme = gtk_icon_theme_get_default ();
	
	if (gtk_icon_theme_has_icon (theme, "accessories-character-map"))
		image = gtk_image_new_from_icon_name ("accessories-character-map",
						      GTK_ICON_SIZE_MENU);
	else
		image = gtk_image_new_from_icon_name ("gucharmap",
						      GTK_ICON_SIZE_MENU);

	data->panel = create_charmap_panel (window);
	
	gtranslator_panel_add_item (panel,
			      data->panel,
			      _("Character Map"),
			      image);

	gtk_object_sink (GTK_OBJECT (image));

	statusbar = GTK_STATUSBAR (gtranslator_window_get_statusbar (window));
	data->context_id = gtk_statusbar_get_context_id (statusbar,
							 "Character Description");

	g_object_set_data_full (G_OBJECT (window),
				WINDOW_DATA_KEY,
				data,
				(GDestroyNotify) free_window_data);
}

void
impl_deactivate	(GtranslatorWindow *window)
{
	GtranslatorPanel *panel;
	GucharmapTable *chartable;
	WindowData *data;

	//gedit_debug (DEBUG_PLUGINS);

	data = (WindowData *) g_object_get_data (G_OBJECT (window),
						 WINDOW_DATA_KEY);
	g_return_if_fail (data != NULL);

	chartable = gtranslator_charmap_panel_get_table
					(GTR_CHARMAP_PANEL (data->panel));
	on_table_status_message (chartable, NULL, window);

	panel = gtranslator_window_get_side_panel (window);
	gtranslator_panel_remove_item (panel, data->panel);

	g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);
}

static void
gtranslator_charmap_class_init (GtranslatorCharmapClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	//GeditPluginClass *plugin_class = GEDIT_PLUGIN_CLASS (klass);

	object_class->finalize = gtranslator_charmap_finalize;

	/*plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;*/
}
