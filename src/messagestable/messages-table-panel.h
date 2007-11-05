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

#ifndef __MESSAGES_TABLE_PANEL_H__
#define __MESSAGES_TABLE_PANEL_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_MESSAGES_TABLE_PANEL		(gtranslator_messages_table_panel_get_type ())
#define GTR_MESSAGES_TABLE_PANEL(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_MESSAGES_TABLE_PANEL, GtranslatorMessagesTablePanel))
#define GTR_MESSAGES_TABLE_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_MESSAGES_TABLE_PANEL, GtranslatorMessagesTablePanelClass))
#define GTR_IS_MESSAGES_TABLE_PANEL(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_MMESSAGES_TABLE_PANEL))
#define GTR_IS_MESSAGES_TABLE_PANEL_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_MESSAGES_TABLE_PANEL))
#define GTR_MESSAGES_TABLE_PANEL_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_MESSAGES_TABLE_PANEL, GtranslatorMessagesTablePanelClass))

/* Private structure type */
typedef struct _GtranslatorMessagesTablePanelPrivate	GtranslatorMessagesTablePanelPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorMessagesTablePanel		GtranslatorMessagesTablePanel;

struct _GtranslatorMessagesTablePanel
{
	GtkVBox parent_instance;
	
	/*< private > */
	GtranslatorMessagesTablePanelPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorMessagesTablePanelClass	GtranslatorMessagesTablePanelClass;

struct _GtranslatorMessagesTablePanelClass
{
	GtkVBoxClass parent_class;
};

/*
 * Public methods
 */
GType		 gtranslator_messages_table_panel_get_type	  (void) G_GNUC_CONST;

GType		 gtranslator_messages_table_panel_register_type   (GTypeModule * module);

GtkWidget	*gtranslator_messages_table_panel_new	          (void);

void             gtranslator_messages_table_panel_populate        (GtranslatorMessagesTablePanel *table, 
								   GList *messages);

G_END_DECLS

#endif /* __MESSAGES_TABLE_PANEL_H__ */
