/*
 * (C) 2001 	Fatih Demir <kabalak@gtranslator.org>
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

#include "formats.h"
#include "nautilus-string.h"

#include <ctype.h>
#include <string.h>

/*
 * "Simply" checks the given message for right formats.
 */
gboolean gtranslator_formats_check(GtrMsg *msg)
{
	gchar	*opstring;
	gchar 	**id_formats;
	gchar 	**str_formats;
	guint	check=0;
	guint	id_occurences=0;
	guint	str_occurences=0;
	
	g_return_val_if_fail(GTR_MSG(msg)!=NULL, FALSE);
	g_return_val_if_fail(GTR_MSG(msg)->msgid!=NULL, FALSE);

	if(!GTR_MSG(msg)->msgstr)
	{
		return FALSE;
	}

	/*
	 * Reverse the strings to get the '%' character as a separator.
	 */
	opstring=g_strdup(GTR_MSG(msg)->msgid);
	g_strreverse(opstring);
	
	/*
	 * Cound the format count and split the string up for the array
	 *  use.
	 */
	id_occurences=nautilus_str_count_characters(opstring, '%');
	id_formats=g_strsplit(opstring, "%", id_occurences);
	g_free(opstring);

	/*
	 * The same again for the translation (aka. msgstr).
	 */
	opstring=g_strdup(GTR_MSG(msg)->msgstr);
	g_strreverse(opstring);

	str_occurences=nautilus_str_count_characters(opstring, '%');
	
	/*
	 * Check if we've got the same count of formats first.
	 */
	if(id_occurences!=str_occurences)
	{
		g_strfreev(id_formats);
		return FALSE;
	}
	
	str_formats=g_strsplit(opstring, "%", str_occurences);
	g_free(opstring);

	while(id_formats[check])
	{
		if(!str_formats[check])
		{
			g_strfreev(id_formats);
			g_strfreev(str_formats);
			return FALSE;
		}
		else
		{
			if(id_formats[check]==str_formats[check])
			{
				continue;
			}
			else if(isdigit(id_formats[check]) && 
				isdigit(str_formats[check]))
			{
				continue;
			}
			else
			{
				g_strfreev(id_formats);
				g_strfreev(str_formats);
				return FALSE;
			}
		}

		check++;
	}

	g_strfreev(id_formats);
	g_strfreev(str_formats);
	return TRUE;
}
