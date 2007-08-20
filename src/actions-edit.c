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


/*
 * Toggle the sticky status
 */
void 
gtranslator_message_status_toggle_fuzzy(GtkAction *action,
					GtranslatorWindow *window)
{
	GtranslatorTab *current;
	GtranslatorPo *po;
	GList *msg;
	
	current = gtranslator_window_get_active_tab(window);
	po = gtranslator_tab_get_po(current);
	msg = gtranslator_po_get_current_message(po);
	
	if(gtranslator_msg_is_fuzzy(msg->data))
		gtranslator_msg_set_fuzzy(msg->data, FALSE);
	else
		gtranslator_msg_set_fuzzy(msg->data, TRUE);
		
						
	/*Maybe something like g_signal_emit message change??*/
}
