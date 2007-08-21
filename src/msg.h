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
 *     MERCHANMSGILITY or FITNESS FOR A PARTICULAR PURMSGSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __MSG_H__
#define __MSG_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gettext-po.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_MSG		(gtranslator_msg_get_type ())
#define GTR_MSG(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_MSG, GtranslatorMsg))
#define GTR_MSG_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_MSG, GtranslatorMsgClass))
#define GTR_IS_MSG(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_MSG))
#define GTR_IS_MSG_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_MSG))
#define GTR_MSG_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_MSG, GtranslatorMsgClass))

/* Private structure type */
typedef struct _GtranslatorMsgPrivate	GtranslatorMsgPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorMsg		GtranslatorMsg;

struct _GtranslatorMsg
{
	GObject parent_instance;
	
	/*< private > */
	GtranslatorMsgPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorMsgClass	GtranslatorMsgClass;

struct _GtranslatorMsgClass
{
	GObjectClass parent_class;
};

/*
 * Public methods
 */
GType		  gtranslator_msg_get_type	       (void) G_GNUC_CONST;

GType		  gtranslator_msg_register_type	       (GTypeModule * module);

GtranslatorMsg   *gtranslator_msg_new                  (void);

po_message_t      gtranslator_msg_get_message          (GtranslatorMsg *msg);

void              gtranslator_msg_set_message          (GtranslatorMsg *msg,
							po_message_t message);

gboolean          gtranslator_msg_is_translated        (GtranslatorMsg *msg);

gboolean          gtranslator_msg_is_fuzzy             (GtranslatorMsg *msg);

const gchar      *gtranslator_msg_get_msgid            (GtranslatorMsg *msg);

const gchar      *gtranslator_msg_get_msgid_plural     (GtranslatorMsg *msg);

const gchar      *gtranslator_msg_get_msgstr           (GtranslatorMsg *msg);

void              gtranslator_msg_set_msgstr           (GtranslatorMsg *msg,
							const gchar *msgstr);

void              gtranslator_msg_set_msgstr_plural    (GtranslatorMsg *msg,
							gint index,
							const gchar *msgstr);

G_END_DECLS

#endif /* __MSG_H__ */
