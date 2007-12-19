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
 *     Based on Gedit alternate_lang plugin
 */

#ifndef __GTR_ALTERNATE_LANG_H__
#define __GTR_ALTERNATE_LANG_H__

#include <glib.h>
#include <glib-object.h>
#include "window.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_ALTERNATE_LANG		(gtranslator_alternate_lang_get_type ())
#define GTR_ALTERNATE_LANG(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_MESSAGEALTERNATE_LANG, GtranslatorAlternateLang))
#define GTR_ALTERNATE_LANG_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_ALTERNATE_LANG, GtranslatorAlternateLangClass))
#define GTR_IS_ALTERNATE_LANG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_ALTERNATE_LANG))
#define GTR_IS_ALTERNATE_LANG_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_ALTERNATE_LANG))
#define GTR_ALTERNATE_LANG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_ALTERNATE_LANG_PLUGIN, GtranslatorAlternateLangClass))

/* Private structure type */
typedef struct _GtranslatorAlternateLangPrivate	GtranslatorAlternateLangPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorAlternateLang		GtranslatorAlternateLang;

struct _GtranslatorAlternateLang
{
	GObject parent_instance;
};

/*
 * Class definition
 */
typedef struct _GtranslatorAlternateLangClass	GtranslatorAlternateLangClass;

struct _GtranslatorAlternateLangClass
{
	GObjectClass parent_class;
};

/*
 * Public methods
 */
GType	gtranslator_alternate_lang_get_type	(void) G_GNUC_CONST;
GType	gtranslator_alternate_lang_register_type  (GTypeModule * module);
/* All the plugins must implement this function */
//G_MODULE_EXPORT GType register_gedit_plugin (GTypeModule *module);

void 	alternate_lang_activate (GtranslatorWindow *window);
void 	alternate_lang_deactivate	(GtranslatorWindow *window);

G_END_DECLS

#endif /* __GTR_ALTERNATE_LANG_H__ */
