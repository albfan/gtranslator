/*
 * (C) 2000-2001 	Fatih Demir <kabalak@kabalak.net>
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

#ifndef GTR_OPEN_H
#define GTR_OPEN_H 1

#include <glib.h>

/*
 * The general file opening action starts from here!
 */

typedef enum 
{
  GTR_OPEN_FILE_ERROR_MISSING_PROGRAM,
  GTR_OPEN_FILE_ERROR_OTHER
} GtrOpenFileError;

#define GTR_OPEN_FILE_ERROR gtranslator_open_file_error_quark()
GQuark gtranslator_open_file_error_quark (void);

/*
 * Open the given filename string via all of our supported, available methods.
 */
gboolean gtranslator_open_file(gchar *filename, GError **error);

#endif
