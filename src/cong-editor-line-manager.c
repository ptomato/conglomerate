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
#include "cong-editor-line-iter.h"
#include "cong-editor-area.h"
#include "cong-editor-node.h"
#include "cong-editor-creation-record.h"
#include "cong-eel.h"

struct CongEditorLineManagerPrivate
{
	CongEditorWidget3* widget;

	/* Mapping from editor_node to per_node data: */
	GHashTable *hash_of_editor_node_to_data;
};

CONG_DEFINE_CLASS_BEGIN (CongEditorLineManager, cong_editor_line_manager, CONG_EDITOR_LINE_MANAGER, GObject, G_TYPE_OBJECT)
CONG_DEFINE_CLASS_END ()
CONG_DEFINE_EMPTY_DISPOSE(cong_editor_line_manager)

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

	/* Record of insertion position at the start of this node: */
	CongEditorLineIter *start_line_iter;

	/* Insertion position for areas _after_ this node: */
	CongEditorLineIter *end_line_iter;

	CongEditorCreationRecord *creation_record;
};

static void
hash_value_destroy_func (gpointer data);

static PerNodeData*
get_data_for_node (CongEditorLineManager *line_manager,
		   CongEditorNode *editor_node);


void
cong_editor_line_manager_construct (CongEditorLineManager *line_manager,
				    CongEditorWidget3 *widget)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));

	PRIVATE (line_manager)->widget = widget;
	PRIVATE (line_manager)->hash_of_editor_node_to_data = g_hash_table_new_full (g_direct_hash,
										     g_direct_equal,
										     NULL,
										     hash_value_destroy_func);
}

CongEditorWidget3*
cong_editor_line_manager_get_widget (CongEditorLineManager *line_manager)
{
	g_return_val_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager), NULL);

	return PRIVATE (line_manager)->widget;
}

void 
cong_editor_line_manager_add_node (CongEditorLineManager *line_manager,
				   CongEditorNode *editor_node)
{
	CongEditorNode *editor_node_prev = cong_editor_node_get_prev (editor_node);
	PerNodeData *per_node_data;

	/* Set up per_node_data for the new editor node: */
	{
		per_node_data = g_new0 (PerNodeData, 1);

		/* Set up the line_iter: */
		{
			if (editor_node_prev) {
				PerNodeData *per_node_data_prev = get_data_for_node (line_manager,
										     editor_node_prev);
				
				per_node_data->start_line_iter = cong_editor_line_iter_clone (per_node_data_prev->end_line_iter);
			} else {
				/* We have a start node; use the factory method: */
				per_node_data->start_line_iter = CONG_EEL_CALL_METHOD_WITH_RETURN_VALUE (CONG_EDITOR_LINE_MANAGER_CLASS,
													 line_manager,
													 make_iter,
													 (line_manager));
			}
			per_node_data->end_line_iter = cong_editor_line_iter_clone (per_node_data->start_line_iter);
			g_assert (per_node_data->start_line_iter);
			g_assert (per_node_data->end_line_iter);
		}

		/* Set up creation record: */
		per_node_data->creation_record = cong_editor_creation_record_new (line_manager);
		
		/* We're done; add this to the hash table: */
		g_hash_table_insert (PRIVATE (line_manager)->hash_of_editor_node_to_data,
				     editor_node,
				     per_node_data);
	}
		
	/* Invoke "create_areas" method for node: */
	{
		CongAreaCreationInfo creation_info;

		creation_info.line_manager = line_manager;
		creation_info.creation_record = per_node_data->creation_record;
		creation_info.line_iter = per_node_data->end_line_iter; /* note that this will be modified */

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

void
cong_editor_line_manager_remove_node (CongEditorLineManager *line_manager,
				      CongEditorNode *editor_node)
{
	PerNodeData *per_node_data;

	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (IS_CONG_EDITOR_NODE (editor_node));

	per_node_data = get_data_for_node (line_manager,
					   editor_node);
	g_assert (per_node_data);

#if 1
	/* Delete all areas recorded for this node: */
	cong_editor_creation_record_undo_changes (per_node_data->creation_record);
	g_object_unref (G_OBJECT (per_node_data->creation_record));

	/* Potentially update successor nodes' area creation info, recreating areas as necessary, which may trigger further updates: */
	{
		/* FIXME: unwritten */

	}
#else
	/* Go backwards, deleting all sibling's areas (recursaively?) including this one, then regenerate siblings forwards? */ 
#endif

	/* Remove from hash: */
	{
		g_hash_table_remove (PRIVATE (line_manager)->hash_of_editor_node_to_data,
				     editor_node);
	}
}

void
cong_editor_line_manager_begin_line (CongEditorLineManager *line_manager,
				     CongEditorCreationRecord *creation_record,
				     CongEditorLineIter *line_iter)
{
	CongEditorLineIter *iter_before;
	CongEditorLineIter *iter_after;

	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (IS_CONG_EDITOR_CREATION_RECORD (creation_record));
	g_return_if_fail (line_iter);

	iter_before = cong_editor_line_iter_clone (line_iter);

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      begin_line, 
			      (line_manager, creation_record, line_iter));

	iter_after = cong_editor_line_iter_clone (line_iter);

	/* Record the change: */
	cong_editor_creation_record_add_change (creation_record,
						CONG_EDITOR_CREATION_EVENT_BEGIN_LINE,
						iter_before,
						iter_after);

	/* We should now have a line at the iter: */
	g_assert (cong_editor_line_iter_get_line (line_iter));
}

void
cong_editor_line_manager_add_to_line (CongEditorLineManager *line_manager,
				      CongEditorCreationRecord *creation_record,
				      CongEditorLineIter *line_iter,
				      CongEditorArea *area)
{
	CongEditorLineIter *iter_before;
	CongEditorLineIter *iter_after;

	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (IS_CONG_EDITOR_CREATION_RECORD (creation_record));
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
							   creation_record,
							   line_iter);
		}
	}

	iter_before = cong_editor_line_iter_clone (line_iter);

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      add_to_line, 
			      (line_manager, creation_record, line_iter, area));

	iter_after = cong_editor_line_iter_clone (line_iter);

	/* Record the change: */
	cong_editor_creation_record_add_change (creation_record,
						CONG_EDITOR_CREATION_EVENT_ADD_AREA,
						iter_before,
						iter_after);
}

void
cong_editor_line_manager_end_line (CongEditorLineManager *line_manager,
				   CongEditorCreationRecord *creation_record,
				   CongEditorLineIter *line_iter)
{
	CongEditorLineIter *iter_before;
	CongEditorLineIter *iter_after;

	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (IS_CONG_EDITOR_CREATION_RECORD (creation_record));
	g_return_if_fail (line_iter);

	iter_before = cong_editor_line_iter_clone (line_iter);

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      end_line,
			      (line_manager, creation_record, line_iter));

	iter_after = cong_editor_line_iter_clone (line_iter);

	/* Record the change: */
	cong_editor_creation_record_add_change (creation_record,
						CONG_EDITOR_CREATION_EVENT_END_LINE,
						iter_before,
						iter_after);
}

#if 1
static const gchar*
get_string_for_event_type (enum CongEditorCreationEvent event)
{
	switch (event) {
	default: g_assert_not_reached ();
	case CONG_EDITOR_CREATION_EVENT_BEGIN_LINE:
		return "BEGIN_LINE";

	case CONG_EDITOR_CREATION_EVENT_END_LINE:
		return "END_LINE";

	case CONG_EDITOR_CREATION_EVENT_ADD_AREA:
		return "ADD_AREA";
	}
}

void
cong_editor_line_manager_undo_change (CongEditorLineManager *line_manager,
				      enum CongEditorCreationEvent event,
				      CongEditorLineIter *iter_before,
				      CongEditorLineIter *iter_after)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (iter_before);
	g_return_if_fail (iter_after);

	g_message ("%s::undo_change (%s)", G_OBJECT_TYPE_NAME (G_OBJECT (line_manager)), get_string_for_event_type (event));

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      undo_change, 
			      (line_manager, 
			       event,
			       iter_before, 
			       iter_after));


}
#else
void
cong_editor_line_manager_delete_areas (CongEditorLineManager *line_manager,
				       CongEditorLineIter *start_iter,
				       CongEditorLineIter *end_iter)
{
	g_return_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager));
	g_return_if_fail (start_iter);
	g_return_if_fail (end_iter);

	CONG_EEL_CALL_METHOD (CONG_EDITOR_LINE_MANAGER_CLASS,
			      line_manager,
			      delete_areas, 
			      (line_manager, 
			       start_iter, 
			       end_iter));


}
#endif

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
	g_return_val_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager), 0);
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

static void
hash_value_destroy_func (gpointer data)
{
	PerNodeData *per_node_data = (PerNodeData*)data;

	g_object_unref (G_OBJECT (per_node_data->start_line_iter));
	g_object_unref (G_OBJECT (per_node_data->end_line_iter));

	g_free (per_node_data);
}

static PerNodeData*
get_data_for_node (CongEditorLineManager *line_manager,
		   CongEditorNode *editor_node)
{	
	g_return_val_if_fail (IS_CONG_EDITOR_LINE_MANAGER (line_manager), NULL);
	g_return_val_if_fail (IS_CONG_EDITOR_NODE (editor_node), NULL);

	return (PerNodeData*)g_hash_table_lookup (PRIVATE (line_manager)->hash_of_editor_node_to_data,
						  editor_node);
}

