/*
 * Copyright (C) 2005 - Paolo Borelli
 * 		 2007 - Ignacio Casal Quinteiro <nacho.resa@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "statusbar.h"

#define GTR_STATUSBAR_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GTR_TYPE_STATUSBAR, GtranslatorStatusbarPrivate))

struct _GtranslatorStatusbarPrivate
{
	GtkWidget     *overwrite_mode_statusbar;

	GtkWidget     *state_frame;
	GtkWidget     *load_image;
	GtkWidget     *save_image;

	GtkWidget     *error_frame;
	GtkWidget     *error_event_box;

	/* tmp flash timeout data */
	guint          flash_timeout;
	guint          flash_context_id;
	guint          flash_message_id;
};

G_DEFINE_TYPE(GtranslatorStatusbar, gtranslator_statusbar, GTK_TYPE_STATUSBAR)

static void
gtranslator_statusbar_notify (GObject    *object,
			      GParamSpec *pspec)
{
	/* don't allow gtk_statusbar_set_has_resize_grip to mess with us.
	 * See _gtranslator_statusbar_set_has_resize_grip for an explanation.
	 */
	if (strcmp (g_param_spec_get_name (pspec), "has-resize-grip") == 0)
	{
		gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (object), FALSE);
		return;
	}

	if (G_OBJECT_CLASS (gtranslator_statusbar_parent_class)->notify)
		G_OBJECT_CLASS (gtranslator_statusbar_parent_class)->notify (object, pspec);
}

static void
gtranslator_statusbar_finalize (GObject *object)
{
	GtranslatorStatusbar *statusbar = GTR_STATUSBAR (object);

	if (statusbar->priv->flash_timeout > 0)
		g_source_remove (statusbar->priv->flash_timeout);

	G_OBJECT_CLASS (gtranslator_statusbar_parent_class)->finalize (object);
}

static void
gtranslator_statusbar_class_init (GtranslatorStatusbarClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->notify = gtranslator_statusbar_notify;
	object_class->finalize = gtranslator_statusbar_finalize;

	g_type_class_add_private (object_class, sizeof (GtranslatorStatusbarPrivate));
}

static void
gtranslator_statusbar_init (GtranslatorStatusbar *statusbar)
{
	GtkWidget *hbox;
	GtkWidget *error_image;

	statusbar->priv = GTR_STATUSBAR_GET_PRIVATE (statusbar);

	gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (statusbar), FALSE);

	statusbar->priv->overwrite_mode_statusbar = gtk_statusbar_new ();
	gtk_widget_show (statusbar->priv->overwrite_mode_statusbar);
	gtk_widget_set_size_request (statusbar->priv->overwrite_mode_statusbar,
				     80,
				     10);

	gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (statusbar->priv->overwrite_mode_statusbar),
					   TRUE);
	gtk_box_pack_end (GTK_BOX (statusbar),
			  statusbar->priv->overwrite_mode_statusbar,
			  FALSE, TRUE, 0);

	statusbar->priv->state_frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (statusbar->priv->state_frame), GTK_SHADOW_IN);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (statusbar->priv->state_frame), hbox);

	statusbar->priv->load_image = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU);
	statusbar->priv->save_image = gtk_image_new_from_stock (GTK_STOCK_SAVE, GTK_ICON_SIZE_MENU);

	gtk_widget_show (hbox);

	gtk_box_pack_start (GTK_BOX (hbox),
			    statusbar->priv->load_image,
			    FALSE, TRUE, 4);
	gtk_box_pack_start (GTK_BOX (hbox),
			    statusbar->priv->save_image,
			    FALSE, TRUE, 4);

	gtk_box_pack_start (GTK_BOX (statusbar),
			    statusbar->priv->state_frame,
			    FALSE, TRUE, 0);

	statusbar->priv->error_frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (statusbar->priv->error_frame), GTK_SHADOW_IN);

	error_image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_MENU);
	gtk_misc_set_padding (GTK_MISC (error_image), 4, 0);
	gtk_widget_show (error_image);

	statusbar->priv->error_event_box = gtk_event_box_new ();
	gtk_event_box_set_visible_window  (GTK_EVENT_BOX (statusbar->priv->error_event_box),
					   FALSE);
	gtk_widget_show (statusbar->priv->error_event_box);

	gtk_container_add (GTK_CONTAINER (statusbar->priv->error_frame),
			   statusbar->priv->error_event_box);
	gtk_container_add (GTK_CONTAINER (statusbar->priv->error_event_box),
			   error_image);

	gtk_box_pack_start (GTK_BOX (statusbar),
			    statusbar->priv->error_frame,
			    FALSE, TRUE, 0);

	gtk_box_reorder_child (GTK_BOX (statusbar),
			       statusbar->priv->error_frame,
			       0);
}

/**
 * gtranslator_statusbar_new:
 *
 * Creates a new #GtranslatorStatusbar.
 *
 * Return value: the new #GtranslatorStatusbar object
 **/
GtkWidget *
gtranslator_statusbar_new (void)
{
	return GTK_WIDGET (g_object_new (GTR_TYPE_STATUSBAR, NULL));
}

/**
 * gtranslator_set_has_resize_grip:
 * @statusbar: a #GtranslatorStatusbar
 * @show: if the resize grip is shown
 *
 * Sets if a resize grip showld be shown.
 *
 **/
 /*
  * I don't like this much, in a perfect world it would have been
  * possible to override the parent property and use
  * gtk_statusbar_set_has_resize_grip. Unfortunately this is not
  * possible and it's not even possible to intercept the notify signal
  * since the parent property should always be set to false thus when
  * using set_resize_grip (FALSE) the property doesn't change and the
  * notification is not emitted.
  * For now just add this private method; if needed we can turn it into
  * a property.
  */
void
_gtranslator_statusbar_set_has_resize_grip (GtranslatorStatusbar *bar,
				      gboolean        show)
{
	g_return_if_fail (GTR_IS_STATUSBAR (bar));

	gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (bar->priv->overwrite_mode_statusbar),
					   show);
}

/**
 * gtranslator_statusbar_set_overwrite:
 * @statusbar: a #GtranslatorStatusbar
 * @overwrite: if the overwrite mode is set
 *
 * Sets the overwrite mode on the statusbar.
 **/
void
gtranslator_statusbar_set_overwrite (GtranslatorStatusbar *statusbar,
				     gboolean        overwrite)
{
	gchar *msg;

	g_return_if_fail (GTR_IS_STATUSBAR (statusbar));

	gtk_statusbar_pop (GTK_STATUSBAR (statusbar->priv->overwrite_mode_statusbar), 0);

	if (overwrite)
		msg = g_strdup (_("  OVR"));
	else
		msg = g_strdup (_("  INS"));

	gtk_statusbar_push (GTK_STATUSBAR (statusbar->priv->overwrite_mode_statusbar), 0, msg);

      	g_free (msg);
}

void
gtranslator_statusbar_clear_overwrite (GtranslatorStatusbar *statusbar)
{
	g_return_if_fail (GTR_IS_STATUSBAR (statusbar));

	gtk_statusbar_pop (GTK_STATUSBAR (statusbar->priv->overwrite_mode_statusbar), 0);
}

static gboolean
remove_message_timeout (GtranslatorStatusbar *statusbar)
{
	gtk_statusbar_remove (GTK_STATUSBAR (statusbar),
			      statusbar->priv->flash_context_id,
			      statusbar->priv->flash_message_id);

	/* remove the timeout */
	statusbar->priv->flash_timeout = 0;
  	return FALSE;
}

/**
 * gtranslator_statusbar_flash_message:
 * @statusbar: a #GtranslatorStatusbar
 * @context_id: message context_id
 * @format: message to flash on the statusbar
 *
 * Flash a temporary message on the statusbar.
 */
void
gtranslator_statusbar_flash_message (GtranslatorStatusbar *statusbar,
				     guint           context_id,
				     const gchar    *format, ...)
{
	const guint32 flash_length = 3000; /* three seconds */
	va_list args;
	gchar *msg;

	g_return_if_fail (GTR_IS_STATUSBAR (statusbar));
	g_return_if_fail (format != NULL);

	va_start (args, format);
	msg = g_strdup_vprintf (format, args);
	va_end (args);

	/* remove a currently ongoing flash message */
	if (statusbar->priv->flash_timeout > 0)
	{
		g_source_remove (statusbar->priv->flash_timeout);
		statusbar->priv->flash_timeout = 0;

		gtk_statusbar_remove (GTK_STATUSBAR (statusbar),
				      statusbar->priv->flash_context_id,
				      statusbar->priv->flash_message_id);
	}

	statusbar->priv->flash_context_id = context_id;
	statusbar->priv->flash_message_id = gtk_statusbar_push (GTK_STATUSBAR (statusbar),
								context_id,
								msg);

	statusbar->priv->flash_timeout = g_timeout_add (flash_length,
							(GtkFunction) remove_message_timeout,
							statusbar);

	g_free (msg);
}
