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
	/*
	 * Use instant spell checking via gtkspell only if the corresponding
	 *  setting in the preferences is set.
	 */
	/*if(GtrPreferences.instant_spell_check)
	{*/
		/*
		 * Start up gtkspell if not already done.
		 */ 
		GError *error = NULL;
		gchar *errortext = NULL;
		view->priv->spell = NULL;
		
		view->priv->spell = 
			gtkspell_new_attach(GTK_TEXT_VIEW(view), NULL, &error);
		if (view->priv->spell == NULL) 
		{
			g_printf(_("gtkspell error: %s\n"), error->message);
			errortext = g_strdup_printf(_("GtkSpell was unable to initialize.\n %s"),
						    error->message);
			g_error_free(error);
		}
		
	//}
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
	gchar *dir;
	
	view->priv = GTR_VIEW_GET_PRIVATE (view);
	
	GtranslatorViewPrivate *priv = view->priv;
	
	lm = gtk_source_language_manager_new();
	
	dir = g_strdup(DATADIR);
	gtk_source_language_manager_set_search_path(lm, &dir);
	lang = gtk_source_language_manager_get_language(lm, "gettext-translation2");
	g_free(dir);
	
	priv->buffer = gtk_source_buffer_new_with_language(lang);
	
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(view), GTK_TEXT_BUFFER(priv->buffer));
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD);
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
