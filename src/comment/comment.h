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

#ifndef __GTR_COMMENT_H__
#define __GTR_COMMENT_H__

#include <glib.h>
#include <glib-object.h>
#include "window.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_COMMENT		(gtranslator_comment_get_type ())
#define GTR_COMMENT(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_MESSAGECOMMENT, GtranslatorComment))
#define GTR_COMMENT_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_COMMENT, GtranslatorCommentClass))
#define GTR_IS_COMMENT(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_COMMENT))
#define GTR_IS_COMMENT_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_COMMENT))
#define GTR_COMMENT_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_COMMENT_PLUGIN, GtranslatorCommentClass))

/* Private structure type */
typedef struct _GtranslatorCommentPrivate	GtranslatorCommentPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorComment		GtranslatorComment;

struct _GtranslatorComment
{
	GObject parent_instance;
};

/*
 * Class definition
 */
typedef struct _GtranslatorCommentClass	GtranslatorCommentClass;

struct _GtranslatorCommentClass
{
	GObjectClass parent_class;
};

/*
 * Public methods
 */
GType	gtranslator_comment_get_type	(void) G_GNUC_CONST;
GType	gtranslator_comment_register_type  (GTypeModule * module);
/* All the plugins must implement this function */
//G_MODULE_EXPORT GType register_gedit_plugin (GTypeModule *module);

void 	comment_activate (GtranslatorWindow *window);
void 	comment_deactivate	(GtranslatorWindow *window);

G_END_DECLS

#endif /* __GTR_COMMENT_H__ */
