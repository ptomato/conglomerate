/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-node-element-unknown.c
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
#include "cong-editor-node-element-unknown.h"
#include <libgnome/gnome-macros.h>
#include "cong-eel.h"

#include "cong-editor-area-unknown-tag.h"

#define PRIVATE(x) ((x)->private)

struct CongEditorNodeElementUnknownDetails
{
	int dummy;
};

#if 1
static void 
create_areas (CongEditorNode *editor_node,
	      const CongAreaCreationInfo *creation_info);
#else
static CongEditorArea*
generate_block_area (CongEditorNode *editor_node);
#endif

/* Exported function definitions: */
GNOME_CLASS_BOILERPLATE(CongEditorNodeElementUnknown, 
			cong_editor_node_element_unknown,
			CongEditorNodeElement,
			CONG_EDITOR_NODE_ELEMENT_TYPE );

static void
cong_editor_node_element_unknown_class_init (CongEditorNodeElementUnknownClass *klass)
{
	CongEditorNodeClass *node_klass = CONG_EDITOR_NODE_CLASS(klass);

#if 1
	node_klass->create_areas = create_areas;
#else
	node_klass->generate_block_area = generate_block_area;
#endif
}

static void
cong_editor_node_element_unknown_instance_init (CongEditorNodeElementUnknown *node_element_unknown)
{
	node_element_unknown->private = g_new0(CongEditorNodeElementUnknownDetails,1);
}

CongEditorNodeElementUnknown*
cong_editor_node_element_unknown_construct (CongEditorNodeElementUnknown *editor_node_element_unknown,
					    CongEditorWidget3* editor_widget,
					    CongTraversalNode *traversal_node)
{
	cong_editor_node_element_construct (CONG_EDITOR_NODE_ELEMENT (editor_node_element_unknown),
					    editor_widget,
					    traversal_node);

	return editor_node_element_unknown;
}

CongEditorNode*
cong_editor_node_element_unknown_new (CongEditorWidget3* widget,
				      CongTraversalNode *traversal_node)
{
#if DEBUG_EDITOR_NODE_LIFETIMES
	g_message("cong_editor_node_element_unknown_new(%s)", node->name);
#endif

	return CONG_EDITOR_NODE( cong_editor_node_element_unknown_construct
				 (g_object_new (CONG_EDITOR_NODE_ELEMENT_UNKNOWN_TYPE, NULL),
				  widget,
				  traversal_node));
}


#if 1
static void 
create_areas (CongEditorNode *editor_node,
	      const CongAreaCreationInfo *creation_info)
{
	CongEditorArea *block_area;

	g_return_if_fail (editor_node);

	block_area = cong_editor_area_unknown_tag_new (cong_editor_node_get_widget (editor_node),
						       cong_editor_node_get_node (editor_node)->name);

	/* FIXME: should this be done by the helper function? */
	cong_editor_area_connect_node_signals (block_area,
					       editor_node);

	cong_editor_node_create_block_area (editor_node,
					    creation_info,
					    block_area,
					    TRUE);
}
#else
static CongEditorArea*
generate_block_area (CongEditorNode *editor_node)
{
	CongEditorArea *new_area;

	g_return_val_if_fail (editor_node, NULL);

	new_area = cong_editor_area_unknown_tag_new (cong_editor_node_get_widget (editor_node),
						     cong_editor_node_get_node (editor_node)->name);

	cong_editor_area_connect_node_signals (new_area,
					       editor_node);

	return new_area;
}
#endif
