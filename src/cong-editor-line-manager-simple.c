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
#include "cong-editor-node.h"
#include "cong-editor-area.h"
#include "cong-editor-area-line.h"
#include "cong-editor-area-composer.h"

#include "cong-eel.h"

#define HACKED_WIDTH (150)
static void 
add_node (CongEditorLineManager *line_manager,
	  CongEditorNode *node);

static void 
remove_node (CongEditorLineManager *line_manager,
	     CongEditorNode *node);

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

/* Definition of CongEditorLineIter is here for now: */
struct CongEditorLineIter
{
	CongEditorAreaLine *current_line;
	CongEditorAreaLine *current_prev_line;
	CongEditorArea *current_prev_area;
};

/* Data stored about each editor node: */
typedef struct PerNodeData PerNodeData;
struct PerNodeData
{
#if 0
	/* Cache of data that the areas of this node were created with; if changes occur then the areas may need to be regenerated: */
	gint line_width;
	gint line_indent;
	/* FIXME: perhaps these ought to be added to struct CongAreaCreationInfo ? */
#endif

	/* Insertion position for areas _after_ this node: */
	CongEditorLineIter trailing_line_iter;
};

static void
my_value_destroy_func (gpointer data)
{
	PerNodeData *per_node_data = (PerNodeData*)data;

	g_free (per_node_data);
}

static PerNodeData*
get_data_for_node (CongEditorLineManagerSimple *simple,
		   CongEditorNode *editor_node);

struct CongEditorLineManagerSimplePrivate
{
	CongEditorAreaLines *area_lines;

#if 1
	/* Mapping from editor_node to iter (for insertion after): */
	GHashTable *hash_of_editor_node_to_data;
#else
	/* Position after end of final node's areas: */
	CongEditorLineIter end_line_iter;
#endif
};

CONG_DEFINE_CLASS_BEGIN (CongEditorLineManagerSimple, cong_editor_line_manager_simple, CONG_EDITOR_LINE_MANAGER_SIMPLE, CongEditorLineManager, CONG_EDITOR_LINE_MANAGER_TYPE)
{
	CongEditorLineManagerClass *lm_klass = CONG_EDITOR_LINE_MANAGER_CLASS (klass);

	lm_klass->add_node = add_node;
	lm_klass->remove_node = remove_node;

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
					   CongEditorAreaLines *area_lines)
{
	g_return_val_if_fail (IS_CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager), NULL);
	g_return_val_if_fail (IS_CONG_EDITOR_AREA_LINES (area_lines), NULL);

	cong_editor_line_manager_construct (CONG_EDITOR_LINE_MANAGER (line_manager),
					    widget);

	PRIVATE (line_manager)->area_lines = area_lines;

	PRIVATE (line_manager)->hash_of_editor_node_to_data = g_hash_table_new_full (g_direct_hash,
										     g_direct_equal,
										     NULL,
										     my_value_destroy_func);

	

	return CONG_EDITOR_LINE_MANAGER (line_manager);
}

CongEditorLineManager*
cong_editor_line_manager_simple_new (CongEditorWidget3 *widget,
				     CongEditorAreaLines *area_lines)
{
	g_return_val_if_fail (IS_CONG_EDITOR_AREA (area_lines), NULL);

	return CONG_EDITOR_LINE_MANAGER (cong_editor_line_manager_simple_construct (g_object_new (CONG_EDITOR_LINE_MANAGER_SIMPLE_TYPE, NULL),
										    widget,
										    area_lines));
}

static void
copy_iter (CongEditorLineIter *dst,
	   const CongEditorLineIter *src)
{
	g_assert (dst);
	g_assert (src);

	*dst = *src;
}


static void 
add_node (CongEditorLineManager *line_manager,
	  CongEditorNode *editor_node)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);
	CongEditorNode *editor_node_prev = cong_editor_node_get_prev (editor_node);
	PerNodeData *per_node_data;

	/* Set up per_node_data for the new editor node: */
	{
		per_node_data = g_new0 (PerNodeData, 1);

		if (editor_node_prev) {
			PerNodeData *per_node_data_prev = get_data_for_node (simple,
									     editor_node_prev);
			
			copy_iter (&per_node_data->trailing_line_iter,
				   &per_node_data_prev->trailing_line_iter);
		} else {
			/* We have a start node: */
			per_node_data->trailing_line_iter.current_line = NULL;
			per_node_data->trailing_line_iter.current_prev_line = NULL;
			per_node_data->trailing_line_iter.current_prev_area = NULL;
		}
		
		/* ...and add to the hash table: */
		g_hash_table_insert (PRIVATE (simple)->hash_of_editor_node_to_data,
				     editor_node,
				     per_node_data);
	}
		
	/* Invoke "create_areas" method for node: */
	{
		CongAreaCreationInfo creation_info;

		creation_info.line_manager = line_manager;
		creation_info.line_iter = &per_node_data->trailing_line_iter; /* note that this will be modified */

		CONG_EEL_CALL_METHOD (CONG_EDITOR_NODE_CLASS,
				      editor_node,
				      create_areas,
				      (editor_node, &creation_info));
	}

	/* Potentially update successor nodes' area creation info, recreating areas as necessary, which may trigger further updates: */
	{
		/* FIXME: unwritten */
	}
}

static void 
remove_node (CongEditorLineManager *line_manager,
	     CongEditorNode *node)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);

	/* FIXME: unimplemented */
	g_assert_not_reached ();

	/* Potentially update successor nodes' area creation info, recreating areas as necessary, which may trigger further updates: */
	{
		/* FIXME: unwritten */
	}

	/* Remove from hash: */
	{
		g_hash_table_remove (PRIVATE (simple)->hash_of_editor_node_to_data,
				     node);
	}
}

static void 
begin_line (CongEditorLineManager *line_manager,
	    CongEditorLineIter *line_iter)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);
	CongEditorAreaLine *new_line;

	new_line = CONG_EDITOR_AREA_LINE (cong_editor_area_line_new (cong_editor_line_manager_get_widget (line_manager),
								     HACKED_WIDTH)); /* FIXME */
	if (line_iter->current_line) {
		line_iter->current_prev_line = line_iter->current_line;
	}
	line_iter->current_line = new_line;
	line_iter->current_prev_area = NULL;

	if (line_iter->current_prev_line) {
		cong_editor_area_composer_pack_after (CONG_EDITOR_AREA_COMPOSER (PRIVATE (simple)->area_lines),
						      CONG_EDITOR_AREA (new_line),
						      CONG_EDITOR_AREA (line_iter->current_prev_line),
						      FALSE,
						      TRUE,
						      0);
	} else {
		cong_editor_area_composer_pack_end (CONG_EDITOR_AREA_COMPOSER (PRIVATE (simple)->area_lines),
						    CONG_EDITOR_AREA (new_line),
						    FALSE,
						    TRUE,
						    0);
	}
}

static void
add_to_line (CongEditorLineManager *line_manager,
	     CongEditorLineIter *line_iter,
	     CongEditorArea *area)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);

	/* Ensure we have a line to add the area to: */
	if (NULL==line_iter->current_line) {
		begin_line (line_manager,
			    line_iter);
	}

	if (line_iter->current_prev_area) {
		cong_editor_area_container_add_child_after (CONG_EDITOR_AREA_CONTAINER (line_iter->current_line),
							    area,
							    line_iter->current_prev_area);
	} else {
		cong_editor_area_container_add_child (CONG_EDITOR_AREA_CONTAINER (line_iter->current_line),
						      area);
	}

	line_iter->current_prev_area = area;
}

static void 
end_line (CongEditorLineManager *line_manager,
	  CongEditorLineIter *line_iter)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);

	if (line_iter->current_line) {
		line_iter->current_prev_line = line_iter->current_line;
	}
	line_iter->current_line = NULL;
}

static gint
get_line_width (CongEditorLineManager *line_manager,
		CongEditorLineIter *line_iter)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);

	return HACKED_WIDTH; /* FIXME */
}

static gint
get_current_indent (CongEditorLineManager *line_manager,
		    CongEditorLineIter *line_iter)
{
	CongEditorLineManagerSimple *simple = CONG_EDITOR_LINE_MANAGER_SIMPLE (line_manager);

#if 1
	if (line_iter->current_line) {
		cong_editor_area_line_get_width_used (line_iter->current_line);
	} else {
		return 0;
	}
#else
	return cong_editor_area_get_requisition (CONG_EDITOR_AREA (PRIVATE (simple)->current_line),
						 GTK_ORIENTATION_HORIZONTAL,
						 HACKED_WIDTH); /* FIXME */
#endif
}


static PerNodeData*
get_data_for_node (CongEditorLineManagerSimple *simple,
		   CongEditorNode *editor_node)
{	
	g_return_val_if_fail (IS_CONG_EDITOR_LINE_MANAGER_SIMPLE (simple), NULL);
	g_return_val_if_fail (IS_CONG_EDITOR_NODE (editor_node), NULL);

	return (PerNodeData*)g_hash_table_lookup (PRIVATE (simple)->hash_of_editor_node_to_data,
						  editor_node);
}

