/*
 * (C) 2001-2003 	Fatih Demir <kabalak@kabalak.net>
 * 			Gediminas Paulauskas <menesis@kabalak.net>
 * 			Thomas Ziehmer <thomas@kabalak.net>
 * 			Ross Golder <ross@kabalak.net>
 *
 * gtranslator is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or   
 *    (at your option) any later version.
 *    
 * gtranslator is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *    GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "actions.h"
#include "comment.h"
#include "dialogs.h"
#include "gui.h"
#include "learn.h"
#include "menus.h"
#include "message.h"
#include "messages-table.h"
#include "parse.h"
#include "prefs.h"
#include "syntax.h"
#include "undo.h"
#include "utils.h"
#include "utils_gui.h"

#include <string.h>
#include <regex.h>
#include <gtk/gtk.h>
#include <gtkspell/gtkspell.h>
#include <libgnomeui/gnome-app.h>
#include <libgnomeui/gnome-app-util.h>

#include <gettext-0.0/config.h>
#include <gettext-0.0/message.h>

static gboolean is_fuzzy(GList *msg, gpointer useless);
static gboolean is_untranslated(GList *msg, gpointer useless);

/*
 * Remove textviews from text and trans vboxes
 */
void gtranslator_message_remove_textviews(void) {
	GList *children;

	if(text_vbox) {
		children = gtk_container_get_children(GTK_CONTAINER(text_vbox));
		while (children) {
			gtk_widget_destroy(GTK_WIDGET(children->data));
			children = children->next;
		}
	}
	if(trans_vbox) {
		children = gtk_container_get_children(GTK_CONTAINER(trans_vbox));
		while (children) {
			gtk_widget_destroy(GTK_WIDGET(children->data));
			children = children->next;
		}
	}
}

/*
 * Calls function func on each item in list 'begin'. Starts from 
 * item 'begin', loops to first element, and stops at 'begin'.
 * Returns TRUE, if found, FALSE otherwise.
 */
gboolean gtranslator_message_for_each(GList * begin, FEFunc func, gpointer user_data)
{
	GList *msg;

	g_return_val_if_fail(begin != NULL, FALSE);

	msg = begin;
	do {
		if (msg == NULL) {
			msg = g_list_first(begin);
			g_return_val_if_fail(msg != NULL, TRUE);
		}
		if (func(msg, user_data))
			return TRUE;
		msg = msg->next;
	} while (msg != begin);
	return FALSE;
}

static gboolean is_fuzzy(GList *msg, gpointer useless)
{
	/* 
	 * Control if there's any message data.
	 */
	if(!(GTR_MSG(msg->data)))
	{
		g_warning(_("Couldn't get the message!"));
		return FALSE;
	}
	if (GTR_MSG(msg->data)->message->is_fuzzy) {
		gtranslator_message_go_to(msg);
		return TRUE;
	} else
		return FALSE;
}

void gtranslator_message_go_to_next_fuzzy(GtkWidget * widget, gpointer useless)
{
	GList *begin;
	
	g_return_if_fail(file_opened == TRUE);
	
 	begin = po->current->next;
	if (!begin)
		begin = po->messages;
	if (gtranslator_message_for_each(begin, (FEFunc)is_fuzzy, NULL) == TRUE)
		return;
	gnome_app_message(GNOME_APP(gtranslator_application), 
			  _("There are no fuzzy messages left."));
	gtranslator_actions_disable(ACT_NEXT_FUZZY);
}

static gboolean is_untranslated(GList *msg, gpointer useless)
{
	message_ty *message = GTR_MSG(msg)->message;
	if (message->msgstr && message->msgstr[0] != '\0')
		return FALSE;
	gtranslator_message_go_to(msg);
	return TRUE;
}

void gtranslator_message_go_to_next_untranslated(GtkWidget * widget, gpointer useless)
{
	GList *begin;
	
	g_return_if_fail(file_opened == TRUE);

 	begin = po->current->next;
	if (!begin)
		begin = po->messages;
	if (gtranslator_message_for_each(begin, (FEFunc)is_untranslated, NULL))
		return;
	gnome_app_message(GNOME_APP(gtranslator_application), 
			  _("All messages seem to be translated."));
	gtranslator_actions_disable(ACT_NEXT_UNTRANSLATED);
}

/*
 * Invert spaces/dots if required
 */
char *gtranslator_invert_dots(char *str) {
	char *response;

	if(GtrPreferences.dot_char) {
		/* Should probably move this function in here directly */
		response = gtranslator_utils_invert_dot(str);
	}
	else {
		response = g_strdup(str);
	}

	return response;
}

/*
 * Create new widgets (text) and attach spellchecker where required.
 */
GtkTextView *gtranslator_new_textview(GtkWidget *vbox,
	char *thetext, gboolean iseditable)
{
	GtkTextView *widget;
	GtkTextBuffer *editable;
	GError *error = NULL;
	GtkSpell *gtrans_spell = NULL;
	char *text, *errortext = NULL;

	/* Set up widget */
	widget = GTK_TEXT_VIEW(gtk_text_view_new());
	gtk_text_view_set_wrap_mode(widget, GTK_WRAP_CHAR);
	gtk_text_view_set_editable(widget, iseditable);
	gtk_text_view_set_cursor_visible(widget, iseditable);

	/* Set text */
	editable = gtk_text_view_get_buffer(widget);
	text = gtranslator_invert_dots(thetext);
	gtk_text_buffer_set_text(editable, text, -1);
	g_free(text);

	/* Connect signals */
	g_signal_connect(G_OBJECT (editable), "changed",
			  G_CALLBACK (gtranslator_translation_changed), NULL);
#ifdef NOT_PORTED
	g_signal_connect_after(G_OBJECT(editable), "selection-get",
			 G_CALLBACK(selection_get_handler), NULL);
	trans_box_insert_text_signal_id = g_signal_connect(
		G_OBJECT(gtk_text_view_get_buffer(trans_box)), 
		"insert-text",
		G_CALLBACK(insert_text_handler), NULL);
	trans_box_delete_text_signal_id = g_signal_connect(
		G_OBJECT(gtk_text_view_get_buffer(trans_box)), 
		"delete-range",
		G_CALLBACK(delete_text_handler), NULL);
#endif
	/* Pack into vertical box */
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(widget),
		TRUE, TRUE, 0);

	/* Set up spellchecking, if enabled */
	if(GtrPreferences.instant_spell_check) {
		gtrans_spell = gtkspell_new_attach(widget, NULL, &error);
		if(gtrans_spell == NULL) {
			g_print(_("gtkspell error: %s\n"), error->message);
			errortext = g_strdup_printf(_("GtkSpell was unable to initialize.\n %s"), error->message);
			g_error_free(error);
		}
	}

	return widget;
}

/* 
 * Display the given message in a bunch of text boxes
 */
void gtranslator_message_show(GtrMsg *msg)
{
	GList *children;
	GtkWidget *child;
	char *p;
	int i, msgcount;

	g_assert(GTK_IS_VBOX(text_vbox));
	g_assert(GTK_IS_VBOX(trans_vbox));
	g_return_if_fail(msg!=NULL);

	nothing_changes = TRUE;

	/*
	 * Set up the comment display.
	 */
	gtranslator_comment_display(GTR_COMMENT(msg->comment));
	
	/*
	 * Free any existing widgets in use for the previous message
	 */
	gtranslator_message_remove_textviews();

	text_msgid = gtranslator_new_textview(text_vbox,
		(char *)msg->message->msgid, FALSE);

	if(msg->message->msgid_plural) {
		text_msgid_plural = gtranslator_new_textview(text_vbox,
			(char*)msg->message->msgid_plural, FALSE);
	}
	gtk_widget_show_all(text_vbox);

	/*
	 * Unpack the plurals from the message_ty directly into GtkTextViews
	 */
	for (p = (char*)msg->message->msgstr, i = 0;
		p < msg->message->msgstr + msg->message->msgstr_len;
		p += strlen (p) + 1, i++)
	{
		trans_msgstr[i] = gtranslator_new_textview(trans_vbox, p, TRUE);
	}
	gtk_widget_show_all(trans_vbox);

	/*
	 * Disable/enable fuzzy action
	 */
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(the_edit_menu[17].widget),
		msg->message->is_fuzzy);

	/*
	 * Reset changed flags
	 */
	nothing_changes = FALSE;
	message_changed = FALSE;
}

void gtranslator_message_update()
{
	char *p;
	guchar *newmsgstr, *newp, *str;
	guint newmsgstrlen = 0, msgcount = 0, i;
	GtkTextIter start, end;
	GtrMsg *msg;

	if (!message_changed)
		return;

	msg = GTR_MSG(po->current->data);

	/* 		
	/*
	 * Work out the length of the new msgstr and check for faulty
	 * translations
	 */
	for (p = (char *)msg->message->msgstr, i = 0;
		p < msg->message->msgstr + msg->message->msgstr_len;
		p += strlen (p) + 1, i++)
	{
		GtkTextBuffer *buf = gtk_text_view_get_buffer(trans_msgstr[i]);
		gtk_text_buffer_get_bounds(buf, &start, &end);
		newp = gtk_text_buffer_get_text(buf, &start, &end, FALSE);

		/* Make both strings end with or without endline */
		if(strlen(p) > 0 && g_utf8_strlen(newp, -1) > 0) {
			gtk_text_buffer_get_end_iter(buf, &end);
			if(p[strlen(p) - 1] == '\n' && newp[strlen(newp) - 1] != '\n') {
				gtk_text_buffer_insert(buf, &end, "\n", 1);
			}
			if(p[strlen(p) - 1] == '\n' && newp[strlen(newp) - 1] != '\n') {
				gtk_text_buffer_delete(buf, &end, &end - 1);
			}
		}

		/* Determine new msgstr string length */
		newmsgstrlen += strlen(newp) + 1;
		msgcount++;
	}

	/*
	 * Pack the msgstrs back into a new msgstr
	 */
	newmsgstr = g_malloc(newmsgstrlen);
	for (i = 0; i < msgcount; i++)
	{
		GtkTextBuffer *buf = gtk_text_view_get_buffer(trans_msgstr[i]);
		gtk_text_buffer_get_bounds(buf, &start, &end);
		newp = gtk_text_buffer_get_text(buf, &start, &end, FALSE);

		/* Remove dots and append msgstr */
		str = gtranslator_invert_dots(newp);
		newmsgstr = g_stpcpy(newmsgstr, str) + 1;
		g_free(str);
	}

	/*
	 * Replace in message_ty
	 */
	g_free((gpointer *)msg->message->msgstr);
	msg->message->msgstr = newmsgstr;

	/*
	 * Alter message counts
	 */		
	if(msg->message->msgstr[0] != '\0') {
		po->translated++;
	}

	/*
	 * Learn the msgstr as a string.
	 */
	if(GtrPreferences.auto_learn)
	{
		gtranslator_learn_string(msg->message->msgid, msg->message->msgstr);
	}

	/*
	 * Mark message as committed to memory
	 */
	message_changed = FALSE;
	
	/*
	 * Update the statusbar informations.
	 */
	gtranslator_application_bar_update(g_list_position(po->messages, 
		po->current));

	/*
	 * Go to the corresponding row in the messages table.
	 */
	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_update_row(msg);
	}

	/*
	 * Enable the remove all translations function if wished and if any
	 *  translated entries have already landed.
	 */
	if(GtrPreferences.rambo_function && (po->translated > 0))
	{
		gtranslator_actions_enable(ACT_REMOVE_ALL_TRANSLATIONS);
	}
}

void gtranslator_message_toggle_fuzzy(GtkWidget  * item, gpointer data)
{
	if (nothing_changes)
		return;
	gtranslator_translation_changed(NULL, NULL);

		gtranslator_message_status_set_fuzzy(GTR_MSG(po->current->data),
			       GTK_CHECK_MENU_ITEM(item)->active);
		if(GTK_CHECK_MENU_ITEM(item)->active)
			po->fuzzy++;
		else
			po->fuzzy--;
	
	gtranslator_message_update();
	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_update_row(GTR_MSG(po->current->data));
	}
}

/*
 * Updates current msg, and shows to_go msg instead, also adjusts actions
 */
void gtranslator_message_go_to(GList * to_go)
{
	static gint pos = 0;
	GtrMsg *msg = NULL;
	GtkTextBuffer *buffer = NULL;
	GtkTextIter iter;
	GtkTextMark *mark = NULL;
 
	g_return_if_fail (to_go!=NULL);
	
	gtranslator_message_update();
	
	if (pos == 0)
	{
		gtranslator_actions_enable(ACT_FIRST, ACT_BACK);
	}	
	else if (pos == po->length - 1)
	{
		gtranslator_actions_enable(ACT_NEXT, ACT_LAST);
	}

	po->current = to_go;
	gtranslator_message_show(po->current->data);

	if(GtrPreferences.show_messages_table)
	{
		gtranslator_messages_table_select_row(GTR_MSG(po->current->data));
	}
	
	pos = g_list_position(po->messages, po->current);
	
	if (pos == 0)
	{
		gtranslator_actions_disable(ACT_FIRST, ACT_BACK);
	}	
	else if (pos == po->length - 1)
	{
		gtranslator_actions_disable(ACT_NEXT, ACT_LAST);
	}
	
	gtranslator_application_bar_update(pos);

	/*
	 * Clean up any Undo stuff lying 'round.
	 */
	gtranslator_undo_clean_register();
	gtranslator_actions_disable(ACT_UNDO);
}

/*
 * Callbacks for moving around messages 
 */
void gtranslator_message_go_to_first(GtkWidget  * widget, gpointer useless)
{
	gtranslator_message_go_to(g_list_first(po->messages));
}

void gtranslator_message_go_to_previous(GtkWidget  * widget, gpointer useless)
{
	gtranslator_message_go_to(g_list_previous(po->current));
}

void gtranslator_message_go_to_next(GtkWidget  * widget, gpointer useless)
{
	gtranslator_message_go_to(g_list_next(po->current));
}

void gtranslator_message_go_to_last(GtkWidget  * widget, gpointer useless)
{
	gtranslator_message_go_to(g_list_last(po->messages));
}

void gtranslator_message_go_to_no(GtkWidget  * widget, gpointer number)
{
	gtranslator_message_go_to(g_list_nth(po->messages, GPOINTER_TO_UINT(number)));
}

/*
 * Set Fuzzy status.
 */
void gtranslator_message_status_set_fuzzy(GtrMsg * msg, gboolean fuzzy)
{
	static int compiled = FALSE;
	static regex_t rexf, rexc;
	regmatch_t pos[3];
	gchar *comment;

	g_return_if_fail(msg!=NULL);
	g_return_if_fail(msg->comment!=NULL);
	g_return_if_fail(GTR_COMMENT(msg->comment)->comment!=NULL);

	comment=gtranslator_comment_get_comment_contents(msg->comment);
	
	if (!compiled) {
		regcomp(&rexf, "^(#), c-format", REG_EXTENDED | REG_NEWLINE);
		regcomp(&rexc, "(^#, fuzzy$)|^#, (fuzzy,) .*$", REG_EXTENDED | REG_NEWLINE);
		compiled = TRUE;
	}

	/* 
	 * If fuzzy status is already correct
	 */
	if (msg->message->is_fuzzy)
		return;
	if (fuzzy) {
		gchar *comchar;
		
		msg->message->is_fuzzy = TRUE;

		if (!regexec(&rexf, comment, 3, pos, 0)) {
			comment[pos[1].rm_so] = '\0';
			comchar = g_strdup_printf("%s#, fuzzy%s", comment, 
					    comment+pos[1].rm_eo);
		} else {
			comchar = g_strdup_printf("%s#, fuzzy\n", comment);
		}

		gtranslator_comment_update(&msg->comment, comchar);

		/* No need to free comment. Fixes several crashes and file corruption bugs */
		/* GTR_FREE(comment); */
		
		GTR_FREE(comchar);
	} else {
		msg->message->is_fuzzy = FALSE;
		if (!regexec(&rexc, comment, 3, pos, 0)) {
			gint i = (pos[1].rm_so == -1) ? 2 : 1;
			strcpy(comment+pos[i].rm_so, comment+pos[i].rm_eo+1);
		}
	}
}

/*
 * Copy msgid to msgstr, or blank msgstr
 */
void gtranslator_message_copy_to_translation (GtrMsg * msg, gpointer useless)
{
	g_return_if_fail(file_opened==TRUE);

	msg=GTR_MSG(GTR_PO(po)->current->data);
	g_return_if_fail(msg!=NULL);

	g_free((gpointer *)msg->message->msgstr);
	msg->message->msgstr = g_strdup(msg->message->msgid);
	
	/*
	 * It is no longer fuzzy.
	 */
	gtranslator_message_status_set_fuzzy(msg, FALSE);
	po->fuzzy--;

	message_changed = TRUE;
	gtranslator_message_show(po->current->data);
	gtranslator_get_translated_count();
	gtranslator_actions_enable(ACT_REVERT, ACT_SAVE);
	gtranslator_translation_changed(NULL, NULL);
}

/*
 * Free the structure and it's elements.
 */
void gtranslator_message_free(gpointer data, gpointer useless)
{
	g_return_if_fail(data!=NULL);
	gtranslator_comment_free(&GTR_MSG(data)->comment);
#ifdef REDUNDANT_NOW
	GTR_FREE(GTR_MSG(data)->msgid);
	GTR_FREE(GTR_MSG(data)->msgstr);

	GTR_FREE(GTR_MSG(data)->msgid_plural);
	GTR_FREE(GTR_MSG(data)->msgstr_1);
	GTR_FREE(GTR_MSG(data)->msgstr_2);
#endif
	GTR_FREE(data);
}

