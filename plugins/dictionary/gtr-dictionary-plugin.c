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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gtr-application.h"
#include "gtr-dictionary-plugin.h"
#include "gtr-dict-panel.h"
#include "gtr-window.h"

#include <glib/gi18n-lib.h>

#define WINDOW_DATA_KEY	"GtrDictPluginWindowData"

#define GTR_DICT_PLUGIN_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_DICT_PLUGIN,		\
				GtrDictPluginPrivate))

typedef struct
{
  GtkWidget *panel;
  guint context_id;
} WindowData;

GTR_PLUGIN_REGISTER_TYPE_WITH_CODE (GtrDictPlugin,
				    gtr_dict_plugin,
				    gtr_dict_panel_register_type
				    (module);)
     static void gtr_dict_plugin_init (GtrDictPlugin * plugin)
{
}

static void
gtr_dict_plugin_finalize (GObject * object)
{
  G_OBJECT_CLASS (gtr_dict_plugin_parent_class)->finalize (object);
}

static void
free_window_data (WindowData * data)
{
  g_return_if_fail (data != NULL);

  g_free (data);
}


static GtkWidget *
create_dict_panel (GtrWindow * window)
{
  GtkWidget *panel;

  panel = gtr_dict_panel_new (window);

  gtk_widget_show (panel);

  return panel;
}

static void
impl_activate (GtrPlugin * plugin, GtrWindow * window)
{
  WindowData *data;

  data = g_new (WindowData, 1);

  data->panel = create_dict_panel (window);

  gtr_application_register_icon (GTR_APP, "gnome-dictionary.png",
					 "dictionary-icon");

  gtr_window_add_widget (window,
				 data->panel,
				 "GtrDictionaryPlugin",
				 _("Dictionary"),
				 "dictionary-icon",
				 GTR_WINDOW_PLACEMENT_LEFT);

  g_object_set_data_full (G_OBJECT (window),
			  WINDOW_DATA_KEY,
			  data, (GDestroyNotify) free_window_data);
}

static void
impl_deactivate (GtrPlugin * plugin, GtrWindow * window)
{
  WindowData *data;

  data = (WindowData *) g_object_get_data (G_OBJECT (window),
					   WINDOW_DATA_KEY);
  g_return_if_fail (data != NULL);

  gtr_window_remove_widget (window, data->panel);

  g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);
}

static void
gtr_dict_plugin_class_init (GtrDictPluginClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtrPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

  object_class->finalize = gtr_dict_plugin_finalize;

  plugin_class->activate = impl_activate;
  plugin_class->deactivate = impl_deactivate;
}