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
cong_editor_line_manager_begin_line (CongEditorLineManager *line_manager)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      begin_line, 
			      (line_manager));

}

void
cong_editor_line_manager_add_to_line (CongEditorLineManager *line_manager,
				      CongEditorArea *area)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (IS_CONG_EDITOR_AREA (area));

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      add_to_line, 
			      (line_manager, area));
}

void
cong_editor_line_manager_end_line (CongEditorLineManager *line_manager)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      end_line,
			      (line_manager));
}


gint
cong_editor_line_manager_get_line_width (CongEditorLineManager *line_manager)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));

	return CONG_EEL_CALL_METHOD_WITH_RETURN_VALUE (CONG_EDITOR_LINE_MANAGER_CLASS,
						       line_manager,
						       get_line_width,
						       (line_manager));
}

gint
cong_editor_line_manager_get_current_indent (CongEditorLineManager *line_manager)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));

	return CONG_EEL_CALL_METHOD_WITH_RETURN_VALUE (CONG_EDITOR_LINE_MANAGER_CLASS,
						       line_manager,
						       get_current_indent,
						       (line_manager));
}
