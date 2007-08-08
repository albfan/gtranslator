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

#ifndef __FILE_DIALOGS_H__
#define __FILE_DIALOGS_H__

#include <gtk/gtk.h>
#include "window.h"

void gtranslator_open_file_dialog(GtkWidget * widget,
				  GtranslatorWindow *window);

void gtranslator_save_file_dialog(GtkWidget *widget,
				  GtranslatorWindow *window);

void gtranslator_save_file_as_dialog(GtkWidget * widget,
				     GtranslatorWindow *window);

void gtranslator_file_dialogs_store_directory(const gchar *filename);


#endif
