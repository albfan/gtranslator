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

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "egg-toolbars-model.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_APPLICATION		(gtranslator_application_get_type ())
#define GTR_APPLICATION(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_APPLICATION, GtranslatorApplication))
#define GTR_APPLICATION_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_APPLICATION, GtranslatorApplicationClass))
#define GTR_IS_APPLICATION(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_APPLICATION))
#define GTR_IS_APPLICATION_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_APPLICATION))
#define GTR_APPLICATION_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_APPLIAPPLICATION, GtranslatorApplicationClass))

#define GTR_APP			(gtranslator_application_get_instance())

/*
 * Main object structure
 */
typedef struct _GtranslatorApplication		GtranslatorApplication;


/*
 * Class definition
 */
typedef struct _GtranslatorApplicationClass	GtranslatorApplicationClass;



/*
 * Public methods
 */
GType		 gtranslator_application_get_type	   (void) G_GNUC_CONST;
GtranslatorApplication	*gtranslator_application_get_instance	           (void);


EggToolbarsModel *gtranslator_application_get_toolbars_model  (GtranslatorApplication   *application);
void              gtranslator_application_save_toolbars_model (GtranslatorApplication   *application);

G_END_DECLS

#endif /* __APPLICATION_H__ */