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
#include "cong-editor-line-iter-span-wrapper.h"
#include "cong-editor-node-element.h"
#include "cong-editor-area-span-tag.h"
#include "cong-dispspec-element.h"
#include "cong-editor-area-composer.h"

#include "cong-eel.h"

static CongEditorLineIter*
make_iter (CongEditorLineManager *line_manager);

static void 
begin_line (CongEditorLineManager *line_manager,
	    CongEditorLineIter *line_iter);

static void
add_to_line (CongEditorLineManager *line_manager,
	     CongEditorLineIter *line_iter,
	     CongEditorArea *area);

static void 
end_line (CongEditorLineManager *line_manager,
	  CongEditorLineIter *line_iter);

static gint
get_line_width (CongEditorLineManager *line_manager,
		CongEditorLineIter *line_iter);

static gint
get_current_indent (CongEditorLineManager *line_manager,
		    CongEditorLineIter *line_iter);

/* Data stored about each line: */
typedef struct PerLineData PerLineData;
struct PerLineData
{
	CongEditorArea *span_area;
	CongEditorArea *inner_line;
};

static void
hash_value_destroy_func (gpointer data);

static PerLineData*
get_data_for_line (CongEditorLineManagerSpanWrapper *span_wrapper,
		   CongEditorAreaLine *line);


struct CongEditorLineManagerSpanWrapperPrivate
{
	CongEditorNode *editor_node;

	CongEditorLineManager *outer_line_manager;
	CongEditorLineIter *outer_iter;

	GHashTable *hash_of_line_to_data;
};

CONG_DEFINE_CLASS_BEGIN (CongEditorLineManagerSpanWrapper, cong_editor_line_manager_span_wrapper, CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER, CongEditorLineManager, CONG_EDITOR_LINE_MANAGER_TYPE)
{
	CongEditorLineManagerClass *lm_klass = CONG_EDITOR_LINE_MANAGER_CLASS (klass);

	lm_klass->make_iter = make_iter;

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
						 CongEditorLineManager *outer_line_manager,
						 CongEditorLineIter *outer_iter)
{
	g_return_val_if_fail (IS_CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager), NULL);
	g_return_val_if_fail (IS_CONG_EDITOR_LINE_MANAGER (outer_line_manager), NULL);
	g_return_val_if_fail (IS_CONG_EDITOR_LINE_ITER (outer_iter), NULL);

	cong_editor_line_manager_construct (CONG_EDITOR_LINE_MANAGER (line_manager),
					    widget);

	PRIVATE (line_manager)->editor_node = editor_node;
	PRIVATE (line_manager)->outer_line_manager = outer_line_manager;
	PRIVATE (line_manager)->outer_iter = outer_iter;

	PRIVATE (line_manager)->hash_of_line_to_data = g_hash_table_new_full (g_direct_hash,
									      g_direct_equal,
									      NULL,
									      hash_value_destroy_func);

	return CONG_EDITOR_LINE_MANAGER (line_manager);
}


CongEditorLineManager*
cong_editor_line_manager_span_wrapper_new (CongEditorWidget3 *widget,
					   CongEditorNode *editor_node,
					   CongEditorLineManager *outer_line_manager,
					   CongEditorLineIter *outer_iter)
{
	return CONG_EDITOR_LINE_MANAGER (cong_editor_line_manager_span_wrapper_construct (g_object_new (CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER_TYPE, NULL),
											  widget,
											  editor_node,
											  outer_line_manager,
											  outer_iter));	
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

static CongEditorLineIter*
make_iter (CongEditorLineManager *line_manager)
{
	CongEditorLineManagerSpanWrapper *span_wrapper = CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager);

	g_assert (PRIVATE (span_wrapper)->outer_iter);
	return CONG_EDITOR_LINE_ITER (cong_editor_line_iter_span_wrapper_new (span_wrapper,
									      PRIVATE (span_wrapper)->outer_iter));
}

static void
begin_line (CongEditorLineManager *line_manager,
	    CongEditorLineIter *line_iter)
{
	CongEditorLineManagerSpanWrapper *span_wrapper = CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager);
	CongEditorLineIterSpanWrapper *span_wrapper_iter = CONG_EDITOR_LINE_ITER_SPAN_WRAPPER (line_iter);

	span_wrapper_iter->current_prev_area_on_line = NULL;

	/* Delegate: */
	cong_editor_line_manager_begin_line (PRIVATE (span_wrapper)->outer_line_manager,
					     span_wrapper_iter->outer_iter);
}

static void
add_to_line (CongEditorLineManager *line_manager,
	     CongEditorLineIter *line_iter,
	     CongEditorArea *area)
{
	CongEditorLineManagerSpanWrapper *span_wrapper = CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager);
	CongEditorLineIterSpanWrapper *span_wrapper_iter = CONG_EDITOR_LINE_ITER_SPAN_WRAPPER (line_iter);
	CongEditorAreaLine *line;
	PerLineData *line_data;

	line = cong_editor_line_iter_get_line (line_iter);

	if (NULL==line) {
		/* Need to start a new line:*/
		cong_editor_line_manager_begin_line (line_manager,
						     line_iter);
		line = cong_editor_line_iter_get_line (line_iter);
	}
	g_assert (line);

	line_data = get_data_for_line (span_wrapper,
				       line);
	
	/* Potentially we're adding the first area to this line; create the span wrapper area for the line: */
	if (NULL==line_data) {
		line_data = g_new0 (PerLineData, 1);
		g_hash_table_insert (PRIVATE (span_wrapper)->hash_of_line_to_data,
				     line,
				     line_data);

		line_data->span_area = make_span_area (span_wrapper);
		line_data->inner_line = make_inner_line (span_wrapper);

		/* FIXME:  what about inserting at the start of a line which already has stuff on it? */
		span_wrapper_iter->current_prev_area_on_line = NULL;

		cong_editor_line_manager_add_to_line (PRIVATE (span_wrapper)->outer_line_manager,
						      span_wrapper_iter->outer_iter,
						      line_data->span_area);

		cong_editor_area_container_add_child (CONG_EDITOR_AREA_CONTAINER (line_data->span_area),
						      line_data->inner_line);
	}
	g_assert (line_data->span_area);
	g_assert (line_data->inner_line);

	if (span_wrapper_iter->current_prev_area_on_line) {
		cong_editor_area_container_add_child_after (CONG_EDITOR_AREA_CONTAINER (line_data->inner_line),
							    area,
							    span_wrapper_iter->current_prev_area_on_line);
	} else {
		/* FIXME: shouldn't we be adding to the start, not the end? */
		cong_editor_area_container_add_child (CONG_EDITOR_AREA_CONTAINER (line_data->inner_line),
						      area);
	}

	span_wrapper_iter->current_prev_area_on_line = area;
}

static void 
end_line (CongEditorLineManager *line_manager,
	  CongEditorLineIter *line_iter)
{
	CongEditorLineManagerSpanWrapper *span_wrapper = CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager);
	CongEditorLineIterSpanWrapper *span_wrapper_iter = CONG_EDITOR_LINE_ITER_SPAN_WRAPPER (line_iter);

	span_wrapper_iter->current_prev_area_on_line = NULL;
	
	/* Delegate: */
	return cong_editor_line_manager_end_line (PRIVATE (span_wrapper)->outer_line_manager,
						  span_wrapper_iter->outer_iter);
}

static gint
get_line_width (CongEditorLineManager *line_manager,
		CongEditorLineIter *line_iter)
{
	CongEditorLineManagerSpanWrapper *span_wrapper = CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager);
	CongEditorLineIterSpanWrapper *span_wrapper_iter = CONG_EDITOR_LINE_ITER_SPAN_WRAPPER (line_iter);

	/* Delegate: */
	return cong_editor_line_manager_get_line_width (PRIVATE (span_wrapper)->outer_line_manager,
							span_wrapper_iter->outer_iter);
}

static gint
get_current_indent (CongEditorLineManager *line_manager,
		    CongEditorLineIter *line_iter)
{
	CongEditorLineManagerSpanWrapper *span_wrapper = CONG_EDITOR_LINE_MANAGER_SPAN_WRAPPER (line_manager);
	CongEditorLineIterSpanWrapper *span_wrapper_iter = CONG_EDITOR_LINE_ITER_SPAN_WRAPPER (line_iter);

	/* Delegate: */
	return cong_editor_line_manager_get_current_indent (PRIVATE (span_wrapper)->outer_line_manager,
							    span_wrapper_iter->outer_iter);
}

static void
hash_value_destroy_func (gpointer data)
{
	PerLineData *per_node_data = (PerLineData*)data;

	g_free (per_node_data);
}

static PerLineData*
get_data_for_line (CongEditorLineManagerSpanWrapper *span_wrapper,
		   CongEditorAreaLine *line)
{
	return (PerLineData*)g_hash_table_lookup (PRIVATE (span_wrapper)->hash_of_line_to_data,
						  line);
}
