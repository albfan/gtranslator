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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "actions.h"
#include "msg.h"
#include "po.h"
#include "tab.h"
#include "window.h"


void
gtranslator_actions_edit_undo (GtkAction   *action,
			       GtranslatorWindow *window)
{
	GtranslatorView *active_view;
	GtkSourceBuffer *active_document;

	active_view = gtranslator_window_get_active_view (window);
	g_return_if_fail (active_view);

	active_document = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (active_view)));

	gtk_source_buffer_undo (active_document);

	gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtranslator_actions_edit_redo (GtkAction   *action,
			       GtranslatorWindow *window)
{
	GtranslatorView *active_view;
	GtkSourceBuffer *active_document;

	active_view = gtranslator_window_get_active_view (window);
	g_return_if_fail (active_view);

	active_document = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (active_view)));

	gtk_source_buffer_redo (active_document);

	gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtranslator_actions_edit_cut (GtkAction   *action,
			      GtranslatorWindow *window)
{
	GtranslatorView *active_view;

	active_view = gtranslator_window_get_active_view (window);
	g_return_if_fail (active_view);

	gtranslator_view_cut_clipboard (active_view);

	gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtranslator_actions_edit_copy (GtkAction   *action,
			       GtranslatorWindow *window)
{
	GtranslatorView *active_view;

	active_view = gtranslator_window_get_active_view (window);
	g_return_if_fail (active_view);

	gtranslator_view_copy_clipboard (active_view);

	gtk_widget_grab_focus (GTK_WIDGET (active_view));
}

void
gtranslator_actions_edit_paste (GtkAction   *action,
				GtranslatorWindow *window)
{
	GtranslatorView *active_view;

	active_view = gtranslator_window_get_active_view (window);
	g_return_if_fail (active_view);

	gtranslator_view_paste_clipboard (active_view);

	gtk_widget_grab_focus (GTK_WIDGET (active_view));
}


/*
 * Use the untranslated message as the translation.
 */
void 
gtranslator_message_copy_to_translation(GtkAction *action,
					GtranslatorWindow *window)
{
	const gchar *msgid;
	GtranslatorTab *current;
	GtranslatorPo *po;
	GList *msg;
	gint page_index;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	msg = gtranslator_po_get_current_message(po);
	
	page_index = gtranslator_tab_get_active_text_tab(current);
	
	if(page_index == 0)
		msgid = gtranslator_msg_get_msgid(msg->data);
	else msgid = gtranslator_msg_get_msgid_plural(msg->data);
	
	if(msgid)
	{
		page_index = gtranslator_tab_get_active_trans_tab(current);
		
		if(page_index == 0)
			gtranslator_msg_set_msgstr(msg->data, msgid);
		else
			gtranslator_msg_set_msgstr_plural(msg->data, page_index, msgid);
	}
	
	/*Is needed to reshow the message unless i make something like emit 
	  a signal in msg.c when the message is modified*/
	gtranslator_tab_show_message(current, msg->data);
}

/*
 * Toggle the sticky status
 */
void 
gtranslator_message_status_toggle_fuzzy(GtkAction *action,
					GtranslatorWindow *window)
{
	GtranslatorTab *current;
	GtranslatorPo *po;
	GtkTextView *view;
	GtkSourceBuffer *buf;
	GList *msg;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	msg = gtranslator_po_get_current_message(po);
	view = GTK_TEXT_VIEW(gtranslator_window_get_active_view(window));
	buf = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(view));
	
	if(gtranslator_msg_is_fuzzy(msg->data))
		gtranslator_msg_set_fuzzy(msg->data, FALSE);
	else
		gtranslator_msg_set_fuzzy(msg->data, TRUE);
	
	/*FIXME: I think this is ugly */
	g_signal_emit_by_name(buf, "changed", NULL);					
}

void
gtranslator_actions_edit_preferences(GtkAction *action,
				     GtranslatorWindow *window)
{
	gtranslator_show_preferences_dialog(window);
}