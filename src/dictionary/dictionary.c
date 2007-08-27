#include "dictionary.h"
#include "dict-panel.h"
#include "panel.h"
#include "window.h"

#include <glib/gi18n.h>

#define WINDOW_DATA_KEY	"DictionaryWindowData"

static GtkWidget *
gtranslator_dictionary_create(GtranslatorWindow *window)
{
	GtkWidget *panel;
	
	panel = gtranslator_dict_panel_new();
	
	gtk_widget_show_all(panel);
	
	return panel;
}

void
dictionary_activate(GtranslatorWindow *window)
{
	GtranslatorPanel *panel;
	GtkIconTheme *theme;
	GtkWidget *image;
	GtkWidget *dictionary;
	
	panel = gtranslator_window_get_side_panel(window);
	
	theme = gtk_icon_theme_get_default();
	
	if(gtk_icon_theme_has_icon(theme, "accessories-dictionary"))
		image = gtk_image_new_from_icon_name ("accessories-dictionary",
						      GTK_ICON_SIZE_MENU);
	else
		image = gtk_image_new_from_icon_name ("gdict",
						      GTK_ICON_SIZE_MENU);
				   
	dictionary = gtranslator_dictionary_create(window);
	
	gtranslator_panel_add_item(panel, dictionary,
				   _("Dictionary"),
				   image);
	
	gtk_object_sink (GTK_OBJECT (image));
	
	g_object_set_data_full (G_OBJECT (window),
				WINDOW_DATA_KEY,
				dictionary,
				(GDestroyNotify) gtk_widget_destroy);
}

void
dictionary_deactivate(GtranslatorWindow *window)
{
	GtranslatorPanel *panel;
	GtkWidget *dictionary;
	
	dictionary = GTK_WIDGET(g_object_get_data(G_OBJECT(window),
						  WINDOW_DATA_KEY));
	
	g_return_if_fail (dictionary != NULL);
	
	panel = gtranslator_window_get_side_panel(window);
	
	gtranslator_panel_remove_item(panel, dictionary);
	
	g_object_set_data(G_OBJECT(window), WINDOW_DATA_KEY, NULL);
}
