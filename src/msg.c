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
 *     MERCHANMsgILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "msg.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gettext-po.h>

#define GTR_MSG_GET_PRIVATE(object)	(G_TYPE_INSTANCE_GET_PRIVATE ( \
					 (object),	\
					 GTR_TYPE_MSG,     \
					 GtranslatorMsgPrivate))


G_DEFINE_TYPE(GtranslatorMsg, gtranslator_msg, G_TYPE_OBJECT)

struct _GtranslatorMsgPrivate
{
	//Missing comment
	//Header too??
	po_message_t message;
};


static void
gtranslator_msg_init (GtranslatorMsg *msg)
{
	msg->priv = GTR_MSG_GET_PRIVATE (msg);
}

static void
gtranslator_msg_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_msg_parent_class)->finalize (object);
}

static void
gtranslator_msg_class_init (GtranslatorMsgClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorMsgPrivate));

	object_class->finalize = gtranslator_msg_finalize;
}

/***************************** Public funcs ***********************************/

/**
 * gtranslator_msg_new:
 * 
 * Return value: a new #GtranslatorMsg object
 **/
GtranslatorMsg *
gtranslator_msg_new(void)
{
	return g_object_new (GTR_TYPE_MSG, NULL);
}

/**
 * gtranslator_msg_get_message:
 * @msg: a #GtranslatorMsg
 *
 * Return value: the message
 **/
po_message_t
gtranslator_msg_get_message(GtranslatorMsg *msg)
{
	return msg->priv->message;
}

/**
 * gtranslator_msg_set_message:
 * @msg: a #GtranslatorMsg
 * @message: the po_message_t to set into the @msg
 *
 * Sets the message into the #GtranslatorMsg class.
 **/
void
gtranslator_msg_set_message(GtranslatorMsg *msg,
			    po_message_t message)
{
	msg->priv->message = message;
}

/**
 * po_message_is_translated:
 * @msg: a #GtranslatorMsg
 * 
 * Return value: TRUE if the message is translated
 **/
gboolean
gtranslator_msg_is_translated (GtranslatorMsg *msg)
{
	if (po_message_msgid_plural(msg->priv->message) == NULL)
		return po_message_msgstr(msg->priv->message)[0] != '\0';	
	else
	{
		gint i;

		for (i = 0; ; i++)
		{
			const gchar *str_i = po_message_msgstr_plural(msg->priv->message, i);
			if (str_i == NULL)
				break;
			if (str_i[0] == '\0')
				return FALSE;
		}
		
		return TRUE;
	}
}
