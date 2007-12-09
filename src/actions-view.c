/*
 * Copyright (C) 2007 Seán de Búrca <leftmostcat@gmail.com>
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
 */

#include "actions.h"
#include "window.h"

void
gtranslator_actions_view_show_side_pane (GtkAction   *action,
					 GtranslatorWindow *window)
{
	gboolean visible;
	GtranslatorPanel *panel;

	visible = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	panel = gtranslator_window_get_side_panel (window);

	if (visible)
	{
		gtk_widget_show (GTK_WIDGET (panel));
		gtk_widget_grab_focus (GTK_WIDGET (panel));
	}
	else
	{
		gtk_widget_hide (GTK_WIDGET (panel));
	}
}
