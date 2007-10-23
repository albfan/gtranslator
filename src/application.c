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
#include "window.h"
#include "egg-toolbars-model.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <libgnomeui/gnome-client.h>



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

/*
 * Quits via the normal quit.
 */
static void
gtranslator_session_die(GnomeClient * client,
			GtranslatorApplication *app)
{
	gtranslator_application_shutdown(app);
}

/*
 * Saves the state of gtranslator before quitting.
 */
static gint
gtranslator_session_sleep(GnomeClient * client, gint phase,
			  GnomeSaveStyle s_style, gint shutdown,
			  GnomeInteractStyle i_style, gint fast,
			  GtranslatorApplication *app)
{
	GtranslatorTab *tab = gtranslator_window_get_active_tab(app->priv->active_window);
	GtranslatorPo *po = gtranslator_tab_get_po(tab);
	
	gchar *argv[] = {
		"rm",
		"-r",
		NULL
	};
	
	/*
	 * The state (for now only the current message number) is stored
	 *  in the preferences.
	 */
/*	gtranslator_config_set_int("state/message_number", 
			     g_list_position(gtranslator_po_get_messages(po),
					     gtranslator_po_get_current_message(po)));*/
	
	argv[2] = NULL;

	gnome_client_set_discard_command(client, 3, argv);

	//argv[0] = (gchar *) data;
	argv[1] = gtranslator_po_get_filename(po);

	gnome_client_set_restart_command(client, 2, argv);

	return TRUE;
}

/*
 * Restores a previously closed session.
 */ 
static void
gtranslator_session_restore(GnomeClient * client,
			    GtranslatorApplication *app)
{
	guint num;

	/*num = gtranslator_config_get_int("state/message_number");

	gtranslator_message_go_to_no(NULL, GUINT_TO_POINTER(num));

	push_statusbar_data(NULL, _("Session restored successfully."));*/
	
}

static void
gtranslator_init_session(GtranslatorApplication *app)
{
	GnomeClient *client;
	GnomeClientFlags flags;
	
	client = gnome_master_client();
	
	g_signal_connect(G_OBJECT(client), "save_yourself",
			 G_CALLBACK(gtranslator_session_sleep),
			 app);
	g_signal_connect(G_OBJECT(client), "die",
			 G_CALLBACK(gtranslator_session_die), app);
	
	flags = gnome_client_get_flags(client);
	if(flags & GNOME_CLIENT_RESTORED)
	{
//		gtranslator_session_restore(client);
	}
}



/***************
 FIXME: This should be improved
***************/
static gboolean
on_window_delete_event_cb(GtranslatorWindow *window,
			  GdkEvent *event,
			  GtranslatorApplication *app)
{
	gtranslator_file_quit(NULL, window);
	g_warning("delete app");
	return TRUE;
}

static void
on_window_destroy_cb(GtranslatorWindow *window,
		     GtranslatorApplication *app)
{
	g_warning("destroy app");
	//if(app->priv->active_window == NULL)
		g_object_unref(app);
}

static void
gtranslator_application_init (GtranslatorApplication *application)
{
	GtranslatorApplicationPrivate * priv;
	
	application->priv = GTR_APPLICATION_GET_PRIVATE (application);
	priv = application->priv;
	
	gtranslator_init_session(application);
	
	priv->toolbars_model = egg_toolbars_model_new ();

	priv->toolbars_file = g_strdup_printf(
				     "%s/.gtranslator/gtr-toolbar.xml", g_get_home_dir());
	
	egg_toolbars_model_load_names (priv->toolbars_model,
				       DATADIR"/gtr-toolbar.xml");

	if (!egg_toolbars_model_load_toolbars (priv->toolbars_model,
					       priv->toolbars_file)) {
		egg_toolbars_model_load_toolbars (priv->toolbars_model,
						  DATADIR"/gtr-toolbar.xml");
	}

	egg_toolbars_model_set_flags (priv->toolbars_model, 0,
				      EGG_TB_MODEL_NOT_REMOVABLE);
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

static void
app_weak_notify (gpointer data,
                 GObject *where_the_app_was)
{
        gtk_main_quit ();
}

GtranslatorApplication *
gtranslator_application_get_instance (void)
{
	static GtranslatorApplication *instance = NULL;
	
	if(!instance)
	{
		instance = GTR_APPLICATION(g_object_new (GTR_TYPE_APPLICATION, NULL));
		
		g_object_weak_ref (G_OBJECT (instance),
				   app_weak_notify,
				   NULL);
	}
	return instance;
}

void
gtranslator_application_open_window (GtranslatorApplication *app)
{
	GtranslatorWindow *window;
	GdkWindowState state;
	gint w,h;

	app->priv->active_window = window = GTR_WINDOW(g_object_new(GTR_TYPE_WINDOW, NULL));
	
	state = gtranslator_prefs_manager_get_window_state();
	
	if((state & GDK_WINDOW_STATE_MAXIMIZED) != 0)
	{
		gtranslator_prefs_manager_get_window_size(&w, &h);
		gtk_window_set_default_size (GTK_WINDOW (window), w, h);
                gtk_window_maximize (GTK_WINDOW (window));
	}
	else
	{
		gtranslator_prefs_manager_get_window_size(&w, &h);
		gtk_window_set_default_size (GTK_WINDOW (window), w, h);
                gtk_window_unmaximize (GTK_WINDOW (window));
	}
	
	g_signal_connect(window, "delete-event",
			 G_CALLBACK(on_window_delete_event_cb), GTR_APP);
	
	g_signal_connect(window, "destroy",
			 G_CALLBACK(on_window_destroy_cb), GTR_APP);
	
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

void
gtranslator_application_shutdown(GtranslatorApplication *app)
{
	g_warning("shutdown");
	
	if(app->priv->toolbars_model)
	{
		g_object_unref(app->priv->toolbars_model);
		g_free(app->priv->toolbars_file);
		app->priv->toolbars_model = NULL;
		app->priv->toolbars_file = NULL;
	}
	
	g_object_unref(app);
	
	//gtk_main_quit();
}
