/*
 * Copyright (C) 2007  Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
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

#include "open-tran-plugin.h"
#include "open-tran-panel.h"

#include <glib/gi18n-lib.h>
#include "window.h"

#define OPEN_TRAN_PLUGIN_ICON PIXMAPSDIR"/open-tran.png"
#define WINDOW_DATA_KEY	"GtranslatorOpenTranPluginWindowData"


#define GTR_OPEN_TRAN_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_OPEN_TRAN_PLUGIN,		\
				GtranslatorOpenTranPluginPrivate))

typedef struct
{
	GtkWidget	*panel;
	guint		 context_id;
} WindowData;

GTR_PLUGIN_REGISTER_TYPE_WITH_CODE (GtranslatorOpenTranPlugin, gtranslator_open_tran_plugin,
		gtranslator_open_tran_panel_register_type (module);
)

static void
gtranslator_open_tran_plugin_init (GtranslatorOpenTranPlugin *plugin)
{
	//gtranslator_debug_message (DEBUG_PLUGINS, "GtranslatorOpenTranPlugin initializing");
}

static void
gtranslator_open_tran_plugin_finalize (GObject *object)
{
	//gtranslator_debug_message (DEBUG_PLUGINS, "GtranslatorOpenTranPlugin finalizing");

	G_OBJECT_CLASS (gtranslator_open_tran_plugin_parent_class)->finalize (object);
}


static void
impl_activate (GtranslatorPlugin *plugin,
	       GtranslatorWindow *window)
{
	//GtkWidget *image = NULL;
	GtkWidget *opentran;
	/*GdkPixbuf *pixbuf;
	GtkIconSet *iconset;
	GError *error = NULL;*/
	
	/*pixbuf = gdk_pixbuf_new_from_file(OPEN_TRAN_PLUGIN_ICON, &error);
	
	if (error)
	{
		g_warning ("Could not load icon: %s\n", error->message);
		g_error_free(error);
		pixbuf = NULL;
	}
	
	if(pixbuf)
	{
		iconset = gtk_icon_set_new_from_pixbuf(pixbuf);
	
		image = gtk_image_new_from_icon_set(iconset,
						    GTK_ICON_SIZE_MENU);
	}*/

	opentran = gtranslator_open_tran_panel_new(window);

	gtranslator_window_add_widget (window,
				       opentran,
				       "GtranslatorOpenTranPlugin",
				       _("Open Tran"),
				       NULL,
				       GTR_WINDOW_PLACEMENT_LEFT);

	g_object_set_data(G_OBJECT(window),
			  WINDOW_DATA_KEY,
			  opentran);
	
	gtk_widget_show_all(opentran);
}

static void
impl_deactivate	(GtranslatorPlugin *plugin,
		 GtranslatorWindow *window)
{
	GtkWidget *opentran;

	opentran = (GtkWidget *) g_object_get_data (G_OBJECT (window),
						    WINDOW_DATA_KEY);
	g_return_if_fail (opentran != NULL);

	gtranslator_window_remove_widget (window, opentran);
	
	g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);
}

static void
gtranslator_open_tran_plugin_class_init (GtranslatorOpenTranPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

	object_class->finalize = gtranslator_open_tran_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
}
