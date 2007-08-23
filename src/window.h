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

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "notebook.h"
#include "tab.h"
#include "panel.h"
#include "view.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_WINDOW		(gtranslator_window_get_type ())
#define GTR_WINDOW(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_WINDOW, GtranslatorWindow))
#define GTR_WINDOW_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_WINDOW, GtranslatorWindowClass))
#define GTR_IS_WINDOW(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_WINDOW))
#define GTR_IS_WINDOW_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_WINDOW))
#define GTR_WINDOW_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_WINDOW, GtranslatorWindowClass))

/* Private structure type */
typedef struct _GtranslatorWindowPrivate	GtranslatorWindowPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorWindow		GtranslatorWindow;

struct _GtranslatorWindow
{
	GtkWindow parent_instance;
	
	/*< private > */
	GtranslatorWindowPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorWindowClass	GtranslatorWindowClass;

struct _GtranslatorWindowClass
{
	GtkWindowClass parent_class;
};

/*
 * Public methods
 */
GType		  gtranslator_window_get_type	      (void) G_GNUC_CONST;

GType		  gtranslator_window_register_type    (GTypeModule * module);

GtranslatorTab   *gtranslator_window_create_tab       (GtranslatorWindow *window,
						       GtranslatorPo *po);

GtranslatorTab   *gtranslator_window_get_active_tab   (GtranslatorWindow *window);

GtranslatorNotebook  *gtranslator_window_get_notebook     (GtranslatorWindow *window);

GtranslatorPanel *gtranslator_window_get_side_panel   (GtranslatorWindow *window);

GtkStatusbar 	 *gtranslator_window_get_statusbar    (GtranslatorWindow *window);

GtkUIManager     *gtranslator_window_get_ui_manager   (GtranslatorWindow *window);

GtranslatorView  *gtranslator_window_get_active_view  (GtranslatorWindow *window);

G_END_DECLS

#endif /* __WINDOW_H__ */
