/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-node-entity-ref.c
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
#include "cong-editor-node-entity-ref.h"
#include <libgnome/gnome-macros.h>
#include "cong-eel.h"

#include "cong-editor-area-entity-ref.h"

#define PRIVATE(x) ((x)->private)

struct CongEditorNodeEntityRefDetails
{
	int dummy;
};

static CongEditorArea*
generate_block_area (CongEditorNode *editor_node);

/* Exported function definitions: */
GNOME_CLASS_BOILERPLATE(CongEditorNodeEntityRef, 
			cong_editor_node_entity_ref,
			CongEditorNode,
			CONG_EDITOR_NODE_TYPE );

static void
cong_editor_node_entity_ref_class_init (CongEditorNodeEntityRefClass *klass)
{

	CongEditorNodeClass *node_klass = CONG_EDITOR_NODE_CLASS(klass);

	node_klass->generate_block_area = generate_block_area;
}

static void
cong_editor_node_entity_ref_instance_init (CongEditorNodeEntityRef *node_entity_ref)
{
	node_entity_ref->private = g_new0(CongEditorNodeEntityRefDetails,1);
}

CongEditorNodeEntityRef*
cong_editor_node_entity_ref_construct (CongEditorNodeEntityRef *editor_node_entity_ref,
					CongEditorWidget3* editor_widget,
					CongNodePtr node,
					CongEditorNode *traversal_parent)
{
	cong_editor_node_construct (CONG_EDITOR_NODE (editor_node_entity_ref),
				    editor_widget,
				    node,
				    traversal_parent);	
	return editor_node_entity_ref;
}

CongEditorNode*
cong_editor_node_entity_ref_new (CongEditorWidget3 *widget,
				  CongNodePtr node,
				  CongEditorNode *traversal_parent)
{
#if DEBUG_EDITOR_NODE_LIFETIMES
	g_message("cong_editor_node_entity_ref_new()");
#endif
	return CONG_EDITOR_NODE( cong_editor_node_entity_ref_construct (g_object_new (CONG_EDITOR_NODE_ENTITY_REF_TYPE, NULL),
									 widget,
									 node,
									 traversal_parent)
				 );
}

static CongEditorArea*
generate_block_area (CongEditorNode *editor_node)
{
	CongEditorArea *new_area;

	g_return_val_if_fail (editor_node, NULL);

	new_area = cong_editor_area_entity_ref_new (cong_editor_node_get_widget (editor_node),
						    cong_editor_node_get_node (editor_node)->name);

	return new_area;
}