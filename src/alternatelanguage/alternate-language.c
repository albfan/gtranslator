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
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "alternate-language.h"
#include "alternate-language-panel.h"
#include "notebook.h"
#include "window.h"
#include "panel.h"

#include <glib/gi18n-lib.h>


#define GTR_MESSAGE_TABLE_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_MESSAGE_TABLE,		\
				GtranslatorAlternateLangPrivate))

G_DEFINE_TYPE(GtranslatorAlternateLang, gtranslator_alternate_lang, G_TYPE_OBJECT)


static void
gtranslator_alternate_lang_init (GtranslatorAlternateLang *message_table)
{
}

static void
gtranslator_alternate_lang_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_alternate_lang_parent_class)->finalize (object);
}


static void
create_alternate_lang_panel (GtkNotebook *notebook,
			     GtkWidget   *child,
			     guint        page_num,
			     GtranslatorWindow *window)
{
	GtkWidget *alternatelang;
	GtkWidget *image;
	GtranslatorPo *po;
	GtranslatorPanel *panel;
	
	po = gtranslator_tab_get_po(GTR_TAB(child));
	
	g_return_if_fail(po != NULL);
	
	alternatelang = gtranslator_alternate_lang_panel_new(child);
	
	panel = gtranslator_tab_get_panel(GTR_TAB(child));
	
	image = gtk_image_new_from_stock(GTK_STOCK_INDEX,
					 GTK_ICON_SIZE_SMALL_TOOLBAR);
	
	gtranslator_panel_add_item(panel,
				   alternatelang,
				   _("Alternate Language"),
				   image);
}

void
alternate_lang_activate (GtranslatorWindow *window)
{
	GtranslatorNotebook *notebook;
	
	notebook = gtranslator_window_get_notebook(window);
	
	g_signal_connect(GTK_NOTEBOOK(notebook),
			 "page-added",
			 G_CALLBACK(create_alternate_lang_panel), window);

}

void
alternate_lang_deactivate(GtranslatorWindow *window)
{
//	GtranslatorPanel *panel;

	/*panel = gtranslator_window_get_side_panel (window);
	gtranslator_panel_remove_item (panel, data->panel);

	g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);*/
}

static void
gtranslator_alternate_lang_class_init (GtranslatorAlternateLangClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	//GeditPluginClass *plugin_class = GEDIT_PLUGIN_CLASS (klass);

	object_class->finalize = gtranslator_alternate_lang_finalize;

	/*plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;*/
}
