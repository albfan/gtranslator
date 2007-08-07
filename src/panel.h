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
 *
 *
 *	Based in gedit-panel widget of Paolo Maggi 
 */
 

#ifndef __GTR_PANEL_H__
#define __GTR_PANEL_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_PANEL			(gtranslator_panel_get_type())
#define GTR_PANEL(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), GTR_TYPE_PANEL, GtranslatorPanel))
#define GTR_PANEL_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), GTR_TYPE_PANEL, GtranslatorPanelClass))
#define GTR_IS_PANEL(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), GTR_TYPE_PANEL))
#define GTR_IS_PANEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTR_TYPE_PANEL))
#define GTR_PANEL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), GTR_TYPE_PANEL, GtranslatorPanelClass))

/* Private structure type */
typedef struct _GtranslatorPanelPrivate GtranslatorPanelPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorPanel GtranslatorPanel;

struct _GtranslatorPanel 
{
	GtkVBox vbox;

	/*< private > */
	GtranslatorPanelPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorPanelClass GtranslatorPanelClass;

struct _GtranslatorPanelClass 
{
	GtkVBoxClass parent_class;

	void (* item_added)     (GtranslatorPanel *panel,
				 GtkWidget        *item);
	void (* item_removed)   (GtranslatorPanel *panel,
				 GtkWidget        *item);

	/* Keybinding signals */
	void (* close)          (GtranslatorPanel *panel);
	//void (* focus_document) (GtranslatorPanel *panel);

	/* Padding for future expansion */
	void (*_gtranslator_reserved1) (void);
	void (*_gtranslator_reserved2) (void);
	void (*_gtranslator_reserved3) (void);
	void (*_gtranslator_reserved4) (void);	
};

/*
 * Public methods
 */
GType 		 gtranslator_panel_get_type 			(void) G_GNUC_CONST;

GtkWidget 	*gtranslator_panel_new 				(GtkOrientation	    orientation);

void		gtranslator_panel_add_item			(GtranslatorPanel  *panel,
								 GtkWidget         *item,
								 const gchar       *name,
								 GtkWidget         *image);

void		 gtranslator_panel_add_item_with_stock_icon	(GtranslatorPanel *panel,
								 GtkWidget        *item,
								 const gchar      *name,
								 const gchar      *stock_id);

gboolean	 gtranslator_panel_remove_item			(GtranslatorPanel *panel,
								 GtkWidget        *item);

gboolean	 gtranslator_panel_activate_item 		(GtranslatorPanel *panel,
								 GtkWidget        *item);

gboolean	 gtranslator_panel_item_is_active 		(GtranslatorPanel *panel,
								 GtkWidget        *item);

GtkOrientation	 gtranslator_panel_get_orientation		(GtranslatorPanel *panel);

gint		 gtranslator_panel_get_n_items			(GtranslatorPanel *panel);


/*
 * Non exported functions
 */
gint		 _gtranslator_panel_get_active_item_id		(GtranslatorPanel *panel);

void		 _gtranslator_panel_set_active_item_by_id	(GtranslatorPanel *panel,
								 gint		   id);

G_END_DECLS

#endif  /* __GTR_PANEL_H__  */
