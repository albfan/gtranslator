/*
 * (C) 2000-2004 	Fatih Demir <kabalak@kabalak.net>
 *			Ross Golder <ross@golder.org>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Peeter Vois <peeter@kabalak.net>
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
#include "prefs.h"
#include "runtime-config.h"
#include "sighandling.h"
#include "translator.h"
#include "utils.h"

#include <locale.h>
#include <glib.h>

#include <signal.h>

#include <libgnome/gnome-program.h>

#include <libgnomevfs/gnome-vfs-init.h>

#include <gconf/gconf.h>

/*
 * The static variables used in the poptTable.
 */
static gchar 	*gtranslator_geometry=NULL;

/*
 * List of files that are currently open
 */
GSList *open_files;

/*
 * gtranslator's option table.
 */
static struct poptOption gtranslator_options[] = {
	{
	 	NULL, '\0', POPT_ARG_INTL_DOMAIN, PACKAGE,
	 	0, NULL, NULL
	},
	{
		"geometry", 'g', POPT_ARG_STRING, &gtranslator_geometry,
		0, N_("Specify main window geometry"), N_("GEOMETRY")
	},
	POPT_AUTOHELP {NULL}
};

/*
 * The ubiquitous main function...
 */
gint
main(gint argc,
     gchar *argv[])
{
	GnomeProgram    *program=NULL;
	
	
	poptContext 	context;
	
	const gchar 	**args=NULL;
	GValue value = { 0, };

	GError		*error=NULL;

	gint			i;
	
	/*
	 * Initialize gettext.
	 */ 
	bindtextdomain(PACKAGE, DATADIR "/locale");
	textdomain(PACKAGE);

	/*
	 * Set up the signal handler.
	 */
	/*signal(SIGSEGV, gtranslator_signal_handler);
	signal(SIGKILL, gtranslator_signal_handler);
	signal(SIGILL, gtranslator_signal_handler);
	signal(SIGABRT, gtranslator_signal_handler);
	signal(SIGINT, gtranslator_signal_handler);
	signal(SIGHUP, gtranslator_signal_handler);
	signal(SIGQUIT, gtranslator_signal_handler);
	signal(SIGTERM, gtranslator_signal_handler);*/

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
	setlocale(LC_ALL, "");
	program = gnome_program_init(PACKAGE, VERSION, LIBGNOMEUI_MODULE, 
			   argc, argv,
			   GNOME_PARAM_POPT_TABLE, gtranslator_options, 
			   GNOME_PROGRAM_STANDARD_PROPERTIES,
			   NULL);
	context = gnome_program_preinit(program, PACKAGE, VERSION, argc, argv);
	bind_textdomain_codeset(PACKAGE, "UTF-8");

	/* Initialize configuration client */
	gtranslator_config_init();

	/*
	 * Show the application window with icon.
	 */
	gtk_window_set_default_icon_from_file(WINDOW_ICON, &error);
	if(error)
	{
		g_warning(_("Error setting the default window icon: %s"),
			  error->message);
	}
	g_clear_error(&error);

	/*
	 * Create our own .gtranslator directory in the user's home directory.
	 */
	//gtranslator_utils_create_gtranslator_directory();

	/*
	 * Read the translator information/data into our generally used 
	 *  GtrTranslator structure.
	 */
	gtranslator_translator=gtranslator_translator_new();
	

	/*
	 * Initialize our generally used GtrRuntimeConfig structure.
	 */
	gtranslator_runtime_config=gtranslator_runtime_config_new();

	/* 
	 * Create the main app-window. 
	 */
	gtranslator_application_open_window(GTR_APP);

	/*
	 * Initialize GnomeVFS right now, if needed.
	 */
	if(!gnome_vfs_initialized())
	{
		gnome_vfs_init();
	}
	

	/*
	 * Clean up the temporary file in the user's home dir eventually 
	 *  created by gtranslator.
	 */
	//gtranslator_utils_remove_temp_files();

	/*
	 * Test if there's a crash recovery file lying around in ~/.gtranslator.
	 */
/*	if(g_file_test(gtranslator_runtime_config->crash_filename, G_FILE_TEST_EXISTS))
	{
		gtranslator_rescue_file_dialog();
	}
*/
	/*
	 * Open up the arguments as files (for now, only the first file is
	 *  opened).
	 */
	g_value_init (&value, G_TYPE_POINTER);
	g_object_get_property (G_OBJECT (program), GNOME_PARAM_POPT_CONTEXT, &value);
	context = g_value_get_pointer (&value);
	g_value_unset (&value);

	args = poptGetArgs(context);
	for (i = 0; args && args[i]; i++)
	{
		/*
		 * Try to open up the supported "special" gettext file types.
		 */ 
		//FIXME
		/*if(!gtranslator_open((gchar *)args[i],NULL, &error))
		{
			g_assert(error!=NULL);
			fprintf(stderr, _("Couldn't open '%s': %s\n"),
				(gchar *)args[i], error->message);
			g_clear_error(&error);
			return 1;
		}*/
	}
	
	poptFreeContext(context);

	/*
	 * Enter main GTK loop
	 */
	gtk_main();
	
	return 0;
}
