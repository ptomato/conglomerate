/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-line-manager-span-wrapper.c
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
#include "cong-editor-line-manager-span-wrapper.h"
#include "cong-editor-node-element.h"
#include "cong-editor-area-span-tag.h"
#include "cong-dispspec-element.h"
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

struct CongEditorLineManagerSpanWrapperPrivate
{
	CongEditorLineManager *outer_line_manager;
	CongEditorNode *editor_node;

	CongEditorArea *current_span_area;
	CongEditorArea *current_inner_line;
};

CONG_DEFINE_CLASS_BEGIN (CongEditorLineManagerSpanWrapper, cong_editor_line_manager_span_wrapper, CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER, CongEditorLineManager, CONG_EDITOR_LINE_MANAGER_TYPE)
{
	CongEditorLineManagerClass *lm_klass = CONG_EDITOR_LINE_MANAGER_CLASS (klass);

	lm_klass->begin_line = begin_line;
	lm_klass->add_to_line = add_to_line;
	lm_klass->end_line = end_line;

	lm_klass->get_line_width = get_line_width;
	lm_klass->get_current_indent = get_current_indent;
}
CONG_DEFINE_CLASS_END ()
CONG_DEFINE_EMPTY_DISPOSE(cong_editor_line_manager_span_wrapper)

CongEditorLineManager*
cong_editor_line_manager_span_wrapper_construct (CongEditorLineManagerSpanWrapper *line_manager,
						 CongEditorWidget3 *widget,
						 CongEditorNode *editor_node,
						 CongEditorLineManager *outer_line_manager)
{
	g_return_val_if_fail (IS_CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager), NULL);

	cong_editor_line_manager_construct (CONG_EDITOR_LINE_MANAGER (line_manager),
					    widget);

	PRIVATE (line_manager)->editor_node = editor_node;
	PRIVATE (line_manager)->outer_line_manager = outer_line_manager;

	return CONG_EDITOR_LINE_MANAGER (line_manager);
}


CongEditorLineManager*
cong_editor_line_manager_span_wrapper_new (CongEditorWidget3 *widget,
					   CongEditorNode *editor_node,
					   CongEditorLineManager *outer_line_manager)
{
	return CONG_EDITOR_LINE_MANAGER (cong_editor_line_manager_span_wrapper_construct (g_object_new (CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER_TYPE, NULL),
											  widget,
											  editor_node,
											  outer_line_manager));	
}

static CongEditorArea*
make_span_area (CongEditorLineManagerSpanWrapper *span_wrapper)
{
	CongEditorArea *area;
	CongDispspecElement *ds_element;
	GdkPixbuf *pixbuf;
	gchar *title_text;
	
	g_return_val_if_fail (span_wrapper, NULL);
	
	ds_element = cong_editor_node_element_get_dispspec_element (CONG_EDITOR_NODE_ELEMENT(PRIVATE (span_wrapper)->editor_node));
	
	pixbuf = cong_dispspec_element_get_icon (ds_element);
	
	title_text = cong_dispspec_element_get_section_header_text (ds_element,
								    cong_editor_node_get_node (PRIVATE (span_wrapper)->editor_node));
	
	area = cong_editor_area_span_tag_new (cong_editor_node_get_widget (PRIVATE (span_wrapper)->editor_node),
					      ds_element,
					      pixbuf,
					      title_text,
					      TRUE, /* is_at_start, */
					      TRUE /* is_at_end */);

	if (pixbuf) {
		g_object_unref (G_OBJECT(pixbuf));
	}

	g_free (title_text);

	return area;
}

static CongEditorArea*
make_inner_line (CongEditorLineManagerSpanWrapper *span_wrapper)
{
	CongEditorArea *inner_line;

	g_assert (PRIVATE (span_wrapper)->outer_line_manager);
	g_assert (PRIVATE (span_wrapper)->current_span_area);

#if 1
	inner_line = cong_editor_area_composer_new (cong_editor_node_get_widget (PRIVATE (span_wrapper)->editor_node),
						    GTK_ORIENTATION_HORIZONTAL,
						    0);
#else
	inner_line = cong_editor_area_line_new (cong_editor_node_get_widget (PRIVATE (span_wrapper)->editor_node),
						cong_editor_line_manager_get_current_width_available (CONG_EDITOR_LINE_MANAGER (span_wrapper));
#endif

	return inner_line;
}

static void
begin_line (CongEditorLineManager *line_manager)
{
	CongEditorLineManagerSpanWrapper *span_wrapper = CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager);

	PRIVATE (span_wrapper)->current_span_area = NULL;
	PRIVATE (span_wrapper)->current_inner_line = NULL;

	/* Delegate: */
	cong_editor_line_manager_begin_line (PRIVATE (span_wrapper)->outer_line_manager);       
}

static void
add_to_line (CongEditorLineManager *line_manager,
	     CongEditorArea *area)
{
	CongEditorLineManagerSpanWrapper *span_wrapper = CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager);

	if (NULL==PRIVATE (span_wrapper)->current_span_area) {
		PRIVATE (span_wrapper)->current_span_area = make_span_area (span_wrapper);
		PRIVATE (span_wrapper)->current_inner_line = make_inner_line (span_wrapper);

		cong_editor_line_manager_add_to_line (PRIVATE (span_wrapper)->outer_line_manager,
						      PRIVATE (span_wrapper)->current_span_area);

		cong_editor_area_container_add_child (CONG_EDITOR_AREA_CONTAINER (PRIVATE (span_wrapper)->current_span_area),
						      PRIVATE (span_wrapper)->current_inner_line);
	}

	/* FIXME: might need to implement some kind of linewrap here */

	cong_editor_area_container_add_child (CONG_EDITOR_AREA_CONTAINER (PRIVATE (span_wrapper)->current_inner_line),
					      area);
}

static void 
end_line (CongEditorLineManager *line_manager)
{
	CongEditorLineManagerSpanWrapper *span_wrapper = CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager);

	PRIVATE (span_wrapper)->current_span_area = NULL;
	PRIVATE (span_wrapper)->current_inner_line = NULL;
	
	/* Delegate: */
	return cong_editor_line_manager_end_line (PRIVATE (span_wrapper)->outer_line_manager);
}

static gint
get_line_width (CongEditorLineManager *line_manager)
{
	CongEditorLineManagerSpanWrapper *span_wrapper = CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager);

	/* Delegate: */
	return cong_editor_line_manager_get_line_width (PRIVATE (span_wrapper)->outer_line_manager);
}

static gint
get_current_indent (CongEditorLineManager *line_manager)
{
	CongEditorLineManagerSpanWrapper *span_wrapper = CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager);

	/* Delegate: */
	return cong_editor_line_manager_get_current_indent (PRIVATE (span_wrapper)->outer_line_manager);
}
