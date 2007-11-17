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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "comment.h"
#include "comment-panel.h"
#include "msg.h"
#include "notebook.h"
#include "panel.h"
#include "po.h"
#include "tab.h"
#include "window.h"


#include <glib/gi18n-lib.h>


#define GTR_COMMENT_GET_PRIVATE(object) \
				(G_TYPE_INSTANCE_GET_PRIVATE ((object),	\
				GTR_TYPE_COMMENT,		\
				GtranslatorCommentPrivate))

G_DEFINE_TYPE(GtranslatorComment, gtranslator_comment, G_TYPE_OBJECT)


static void
gtranslator_comment_init (GtranslatorComment *comment)
{
}

static void
gtranslator_comment_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_comment_parent_class)->finalize (object);
}

static void
showed_message_cb(GtranslatorTab *tab,
		  GtranslatorCommentPanel *panel)
{
	GtranslatorPo *po;
	GList *msg;
	
	po = gtranslator_tab_get_po(tab);
	msg = gtranslator_po_get_current_message(po);
	
	gtranslator_comment_panel_set_text(panel,
					   gtranslator_msg_get_extracted_comments(GTR_MSG(msg->data)));
}

static void
create_comment_panel (GtkNotebook *notebook,
		      GtkWidget   *child,
		      guint        page_num,
		      GtranslatorWindow *window)
{
	GtkWidget *table;
	GtkWidget *image;
	GtranslatorPo *po;
	GtranslatorPanel *panel;
	
	po = gtranslator_tab_get_po(GTR_TAB(child));
	
	g_return_if_fail(po != NULL);
	
	table = gtranslator_comment_panel_new();
		
	panel = gtranslator_tab_get_panel(GTR_TAB(child));
	
	image = gtk_image_new_from_stock(GTK_STOCK_INDEX,
					 GTK_ICON_SIZE_SMALL_TOOLBAR);
	
	gtranslator_panel_add_item(panel,
				   table,
				   _("Comment"),
				   image);
	
	g_signal_connect(child, "showed-message",
			 G_CALLBACK(showed_message_cb), table);

	gtk_widget_show_all (table);
}

void
comment_activate (GtranslatorWindow *window)
{
	GtranslatorNotebook *notebook;
	
	notebook = gtranslator_window_get_notebook(window);
	
	g_signal_connect(GTK_NOTEBOOK(notebook),
			 "page-added",
			 G_CALLBACK(create_comment_panel), window);

}

void
comment_deactivate(GtranslatorWindow *window)
{
	GtranslatorPanel *panel;


	/*panel = gtranslator_window_get_side_panel (window);
	gtranslator_panel_remove_item (panel, data->panel);

	g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);*/
}

static void
gtranslator_comment_class_init (GtranslatorCommentClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	//GeditPluginClass *plugin_class = GEDIT_PLUGIN_CLASS (klass);

	object_class->finalize = gtranslator_comment_finalize;

	/*plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;*/
}
