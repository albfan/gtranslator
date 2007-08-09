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

#include "application.h"
#include "prefs.h"
#include "window.h"
#include "egg-toolbars-model.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>



#define GTR_APPLICATION_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_APPLICATION,     \
					 GtranslatorApplicationPrivate))

G_DEFINE_TYPE(GtranslatorApplication, gtranslator_application, G_TYPE_OBJECT)

struct _GtranslatorApplicationPrivate
{
	GtranslatorWindow *active_window;
	
	gchar *toolbars_file;
	EggToolbarsModel *toolbars_model;
};



static void
gtranslator_application_init (GtranslatorApplication *application)
{
	GtranslatorApplicationPrivate * priv;
	
	application->priv = GTR_APPLICATION_GET_PRIVATE (application);
	priv = application->priv;
	
	priv->toolbars_model = egg_toolbars_model_new ();

	priv->toolbars_file = g_strdup_printf(
				     "%s/.gtranslator/gtr-toolbar.xml", g_get_home_dir());
	
	egg_toolbars_model_load_names (priv->toolbars_model,
				       "gtr-toolbar.xml");

	if (!egg_toolbars_model_load_toolbars (priv->toolbars_model,
					       priv->toolbars_file)) {
		egg_toolbars_model_load_toolbars (priv->toolbars_model,
						  "gtr-toolbar.xml");
	}

	egg_toolbars_model_set_flags (priv->toolbars_model, 0,
				      EGG_TB_MODEL_NOT_REMOVABLE);
	
	gtranslator_preferences_read();
}


static void
gtranslator_application_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_application_parent_class)->finalize (object);
}

static void
gtranslator_application_class_init (GtranslatorApplicationClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorApplicationPrivate));
	
	object_class->finalize = gtranslator_application_finalize;
}

GtranslatorApplication *
gtranslator_application_get_instance (void)
{
	static GtranslatorApplication *instance = NULL;
	
	if(!instance)
	{
		instance = GTR_APPLICATION(g_object_new (GTR_TYPE_APPLICATION, NULL));
	}
	return instance;
}

void
gtranslator_application_open_window (GtranslatorApplication *app)
{
	GtranslatorWindow *window;

	app->priv->active_window = window = GTR_WINDOW(g_object_new(GTR_TYPE_WINDOW, NULL));
	
	gtk_widget_show_all(GTK_WIDGET(window));
}
				     

EggToolbarsModel *
gtranslator_application_get_toolbars_model (GtranslatorApplication *application)
{
	return application->priv->toolbars_model;
}

void
gtranslator_application_save_toolbars_model (GtranslatorApplication *application)
{
        egg_toolbars_model_save_toolbars (application->priv->toolbars_model,
			 	          application->priv->toolbars_file, "1.0");
}
