/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-line-manager.c
 *
 * Copyright (C) 2004 David Malcolm
 *
 * Conglomerate is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Conglomerate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Authors: David Malcolm <david@davemalcolm.demon.co.uk>
 */

#include "global.h"
#include "cong-editor-line-manager.h"
#include "cong-editor-area.h"
#include "cong-editor-node.h"
#include "cong-eel.h"

struct CongEditorLineManagerPrivate
{
	CongEditorWidget3* widget;
};

CONG_DEFINE_CLASS_BEGIN (CongEditorLineManager, cong_editor_line_manager, CONG_EDITOR_LINE_MANAGER, GObject, G_TYPE_OBJECT)
CONG_DEFINE_CLASS_END ()
CONG_DEFINE_EMPTY_DISPOSE(cong_editor_line_manager)

void
cong_editor_line_manager_construct (CongEditorLineManager *line_manager,
				    CongEditorWidget3 *widget)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));

	PRIVATE (line_manager)->widget = widget;
}

CongEditorWidget3*
cong_editor_line_manager_get_widget (CongEditorLineManager *line_manager)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));

	return PRIVATE (line_manager)->widget;
}

void 
cong_editor_line_manager_add_node (CongEditorLineManager *line_manager,
				   CongEditorNode *node)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (IS_CONG_EDITOR_NODE (node));

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      add_node, 
			      (line_manager, node));
}

void 
cong_editor_line_manager_remove_node (CongEditorLineManager *line_manager,
				      CongEditorNode *node)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (IS_CONG_EDITOR_NODE (node));

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      remove_node, 
			      (line_manager, node));
}

void
cong_editor_line_manager_begin_line (CongEditorLineManager *line_manager,
				     CongEditorLineIter *line_iter)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (line_iter);

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      begin_line, 
			      (line_manager, line_iter));

}

void
cong_editor_line_manager_add_to_line (CongEditorLineManager *line_manager,
				      CongEditorLineIter *line_iter,
				      CongEditorArea *area)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (line_iter);
	g_return_if_fail (IS_CONG_EDITOR_AREA (area));

	/* Line wrapping: */
	{
		/* The areas we are getting should hopefully either be exactly the correct size to line-wrap appropriately,
		   or should stop short due to things like span tags starting/stopping, or simply due to running out of content etc: */
		gint width_available = cong_editor_line_manager_get_current_width_available (line_manager, 
											     line_iter);
		gint area_width = cong_editor_area_get_requisition_width (area,
									  width_available);
		if (area_width>width_available) {
			/* Force a line-break: */
			cong_editor_line_manager_end_line (line_manager,
							   line_iter);
		}
	}

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      add_to_line, 
			      (line_manager, line_iter, area));
}

void
cong_editor_line_manager_end_line (CongEditorLineManager *line_manager,
				   CongEditorLineIter *line_iter)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (line_iter);

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      end_line,
			      (line_manager, line_iter));
}


gint
cong_editor_line_manager_get_line_width (CongEditorLineManager *line_manager,
					 CongEditorLineIter *line_iter)
{
	g_return_val_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager), 0);
	g_return_val_if_fail (line_iter, 0);

	return CONG_EEL_CALL_METHOD_WITH_RETURN_VALUE (CONG_EDITOR_LINE_MANAGER_CLASS,
						       line_manager,
						       get_line_width,
						       (line_manager, line_iter));
}

gint
cong_editor_line_manager_get_current_indent (CongEditorLineManager *line_manager,
					     CongEditorLineIter *line_iter)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_val_if_fail (line_iter, 0);

	return CONG_EEL_CALL_METHOD_WITH_RETURN_VALUE (CONG_EDITOR_LINE_MANAGER_CLASS,
						       line_manager,
						       get_current_indent,
						       (line_manager, line_iter));
}

gint
cong_editor_line_manager_get_current_width_available (CongEditorLineManager *line_manager,
						      CongEditorLineIter *line_iter)
{
	gint line_width;
	gint current_indent;

	g_return_val_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager), 0);
	g_return_val_if_fail (line_iter, 0);

	line_width = cong_editor_line_manager_get_line_width (line_manager, line_iter);
	current_indent = cong_editor_line_manager_get_current_indent (line_manager, line_iter);

	return line_width - current_indent;
}
