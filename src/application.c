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




struct _GtranslatorApplication
{
	GObject base_instance;
	
	gchar *toolbars_file;
	EggToolbarsModel *toolbars_model;
};


struct _GtranslatorApplicationClass
{
	GObjectClass parent_class;
};

G_DEFINE_TYPE(GtranslatorApplication, gtranslator_application, G_TYPE_OBJECT)


static void
gtranslator_application_init (GtranslatorApplication *application)
{
	application->toolbars_model = egg_toolbars_model_new ();

	application->toolbars_file = g_strdup_printf(
				     "%s/.gtranslator/gtr-toolbar.xml", g_get_home_dir());
	
	egg_toolbars_model_load_names (application->toolbars_model,
				       "gtr-toolbar.xml");

	if (!egg_toolbars_model_load_toolbars (application->toolbars_model,
					       application->toolbars_file)) {
		egg_toolbars_model_load_toolbars (application->toolbars_model,
						  "gtr-toolbar.xml");
	}

	egg_toolbars_model_set_flags (application->toolbars_model, 0,
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
	GtkWidget *window;
	window = gtranslator_window_new();
	gtk_widget_show_all(window);
}
				     

EggToolbarsModel *
gtranslator_application_get_toolbars_model (GtranslatorApplication *application)
{
	return application->toolbars_model;
}

void
gtranslator_application_save_toolbars_model (GtranslatorApplication *application)
{
        egg_toolbars_model_save_toolbars (application->toolbars_model,
			 	          application->toolbars_file, "1.0");
}
