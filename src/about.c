/*
 * (C) 2000-2007 	Fatih Demir <kabalak@kabalak.net>
 *			Gediminas Paulauskas <menesis@kabalak.net>
 *			Roy-Magne Mo <rmo@sunnmore.net>
 *			Ignacio Casal <nacho.resa@gmail.com>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "about.h"
#include "window.h"

#include <string.h>

#include <glib.h>
#include <glib/gi18n.h>

#include <gtk/gtkaboutdialog.h>

/*
 * Creates and shows the about box for gtranslator.
 */ 
void gtranslator_about_dialog(GtkWidget * widget,
			      GtranslatorWindow *window)
{
	gchar *license_trans;
	
	const gchar *authors[] = {
		"Present Developers",
		"---------------------------------------------------------",
		"Ross Golder <ross@golder.org>",
		"Ignacio Casal <nacho.resa@gmail.com>",
		"",
		"Past Developers",
		"---------------------------------------------------------",
		"Fatih Demir <kabalak@kabalak.net>",
		"---------------------------------------------------------",
		"Gediminas Paulauskas <menesis@delfi.lt>",
		"Thomas Ziehmer <thomas@kabalak.net>",
		"Peeter Vois <peeter@kabalak.net>",
		NULL
	};	
	const gchar *documenters[] =
	{
		"Abel Cheung <deaddog@deaddog.org>",
		"Emese Kovacs <emese@gnome.hu>",
		NULL
	};	
	
	
	const gchar *license[] = {
		N_("This program is free software: you can redistribute it and/or modify"
		   "it under the terms of the GNU General Public License as published by"
		   "the Free Software Foundation, either version 3 of the License, or"
		   "(at your option) any later version."),
		N_("This program is distributed in the hope that it will be useful,"
		   "but WITHOUT ANY WARRANTY; without even the implied warranty of"
		   "MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the"
		   "GNU General Public License for more details."),
		N_("You should have received a copy of the GNU General Public License"
		   "along with this program.  If not, see <http://www.gnu.org/licenses/>.")
	};


	license_trans = g_strconcat (_(license[0]), "\n\n",
	                             _(license[1]), "\n\n",
         	                     _(license[2]), "\n\n",
                	             NULL);                          

	
	gtk_show_about_dialog (GTK_WINDOW (window),
                         "comments", _("gtranslator is a po file editing suite with many bells and whistles."),
                         "version", VERSION,
                         "copyright", _("(C) 1999-2007 The Free Software Foundation, Inc."),
                         "authors", authors,
                         "documenters", documenters,
                         /*
                          * Note to translators: put here your name and email so it will show
                          * up in the "about" box
                          */
                         "translator-credits", _("translator-credits"),
                         "logo-icon-name", "gtranslator",
                         "license", license_trans,
                         "wrap-license", TRUE,
                         NULL);

	g_free (license_trans);
}
