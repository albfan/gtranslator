/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
 * 			Ignacio Casal <nacho.resa@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gnome.h>

#include "application.h"
#include "prefs-manager-app.h"

#include <locale.h>
#include <glib.h>

#include <libgnome/gnome-program.h>

#include <gconf/gconf.h>

/*
 * The ubiquitous main function...
 */
gint
main(gint argc,
     gchar *argv[])
{
	GError *error = NULL;
	GnomeProgram *program;
	
	/*
	 * Initialize gettext.
	 */ 
	setlocale (LC_ALL, "");
	
	bindtextdomain(GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	g_set_application_name(_("Gtranslator"));

	/*
	 * Initialize the GConf library.
	 */
	if(!(gconf_init(argc, argv, &error)))
	{
		if(error)
		{
			g_warning(_("Error during GConf initialization: %s."),
				error->message);
		}

		g_clear_error(&error);
	}

	/*
	 * Initialize gtranslator within libgnomeui.
	 */
	program = gnome_program_init(PACKAGE, VERSION, LIBGNOMEUI_MODULE, 
				     argc, argv,
				     GNOME_PROGRAM_STANDARD_PROPERTIES,
				     NULL);

	/*
	 * Show the application window with icon.
	 */
	gtk_window_set_default_icon_from_file(WINDOW_ICON, &error);
	if(error)
	{
		g_warning(_("Error setting the default window icon: %s"),
			  error->message);
		g_clear_error(&error);
	}
	
	gtranslator_prefs_manager_app_init();

	/* 
	 * Create the main app-window. 
	 */
	gtranslator_application_open_window(GTR_APP);
	
	/*
	 * Enter main GTK loop
	 */
	gtk_main();
	
	gtranslator_prefs_manager_app_shutdown();
	
	g_object_unref(program);
	
	return 0;
}
