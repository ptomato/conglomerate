/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-area-flow-holder-blocks.c
 *
 * Copyright (C) 2003 David Malcolm
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
#include "cong-editor-area-flow-holder-blocks.h"
#include "cong-editor-area-flow-holder-inlines.h"
#include "cong-editor-area-flow-holder-single.h"
#include <libgnome/gnome-macros.h>

#include "cong-editor-area-composer.h"

#define PRIVATE(x) ((x)->private)

struct CongEditorAreaFlowHolderBlocksDetails
{
	CongEditorAreaComposer *outer_compose;
	GHashTable *hash_of_doc_node_to_child_flow_holder;
};

/* Method implementation prototypes: */
static gint
calc_requisition (CongEditorArea *area, 
		  GtkOrientation orientation,
		  int width_hint);

static void
allocate_child_space (CongEditorArea *area);

static CongEditorArea*
for_all (CongEditorArea *editor_area, 
	 CongEditorAreaCallbackFunc func, 
	 gpointer user_data);

static CongEditorChildPolicy*
insert_areas_for_node (CongEditorAreaFlowHolder *area_flow_holder,
		       CongEditorNode *editor_node);
static void
remove_areas_for_node (CongEditorAreaFlowHolder *area_flow_holder,
		       CongEditorNode *editor_node);

/* Other utilities: */
static void	
insert_child_flow_holder_into_composer (CongEditorAreaFlowHolderBlocks *area_flow_holder_blocks,
					CongEditorAreaFlowHolder *child_flow_holder,
					CongEditorNode *editor_node);

/* GObject boilerplate stuff: */
GNOME_CLASS_BOILERPLATE(CongEditorAreaFlowHolderBlocks, 
			cong_editor_area_flow_holder_blocks,
			CongEditorAreaFlowHolder,
			CONG_EDITOR_AREA_FLOW_HOLDER_TYPE );

static void
cong_editor_area_flow_holder_blocks_class_init (CongEditorAreaFlowHolderBlocksClass *klass)
{
	CongEditorAreaClass *area_klass = CONG_EDITOR_AREA_CLASS(klass);
	CongEditorAreaFlowHolderClass *flow_holder_klass = CONG_EDITOR_AREA_FLOW_HOLDER_CLASS(klass);

	area_klass->calc_requisition = calc_requisition;
	area_klass->allocate_child_space = allocate_child_space;
	area_klass->for_all = for_all;

	flow_holder_klass->insert_areas_for_node = insert_areas_for_node;
	flow_holder_klass->remove_areas_for_node = remove_areas_for_node;
}

static void
cong_editor_area_flow_holder_blocks_instance_init (CongEditorAreaFlowHolderBlocks *area_flow_holder_blocks)
{
	area_flow_holder_blocks->private = g_new0(CongEditorAreaFlowHolderBlocksDetails,1);
}

/* Exported function definitions: */
CongEditorArea*
cong_editor_area_flow_holder_blocks_construct (CongEditorAreaFlowHolderBlocks *area_flow_holder_blocks,
					CongEditorWidget3 *editor_widget)
{
	cong_editor_area_flow_holder_construct (CONG_EDITOR_AREA_FLOW_HOLDER(area_flow_holder_blocks),
						editor_widget);

	PRIVATE(area_flow_holder_blocks)->outer_compose = CONG_EDITOR_AREA_COMPOSER( cong_editor_area_composer_new (editor_widget,
													      GTK_ORIENTATION_VERTICAL,
													      0));
	PRIVATE(area_flow_holder_blocks)->hash_of_doc_node_to_child_flow_holder = g_hash_table_new (NULL,
												    NULL);

	cong_editor_area_protected_postprocess_add_internal_child (CONG_EDITOR_AREA (area_flow_holder_blocks),
								   CONG_EDITOR_AREA (PRIVATE(area_flow_holder_blocks)->outer_compose));

	cong_editor_area_protected_set_parent (CONG_EDITOR_AREA (PRIVATE(area_flow_holder_blocks)->outer_compose),
					       CONG_EDITOR_AREA (area_flow_holder_blocks));

	return CONG_EDITOR_AREA (area_flow_holder_blocks);
}

CongEditorArea*
cong_editor_area_flow_holder_blocks_new (CongEditorWidget3 *editor_widget)
{
#if DEBUG_EDITOR_AREA_LIFETIMES
	g_message("cong_editor_area_flow_holder_blocks_new");
#endif

	return cong_editor_area_flow_holder_blocks_construct
		(g_object_new (CONG_EDITOR_AREA_FLOW_HOLDER_BLOCKS_TYPE, NULL),
		 editor_widget);
}

CongEditorAreaFlowHolder*
cong_editor_area_flow_holder_get_child_flow_holder_for_node (CongEditorAreaFlowHolderBlocks *area_flow_holder_blocks,
							     CongNodePtr doc_node)
{
	g_return_val_if_fail (area_flow_holder_blocks, NULL);
	g_return_val_if_fail (doc_node, NULL);

	return CONG_EDITOR_AREA_FLOW_HOLDER (g_hash_table_lookup (PRIVATE(area_flow_holder_blocks)->hash_of_doc_node_to_child_flow_holder,
								  doc_node)
					     );
}

/* Method implementation definitions: */
static gint
calc_requisition (CongEditorArea *area, 
		  GtkOrientation orientation,
		  int width_hint)
{
	const GtkRequisition *child_req;

	CongEditorAreaFlowHolderBlocks *area_flow_holder_blocks = CONG_EDITOR_AREA_FLOW_HOLDER_BLOCKS(area);

	if (PRIVATE(area_flow_holder_blocks)->outer_compose) {

		return cong_editor_area_get_requisition (CONG_EDITOR_AREA(PRIVATE(area_flow_holder_blocks)->outer_compose),
							 orientation,
							 width_hint);
	} else {
		return 0;
	}
}

static void
allocate_child_space (CongEditorArea *area)
{
	CongEditorAreaFlowHolderBlocks *area_flow_holder_blocks = CONG_EDITOR_AREA_FLOW_HOLDER_BLOCKS(area);

	if (PRIVATE(area_flow_holder_blocks)->outer_compose) {
		const GdkRectangle *rect = cong_editor_area_get_window_coords(area);

		cong_editor_area_set_allocation (CONG_EDITOR_AREA(PRIVATE(area_flow_holder_blocks)->outer_compose),
						 rect->x,
						 rect->y,
						 rect->width,
						 rect->height);
	}
}

static CongEditorArea*
for_all (CongEditorArea *editor_area, 
	 CongEditorAreaCallbackFunc func, 
	 gpointer user_data)
{
	CongEditorAreaFlowHolderBlocks *area_flow_holder_blocks = CONG_EDITOR_AREA_FLOW_HOLDER_BLOCKS(editor_area);

	if (PRIVATE(area_flow_holder_blocks)->outer_compose) {
		if ((*func)(CONG_EDITOR_AREA(PRIVATE(area_flow_holder_blocks)->outer_compose), user_data)) {
			return CONG_EDITOR_AREA(PRIVATE(area_flow_holder_blocks)->outer_compose);
		}
	}

	return NULL;
}

static CongEditorChildPolicy*
insert_areas_for_node (CongEditorAreaFlowHolder *area_flow_holder,
		       CongEditorNode *editor_node)
{
	CongEditorAreaFlowHolderBlocks *area_flow_holder_blocks = CONG_EDITOR_AREA_FLOW_HOLDER_BLOCKS(area_flow_holder);
	CongEditorAreaFlowHolder *child_flow_holder = NULL;
	CongNodePtr doc_node;

	enum CongFlowType flow_type;

	flow_type = cong_editor_node_get_flow_type (editor_node);

	doc_node = cong_editor_node_get_node(editor_node);

	/* Find the appropriate child flow-holder to add the node to, creating if necessary: */
	switch (flow_type) {
	default: g_assert_not_reached();
	case CONG_FLOW_TYPE_BLOCK:
		{
			/* Create a "single" flow-holder to hold the areas of the editor_node: */
			child_flow_holder = CONG_EDITOR_AREA_FLOW_HOLDER(cong_editor_area_flow_holder_single_new (cong_editor_node_get_widget (editor_node)));

			insert_child_flow_holder_into_composer (area_flow_holder_blocks,
								child_flow_holder, 
								editor_node);

			/* FIXME: what about the case where a new block node is inserted between existing inline nodes; we need to split the existing inline flow into two separate ones... */

		}
		break;

	case CONG_FLOW_TYPE_INLINE:
		{
			CongEditorNode *editor_node_prev, *editor_node_next;

			editor_node_prev = cong_editor_node_get_prev (editor_node);
			editor_node_next = cong_editor_node_get_next (editor_node);

			/* Find an inline flow, or create one: */
			if (editor_node_prev) {
				if (CONG_FLOW_TYPE_INLINE==cong_editor_node_get_flow_type(editor_node_prev)) {
					/* Then we've found an inline flow to add ourselves to: */
					child_flow_holder = cong_editor_area_flow_holder_get_child_flow_holder_for_node (area_flow_holder_blocks,
															 doc_node->prev);
					g_assert(child_flow_holder);
				}
			} else {
				if (editor_node_next) {
					if (CONG_FLOW_TYPE_INLINE==cong_editor_node_get_flow_type(editor_node_next)) {
						/* then add ourselves in front of this node in its inline flow */
						child_flow_holder = cong_editor_area_flow_holder_get_child_flow_holder_for_node (area_flow_holder_blocks,
																 doc_node->next);
						g_assert(child_flow_holder);
					}
				}
			}
			
			/* If we haven't found an existing neighbouring inline flow to join; create a new one: */
			if (NULL==child_flow_holder)
			{
				CongNodePtr doc_node_iter;

				child_flow_holder = CONG_EDITOR_AREA_FLOW_HOLDER(cong_editor_area_flow_holder_inlines_new (cong_editor_node_get_widget (editor_node)));

				insert_child_flow_holder_into_composer (area_flow_holder_blocks,
									child_flow_holder, 
									editor_node);
			}
			
		}
		break;
	}

	/* OK - we now know which inline flow this editor_area is going in; register this in the hash table: */
	{
		g_assert(child_flow_holder);
		
		g_hash_table_insert (PRIVATE(area_flow_holder_blocks)->hash_of_doc_node_to_child_flow_holder,
				     doc_node,
				     child_flow_holder);
	}
	
	/* Delegate: Add to the child flow-holder: */
	{
		return cong_editor_area_flow_holder_insert_areas_for_node (child_flow_holder,
									   editor_node);
	}
	
}

static void
remove_areas_for_node (CongEditorAreaFlowHolder *area_flow_holder,
		       CongEditorNode *editor_node)
{
	CongEditorAreaFlowHolderBlocks *area_flow_holder_blocks = CONG_EDITOR_AREA_FLOW_HOLDER_BLOCKS(area_flow_holder);
	CongEditorAreaFlowHolder *child_flow_holder;
	CongNodePtr doc_node;
	
	doc_node = cong_editor_node_get_node(editor_node);

	child_flow_holder = g_hash_table_lookup (PRIVATE(area_flow_holder_blocks)->hash_of_doc_node_to_child_flow_holder,
						 doc_node);
	g_assert(child_flow_holder);
	
	/* Delegate to child flow: */
	cong_editor_area_flow_holder_remove_areas_for_node (CONG_EDITOR_AREA_FLOW_HOLDER(child_flow_holder),
							    editor_node);

	g_hash_table_remove (PRIVATE(area_flow_holder_blocks)->hash_of_doc_node_to_child_flow_holder,
			     doc_node);
	
	/* FIXME: What if the child flow is now empty??? */
	/* FIXME: What if two adjacent inline flows should now be merged? */

#if 0
	flow_type = cong_editor_node_get_flow_type (editor_node);

	switch (flow_type) {
	default: g_assert_not_reached();
	case CONG_FLOW_TYPE_BLOCK:
		{
			cong_editor_area_container_remove_child (CONG_EDITOR_AREA_CONTAINER(PRIVATE(area_flow_holder_blocks)->outer_compose),
								 area);

			/* FIXME: What if two adjacent inline flows should be merged? */
		}
		break;

	case CONG_FLOW_TYPE_INLINE:
		{
		}
		break;
	}
#endif

}

/* Other utilities: */
static void	
insert_child_flow_holder_into_composer (CongEditorAreaFlowHolderBlocks *area_flow_holder_blocks,
					CongEditorAreaFlowHolder *child_flow_holder,
					CongEditorNode *editor_node)
{
	/* Given that we've just created a new child flow-holder, we must have a distinct flow-holder for the neighbouring editor-nodes. 
	   So try the previous one... */
	CongNodePtr doc_node = cong_editor_node_get_node (editor_node);

	if (doc_node->prev) {
		CongEditorAreaFlowHolder* prev_flow_holder = cong_editor_area_flow_holder_get_child_flow_holder_for_node (area_flow_holder_blocks,
															  doc_node->prev);

		g_assert(prev_flow_holder);
		g_assert(prev_flow_holder!=child_flow_holder);

		cong_editor_area_composer_pack_after (PRIVATE(area_flow_holder_blocks)->outer_compose,
						      CONG_EDITOR_AREA(child_flow_holder),
						      CONG_EDITOR_AREA(prev_flow_holder),
						      FALSE,
						      FALSE,
						      0);
	} else {
		/* Insert this area after any already present in the parent's insertion area: */
		cong_editor_area_composer_pack (PRIVATE(area_flow_holder_blocks)->outer_compose,
						CONG_EDITOR_AREA(child_flow_holder),
						FALSE,
						FALSE,
						0);
	}
}