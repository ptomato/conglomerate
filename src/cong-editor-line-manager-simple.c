/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-line-manager-simple.c
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
#include "cong-editor-line-manager-simple.h"
#include "cong-editor-area.h"
#include "cong-editor-area-line.h"
#include "cong-editor-area-composer.h"

static void 
begin_line (CongEditorLineManager *line_manager);

static void
add_to_line (CongEditorLineManager *line_manager,
	     CongEditorArea *area);

static void 
end_line (CongEditorLineManager *line_manager);

static gint
get_line_width (CongEditorLineManager *line_manager);

static gint
get_current_indent (CongEditorLineManager *line_manager);

struct CongEditorLineManagerSimplePrivate
{
	CongEditorArea *area_lines;
	CongEditorAreaLine *current_line;
};

CONG_DEFINE_CLASS_BEGIN (CongEditorLineManagerSimple, cong_editor_line_manager_simple, CONG_EDITOR_LINE_MANAGER_SIMPLE, CongEditorLineManager, CONG_EDITOR_LINE_MANAGER_TYPE)
{
	CongEditorLineManagerClass *lm_klass = CONG_EDITOR_LINE_MANAGER_CLASS (klass);

	lm_klass->begin_line = begin_line;
	lm_klass->add_to_line = add_to_line;
	lm_klass->end_line = end_line;

	lm_klass->get_line_width = get_line_width;
	lm_klass->get_current_indent = get_current_indent;

}
CONG_DEFINE_CLASS_END ()
CONG_DEFINE_EMPTY_DISPOSE(cong_editor_line_manager_simple)

CongEditorLineManager*
cong_editor_line_manager_simple_construct (CongEditorLineManagerSimple *line_manager,
					   CongEditorWidget3 *widget,
					   CongEditorArea *area_lines)
{
	g_return_val_if_fail (IS_CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager), NULL);
	g_return_val_if_fail (IS_CONG_EDITOR_AREA (area_lines), NULL);

	cong_editor_line_manager_construct (CONG_EDITOR_LINE_MANAGER (line_manager),
					    widget);

	PRIVATE (line_manager)->area_lines = area_lines;

	return CONG_EDITOR_LINE_MANAGER (line_manager);
}

CongEditorLineManager*
cong_editor_line_manager_simple_new (CongEditorWidget3 *widget,
				     CongEditorArea *area_lines)
{
	g_return_val_if_fail (IS_CONG_EDITOR_AREA (area_lines), NULL);

	return CONG_EDITOR_LINE_MANAGER (cong_editor_line_manager_simple_construct (g_object_new (CONG_EDITOR_LINE_MANAGER_SIMPLE_TYPE, NULL),
										    widget,
										    area_lines));
}


static void 
begin_line (CongEditorLineManager *line_manager)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);
	CongEditorAreaLine *new_line;

	new_line = CONG_EDITOR_AREA_LINE (cong_editor_area_line_new (cong_editor_line_manager_get_widget (line_manager),
								     300)); /* FIXME */
	PRIVATE (simple)->current_line = new_line;

	cong_editor_area_composer_pack_end (CONG_EDITOR_AREA_COMPOSER (PRIVATE (simple)->area_lines),
					    CONG_EDITOR_AREA (new_line),
					    FALSE,
					    TRUE,
					    0);
}

static void
add_to_line (CongEditorLineManager *line_manager,
	     CongEditorArea *area)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);

	if (NULL==PRIVATE (simple)->current_line) {
		begin_line (line_manager);
	}

	/* FIXME: need to have some logic to do line wrapping here */

	cong_editor_area_container_add_child (CONG_EDITOR_AREA_CONTAINER (PRIVATE (simple)->current_line),
					      area);
}

static void 
end_line (CongEditorLineManager *line_manager)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);

	PRIVATE (simple)->current_line = NULL;
}

static gint
get_line_width (CongEditorLineManager *line_manager)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);

	return 300; /* FIXME */
}

static gint
get_current_indent (CongEditorLineManager *line_manager)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);

#if 1
	if (PRIVATE (simple)->current_line) {
		cong_editor_area_line_get_width_used (PRIVATE (simple)->current_line);
	} else {
		return 0;
	}
#else
	return cong_editor_area_get_requisition (CONG_EDITOR_AREA (PRIVATE (simple)->current_line),
						 GTK_ORIENTATION_HORIZONTAL,
						 300); /* FIXME */
#endif
}
