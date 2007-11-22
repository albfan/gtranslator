/*
 * (C) 2001-2003 	Fatih Demir <kabalak@kabalak.net>
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

//#include "dialogs.h"
//#include "gui.h"
#include "nautilus-string.h"
//#include "parse.h"
#include "runtime-config.h"
#include "utils.h"
#include "utils_gui.h"

#include <libgnome/gnome-url.h>

#include <libgnomeui/libgnomeui.h>
#include <libgnomevfs/gnome-vfs.h>
#include <glib/gi18n.h>
#include <glade/glade.h>



/*
 * Check if the given file is already opened by gtranslator.
 */
gboolean
gtranslator_utils_reopen_if_already_open(const gchar *filename)
{
	gchar *resultfilename;

	g_return_val_if_fail(filename!=NULL, FALSE);

//	resultfilename=gtranslator_config_get_string("runtime/filename");

	/*
	 * Test if we've got a filename and then test it for equality with our
	 *  currently in another instance opened po file.
	 */
	if(resultfilename && (!nautilus_strcasecmp(resultfilename, filename)) &&
		(strlen(resultfilename)==strlen(filename)))
	{
		gint reply;
		//FIXME:  reply = gtranslator_already_open_dialog(NULL, (gpointer)filename);
		if(reply == GTK_RESPONSE_NO)
		return FALSE;
	}

	/*
	 * Assume we want to open it
	 */
	return TRUE;
}

/*
 * Check for a needed program -- returns FALSE on failure (how logical, not ,-)).
 */
gboolean gtranslator_utils_check_program(const gchar *program_name,
	const gint type_int)
{
	g_return_val_if_fail(program_name!=NULL, FALSE);
	
	if(!g_find_program_in_path(program_name))
	{
		gchar *warning_message;

		if(type_int==0)
		{
			warning_message=g_strdup_printf(
				_("The necessary decompression program `%s' is not installed!"), program_name);
		}
		else
		{
			warning_message=g_strdup_printf(
				_("The necessary compression program `%s' is not installed!"), program_name);
		}

		//gnome_app_warning(GNOME_APP(gtranslator_application), warning_message);
		g_free(warning_message);

		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


/**
 * gtranslator_utils_get_glade_widgets:
 * @filename: the path to the glade file
 * @root_node: the root node in the glade file
 * @error_widget: a pointer were a #GtkLabel
 * @widget_name: the name of the first widget
 * @...: a pointer were the first widget is returned, followed by more
 *       name / widget pairs and terminated by NULL.
 *
 * This function gets the requested widgets from a glade file. In case
 * of error it returns FALSE and sets error_widget to a GtkLabel containing
 * the error message to display.
 *
 * Returns FALSE if an error occurs, TRUE on success.
 */
gboolean
gtranslator_utils_get_glade_widgets (const gchar *filename,
				     const gchar *root_node,
				     GtkWidget **error_widget,
				     const gchar *widget_name,
				     ...)
{
	GtkWidget *label;
	GladeXML *gui;
	va_list args;
	const gchar *name;
	gchar *msg;
	gchar *filename_markup;
	gchar *msg_plain;
	gboolean ret = TRUE;

	g_return_val_if_fail (filename != NULL, FALSE);
	g_return_val_if_fail (error_widget != NULL, FALSE);
	g_return_val_if_fail (widget_name != NULL, FALSE);

	*error_widget = NULL;

	gui = glade_xml_new (filename, root_node, NULL);
	if (!gui)
	{
		filename_markup = g_markup_printf_escaped ("<i>%s</i>", filename);
		msg_plain = g_strdup_printf (_("Unable to find file %s."),
				filename_markup);
		msg = g_strconcat ("<span size=\"large\" weight=\"bold\">",
				msg_plain, "</span>\n\n",
				_("Please check your installation."), NULL);
		label = gtk_label_new (msg);

		gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
		gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
		
		g_free (filename_markup);
		g_free (msg_plain);
		g_free (msg);

		gtk_misc_set_padding (GTK_MISC (label), 5, 5);
 		
		*error_widget = label;

		return FALSE;
	}

	va_start (args, widget_name);
	for (name = widget_name; name; name = va_arg (args, const gchar *) )
	{
		GtkWidget **wid;

		wid = va_arg (args, GtkWidget **);
		*wid = glade_xml_get_widget (gui, name);
		if (*wid == NULL)
		{
			g_warning ("Cannot find widget '%s' inside file '%s'.",
				   name,
				   filename);
				   
			filename_markup = g_markup_printf_escaped ("<i>%s</i>", filename);
			msg_plain = g_strdup_printf (
					_("Unable to find the required widgets inside file %s."),
					filename_markup);
			msg = g_strconcat ("<span size=\"large\" weight=\"bold\">",
					msg_plain, "</span>\n\n",
					_("Please check your installation."), NULL);
			label = gtk_label_new (msg);

			gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
			gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
			
			g_free (filename_markup);
			g_free (msg_plain);
			g_free (msg);

			gtk_misc_set_padding (GTK_MISC (label), 5, 5);
 			
			*error_widget = label;

			ret = FALSE;

			break;
		}
	}
	va_end (args);

	g_object_unref (gui);

	return ret;
}

static gboolean
is_valid_scheme_character (gchar c)
{
	return g_ascii_isalnum (c) || c == '+' || c == '-' || c == '.';
}

static gboolean
has_valid_scheme (const gchar *uri)
{
	const gchar *p;

	p = uri;

	if (!is_valid_scheme_character (*p)) {
		return FALSE;
	}

	do {
		p++;
	} while (is_valid_scheme_character (*p));

	return *p == ':';
}

gboolean
gtranslator_utils_is_valid_uri (const gchar *uri)
{
	const guchar *p;

	if (uri == NULL)
		return FALSE;

	if (!has_valid_scheme (uri))
		return FALSE;

	/* We expect to have a fully valid set of characters */
	for (p = (const guchar *)uri; *p; p++) {
		if (*p == '%')
		{
			++p;
			if (!g_ascii_isxdigit (*p))
				return FALSE;

			++p;		
			if (!g_ascii_isxdigit (*p))
				return FALSE;
		}
		else
		{
			if (*p <= 32 || *p >= 128)
				return FALSE;
		}
	}

	return TRUE;
}

gchar *
gtranslator_utils_make_canonical_uri_from_shell_arg (const gchar *str)
{	
	gchar *uri;
	gchar *canonical_uri;

	g_return_val_if_fail (str != NULL, NULL);
	g_return_val_if_fail (*str != '\0', NULL);
	
	/* Note for the future: 
	 *
	 * <federico> paolo: and flame whoever tells 
	 * you that file:///gnome/test_files/hëllò 
	 * doesn't work --- that's not a valid URI
	 *
	 * <paolo> federico: well, another solution that 
	 * does not requires patch to _from_shell_args 
	 * is to check that the string returned by it 
	 * contains only ASCII chars
	 * <federico> paolo: hmmmm, isn't there 
	 * gnome_vfs_is_uri_valid() or something?
	 * <paolo>: I will use gedit_utils_is_valid_uri ()
	 *
	 */
	 
	uri = gnome_vfs_make_uri_from_shell_arg (str);
	canonical_uri = gnome_vfs_make_uri_canonical (uri);
	g_free (uri);
	
	/* g_print ("URI: %s\n", canonical_uri); */
	
	if (gtranslator_utils_is_valid_uri (canonical_uri))
		return canonical_uri;
	
	return NULL;
}

/**
 * gtranslator_utils_drop_get_uris:
 * @selection_data: the #GtkSelectionData from drag_data_received
 * @info: the info from drag_data_received
 *
 * Create a list of valid uri's from a uri-list drop.
 * 
 * Return value: a string array which will hold the uris or NULL if there 
 *		 were no valid uris. g_strfreev should be used when the 
 *		 string array is no longer used
 */
gchar **
gtranslator_utils_drop_get_uris (GtkSelectionData *selection_data)
{
	gchar **uris;
	gint i;
	gint p = 0;
	gchar **uri_list;

	uris = g_uri_list_extract_uris ((gchar *) selection_data->data);
	uri_list = g_new0(gchar *, g_strv_length (uris) + 1);

	for (i = 0; uris[i] != NULL; i++)
	{
		gchar *uri;
		
		uri = gtranslator_utils_make_canonical_uri_from_shell_arg (uris[i]);
		
		/* Silently ignore malformed URI/filename */
		if (uri != NULL)
			uri_list[p++] = uri;
	}

	if (*uri_list == NULL)
	{
		g_free(uri_list);
		return NULL;
	}

	return uri_list;
}

gchar *
gtranslator_utils_escape_search_text (const gchar* text)
{
	GString *str;
	gint length;
	const gchar *p;
 	const gchar *end;

	if (text == NULL)
		return NULL;

    	length = strlen (text);

	/* no escape when typing.
	 * The short circuit works only for ascii, but we only
	 * care about not escaping a single '\' */
	if (length == 1)
		return g_strdup (text);

	str = g_string_new ("");

	p = text;
  	end = text + length;

  	while (p != end)
    	{
      		const gchar *next;
      		next = g_utf8_next_char (p);

		switch (*p)
        	{
       			case '\n':
          			g_string_append (str, "\\n");
          			break;
			case '\r':
          			g_string_append (str, "\\r");
          			break;
			case '\t':
          			g_string_append (str, "\\t");
          			break;
			case '\\':
          			g_string_append (str, "\\\\");
          			break;
        		default:
          			g_string_append_len (str, p, next - p);
          			break;
        	}

      		p = next;
    	}

	return g_string_free (str, FALSE);
}

gchar *
gtranslator_utils_unescape_search_text (const gchar *text)
{
	GString *str;
	gint length;
	gboolean drop_prev = FALSE;
	const gchar *cur;
	const gchar *end;
	const gchar *prev;
	
	if (text == NULL)
		return NULL;

	length = strlen (text);

	str = g_string_new ("");

	cur = text;
	end = text + length;
	prev = NULL;
	
	while (cur != end) 
	{
		const gchar *next;
		next = g_utf8_next_char (cur);

		if (prev && (*prev == '\\')) 
		{
			switch (*cur) 
			{
				case 'n':
					str = g_string_append (str, "\n");
				break;
				case 'r':
					str = g_string_append (str, "\r");
				break;
				case 't':
					str = g_string_append (str, "\t");
				break;
				case '\\':
					str = g_string_append (str, "\\");
					drop_prev = TRUE;
				break;
				default:
					str = g_string_append (str, "\\");
					str = g_string_append_len (str, cur, next - cur);
				break;
			}
		} 
		else if (*cur != '\\') 
		{
			str = g_string_append_len (str, cur, next - cur);
		} 
		else if ((next == end) && (*cur == '\\')) 
		{
			str = g_string_append (str, "\\");
		}
		
		if (!drop_prev)
		{
			prev = cur;
		}
		else 
		{
			prev = NULL;
			drop_prev = FALSE;
		}

		cur = next;
	}

	return g_string_free (str, FALSE);
}
