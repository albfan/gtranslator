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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "comment.h"
#include "tab.h"

#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define GTR_COMMENT_PANEL_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
						 (object),		       \
						 GTR_TYPE_COMMENT_PANEL,     \
						 GtranslatorCommentPanelPrivate))

G_DEFINE_TYPE(GtranslatorCommentPanel, gtranslator_comment_panel, GTK_TYPE_VBOX)


struct _GtranslatorCommentPanelPrivate
{
	GtkLabel *comment;
	GtranslatorTab *tab;
};

static void
showed_message_cb(GtranslatorTab *tab,
		  GtranslatorMsg *msg,
		  GtranslatorCommentPanel *panel)
{
	gtranslator_comment_panel_set_text(panel,
					   gtranslator_msg_get_extracted_comments(msg),
					   gtranslator_msg_get_comment(msg));
}

static void
gtranslator_comment_panel_draw(GtranslatorCommentPanel *panel)
{
	GtranslatorCommentPanelPrivate *priv = panel->priv;
	GtkWidget *comments_scrolled_window;
	GtkWidget *comments_viewport;
	
	/*
	 * Set up the scrolling window for the comments display
	 */	
	comments_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(comments_scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(panel), comments_scrolled_window, TRUE, TRUE, 0);
	
	comments_viewport = gtk_viewport_new(NULL, NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(comments_scrolled_window),
					      comments_viewport);
	
	priv->comment=GTK_LABEL(gtk_label_new(""));
	gtk_container_add(GTK_CONTAINER(comments_viewport), GTK_WIDGET(priv->comment));
}


static void
gtranslator_comment_panel_init (GtranslatorCommentPanel *panel)
{
	panel->priv = GTR_COMMENT_PANEL_GET_PRIVATE (panel);
	
	gtranslator_comment_panel_draw(panel);
}

static void
gtranslator_comment_panel_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_comment_panel_parent_class)->finalize (object);
}

static void
gtranslator_comment_panel_class_init (GtranslatorCommentPanelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorCommentPanelPrivate));

	object_class->finalize = gtranslator_comment_panel_finalize;
}

GtkWidget *
gtranslator_comment_panel_new (GtkWidget *tab)
{
	GtranslatorCommentPanel *comment;
	comment = g_object_new (GTR_TYPE_COMMENT_PANEL, NULL);
	
	comment->priv->tab = GTR_TAB(tab);
	g_signal_connect(tab,
			 "showed-message",
			 G_CALLBACK(showed_message_cb),
			 comment);
	
	return GTK_WIDGET(comment);
}

void
gtranslator_comment_panel_set_text(GtranslatorCommentPanel *panel,
				   const gchar *extracted_comments,
				   const gchar *comments)
{
	gchar *comment;
	
	comment = g_strdup_printf("%s\n%s",
				  extracted_comments,
				  comments);
	
	gtk_label_set_text(panel->priv->comment,
			   comment);
	
	g_free(comment);
}
