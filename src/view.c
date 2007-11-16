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

#include "draw-spaces.h"
#include "prefs-manager.h"
#include "view.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <gtksourceview/gtksourcelanguagemanager.h>


//#undef HAVE_GTKSPELL
#ifdef HAVE_GTKSPELL
#include <gtkspell/gtkspell.h>
#endif

#undef HAVE_SPELL_CHECK
#ifdef HAVE_SPELL_CHECK
#include <gtkspellcheck/client.h>
#include <gtkspellcheck/manager.h>
#include <gtkspellcheck/textviewclient.h>
#endif

#define GTR_VIEW_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 	(object),	\
						 	GTR_TYPE_VIEW,     \
						 	GtranslatorViewPrivate))

G_DEFINE_TYPE(GtranslatorView, gtranslator_view, GTK_TYPE_SOURCE_VIEW)

struct _GtranslatorViewPrivate
{
	GtkSourceBuffer *buffer;
	
#ifdef HAVE_GTKSPELL
	GtkSpell *spell;
#endif
	
#ifdef HAVE_SPELL_CHECK
	GtkSpellCheckClient *client;
	GtkSpellCheckManager *manager;
#endif
};


#ifdef HAVE_GTKSPELL
static void
gtranslator_attach_gtkspell(GtranslatorView *view)
{
	gint i;
	GError *error = NULL;
	gchar *errortext = NULL;
	view->priv->spell = NULL;
	
	view->priv->spell = 
		gtkspell_new_attach(GTK_TEXT_VIEW(view), NULL, &error);
	if (view->priv->spell == NULL) 
	{
		g_warning(_("gtkspell error: %s\n"), error->message);
		errortext = g_strdup_printf(_("GtkSpell was unable to initialize.\n %s"),
					    error->message);
		g_warning(errortext);
		
		g_error_free(error);
		g_free(errortext);
	}
}
#endif

#ifdef HAVE_SPELL_CHECK
static void
gtranslator_attach_spellcheck(GtranslatorView *view)
{
	view->priv->client = GTK_SPELL_CHECK_CLIENT(gtk_spell_check_text_view_client_new(GTK_TEXT_VIEW(view)));
	view->priv->manager = gtk_spell_check_manager_new(NULL, TRUE);
	
	gtk_spell_check_manager_attach(view->priv->manager,
				       view->priv->client);
}
#endif

	       
static void
gtranslator_view_init (GtranslatorView *view)
{
	GtkSourceLanguageManager *lm;
	GtkSourceLanguage *lang;
	GPtrArray *dirs;
	gchar **langs;
	const gchar * const *temp;
	gint i;
	
	view->priv = GTR_VIEW_GET_PRIVATE (view);
	
	GtranslatorViewPrivate *priv = view->priv;
	
	lm = gtk_source_language_manager_new();
	dirs = g_ptr_array_new();
	
	for(temp = gtk_source_language_manager_get_search_path(lm);
	    temp != NULL && *temp != NULL;
	    ++temp)
		g_ptr_array_add(dirs, g_strdup(*temp));
		
	g_ptr_array_add(dirs, g_strdup(DATADIR));
	g_ptr_array_add(dirs, NULL);
	langs = (gchar **)g_ptr_array_free(dirs, FALSE);

	
	gtk_source_language_manager_set_search_path(lm, langs);
	lang = gtk_source_language_manager_get_language(lm, "gtranslator");
	g_strfreev(langs);
		
	priv->buffer = gtk_source_buffer_new_with_language(lang);
	
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(view), GTK_TEXT_BUFFER(priv->buffer));
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD);
	
	//Set syntax highlight according to preferences
	gtk_source_buffer_set_highlight_syntax(priv->buffer, gtranslator_prefs_manager_get_highlight());
	
	//Set dot char according to preferences
	
	if(gtranslator_prefs_manager_get_use_dot_char())
		gtranslator_view_enable_dot_char(view, TRUE);
	
	/*
	 *  Set fonts according to preferences 
	 */
	if (gtranslator_prefs_manager_get_own_fonts ())
	{
		gchar *editor_font;

		editor_font = g_strdup(gtranslator_prefs_manager_get_text_font ());

		gtranslator_view_set_font (view, FALSE, editor_font);

		g_free (editor_font);
	}
	else
	{
		gtranslator_view_set_font (view, TRUE, NULL);
	}
}

static void
gtranslator_view_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_view_parent_class)->finalize (object);
}

static void
gtranslator_view_class_init (GtranslatorViewClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorViewPrivate));

	object_class->finalize = gtranslator_view_finalize;
}

GtkWidget *
gtranslator_view_new (void)
{
	GtkWidget *view;
	
	view = GTK_WIDGET (g_object_new (GTR_TYPE_VIEW, NULL));
	gtk_widget_show_all(view);
	return view;
}


/**
 * gtranslator_view_enable_spell_check:
 * @view: a #GtranslatorView
 * @enable: TRUE if you want enable the spellcheck
 * 
 * Enables the spellcheck
 **/
void
gtranslator_view_enable_spell_check(GtranslatorView *view,
				    gboolean enable)
{
	if(enable)
	{
#ifdef HAVE_GTKSPELL
		gtranslator_attach_gtkspell(view);
#endif
#ifdef HAVE_SPELL_CHECK
		if(!view->priv->manager)
			gtranslator_attach_spellcheck(view);
		else
			gtk_spell_check_manager_set_active(view->priv->manager,
							   TRUE);
#endif
	}
	else
	{
#ifdef HAVE_GTKSPELL
		if(!view->priv->spell)
			return;
		gtkspell_detach(view->priv->spell);
#endif
#ifdef HAVE_SPELL_CHECK
		if(!view->priv->manager)
			return;
		gtk_spell_check_manager_set_active(view->priv->manager,
						   FALSE);
#endif
	}
}

/**
 * gtranslator_view_enable_dot_char:
 * @view: a #GtranslatorView
 * @enable: TRUE if you want to enable special chars for white spaces
 *
 * Enables special chars for white spaces including \n and \t
**/
void
gtranslator_view_enable_dot_char(GtranslatorView *view,
				 gboolean enable)
{
	g_return_if_fail(GTR_IS_VIEW(view));
	
	if(enable)
		g_signal_connect(view, "event-after",
				 G_CALLBACK(on_event_after), NULL);
	else
		g_signal_handlers_disconnect_by_func(view,
						     G_CALLBACK(on_event_after),
						     NULL);
	
	/*It's neccessary redraw the widget when you connect or disconnect the signal*/
	gtk_widget_queue_draw (GTK_WIDGET (view));
}

void
gtranslator_view_cut_clipboard (GtranslatorView *view)
{
	GtkTextBuffer *buffer;
	GtkClipboard *clipboard;

	g_return_if_fail (GTR_IS_VIEW (view));

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	g_return_if_fail (buffer != NULL);

	clipboard = gtk_widget_get_clipboard (GTK_WIDGET (view),
					      GDK_SELECTION_CLIPBOARD);

	/* FIXME: what is default editability of a buffer? */
  	gtk_text_buffer_cut_clipboard (buffer,
  				       clipboard,
				       gtk_text_view_get_editable(
						GTK_TEXT_VIEW (view)));
  	
	gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (view),
				      gtk_text_buffer_get_insert (buffer),
				      0.0,
				      FALSE,
				      0.0,
				      0.0);
}

void
gtranslator_view_copy_clipboard (GtranslatorView *view)
{
	GtkTextBuffer *buffer;
	GtkClipboard *clipboard;

	g_return_if_fail (GTR_IS_VIEW (view));

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	g_return_if_fail (buffer != NULL);

	clipboard = gtk_widget_get_clipboard (GTK_WIDGET (view),
					      GDK_SELECTION_CLIPBOARD);

  	gtk_text_buffer_copy_clipboard (buffer, clipboard);

	/* on copy do not scroll, we are already on screen */
}

void
gtranslator_view_paste_clipboard (GtranslatorView *view)
{
  	GtkTextBuffer *buffer;
	GtkClipboard *clipboard;

	g_return_if_fail (GTR_IS_VIEW (view));

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	g_return_if_fail (buffer != NULL);

	clipboard = gtk_widget_get_clipboard (GTK_WIDGET (view),
					      GDK_SELECTION_CLIPBOARD);

	/* FIXME: what is default editability of a buffer? */
  	gtk_text_buffer_paste_clipboard (buffer,
					 clipboard,
					 NULL,
					 gtk_text_view_get_editable(
						GTK_TEXT_VIEW (view)));

	gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (view),
				      gtk_text_buffer_get_insert (buffer),
				      0.0,
				      FALSE,
				      0.0,
				      0.0);
}

/**
 * gtranslator_view_set_font:
 * @view: a #GtranslatorView
 * @def: TRUE if you want to use the default font
 * @font_name: The name of the font you want to use in the #GtranslatorView
 * 
 * Sets the #GtranslatorView font.
 **/
void
gtranslator_view_set_font (GtranslatorView *view, 
			   gboolean     def, 
			   const gchar *font_name)
{
	PangoFontDescription *font_desc = NULL;

	g_return_if_fail (GTR_IS_VIEW (view));

	if (def)
		font_name = g_strdup(GPM_DEFAULT_TEXT_FONT);

	g_return_if_fail (font_name != NULL);

	font_desc = pango_font_description_from_string (font_name);
	g_return_if_fail (font_desc != NULL);

	gtk_widget_modify_font (GTK_WIDGET (view), font_desc);

	pango_font_description_free (font_desc);	
}
