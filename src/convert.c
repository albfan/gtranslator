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

#ifdef HAVE_CONFIG_H
#include <../config.h>
#endif

#include "convert.h"
#include "utils.h"

#include <iconv.h>

/*
 * Convert the given string from the given charset to the given "to" charset.
 */
gchar *gtranslator_convert_string(const gchar *string, const gchar *from,
	const gchar *to)
{
	gchar 	*converted_string;
	gchar	*original_string;
	iconv_t iconv_master;
	size_t 	in_bytes, out_bytes;
	
	g_return_val_if_fail(string!=NULL, NULL);
	g_return_val_if_fail(to!=NULL, NULL);

	/*
	 * We do assume the default charset if no charset has been given.
	 */
	if(!from)
	{
		from=gtranslator_utils_get_locale_name();
	}

	original_string=g_strdup(string);

	in_bytes=(strlen(original_string) + 1);
	out_bytes=(in_bytes << 1);

	converted_string=g_malloc0(out_bytes);

	/*
	 * Open up the iconv for out task.
	 */
	iconv_master=iconv_open(to, from);
	g_return_val_if_fail(iconv_master!=((iconv_t) -1), NULL);

	/*
	 * This piece of code is lend from Balsa; so we don't get compile
	 *  warnings with iconv.
	 *
	 * Quote from original Balsa code in print.c:
	 *
	 * "the prototype of iconv() changed with glibc 2.2"
	 */
	#if defined __GLIBC__ && __GLIBC_MINOR__ <= 1
	iconv(iconv_master, (const char **) &original_string, &in_bytes, 
		&converted_string, &out_bytes);
	#else
	iconv(iconv_master, &original_string, &in_bytes, 
		&converted_string, &out_bytes);
	#endif
	
	iconv_close(iconv_master);

	/*
	 * Return the converted string in case of success.
	 */
	if(converted_string)
	{
		return converted_string;
	}
	else
	{
		return NULL;
	}
}

/*
 * Simple wrappers for the converting functions.
 */
gchar *gtranslator_convert_string_to_utf8(const gchar *string)
{
	gchar *charset=gtranslator_utils_get_locale_charset();

	/*
	 * Assume iso-8859-1 in worst-case.
	 */
	if(!charset)
	{
		charset=g_strdup("iso-8859-1");
	}
	
	return (gtranslator_convert_string(string, charset, "UTF-8"));

	g_free(charset);
}
