/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-node.h
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

#ifndef __CONG_EDITOR_NODE_H__
#define __CONG_EDITOR_NODE_H__

#include "cong-document.h"
#include "cong-editor-widget.h"

G_BEGIN_DECLS

#define DEBUG_EDITOR_NODE_LIFETIMES 0

#define CONG_EDITOR_NODE_TYPE	      (cong_editor_node_get_type ())
#define CONG_EDITOR_NODE(obj)         G_TYPE_CHECK_INSTANCE_CAST (obj, CONG_EDITOR_NODE_TYPE, CongEditorNode)
#define CONG_EDITOR_NODE_CLASS(klass) G_TYPE_CHECK_CLASS_CAST (klass, CONG_EDITOR_NODE_TYPE, CongEditorNodeClass)
#define IS_CONG_EDITOR_NODE(obj)      G_TYPE_CHECK_INSTANCE_TYPE (obj, CONG_EDITOR_NODE_TYPE)

typedef struct CongEditorNodeDetails CongEditorNodeDetails;

typedef struct CongAreaCreationInfo CongAreaCreationInfo;

struct CongAreaCreationInfo
{
	CongEditorLineManager *line_manager;

	/* Position at which to add areas (so that when a node is inserted between two existing nodes, 
	   we can add the areas between their areas.  Note, though that subsequent areas may well need regenerating
	   since the word-wrap will often start at a different place).

	   The line_iter will get modified as actions are performed on it; it represents the "current" position.
	*/
	CongEditorLineIter *line_iter;
};

/**
 * CongEditorNode
 * 
 * A CongEditorNode is a per-editor-widget GObject, and represents a node that is visited in a traversal of the xml tree.
 * Hence there is generally a 1-1 mapping between xml nodes and CongEditorNodes.  When an xmlnode is added or removed from the tree, 
 * even temporarily, then a corresponding CongEditorNode is added/removed.
 *
 * However.  if you have an entity ref, then the nodes below the entity decls get visited multiple times in a traversal,
 * hence there are multiple CongEditorNodes for such nodes, one for below the entity decl, and one below every entity ref.
 *
 * In order to support this every editor node know both which xml node it represents, and which "traversal parent" it has.
 * So although it is generally the case that the traversal parent is the parent of the xml node, it is NOT always the case.
 * 
 * The motivating example is for the immediate children of entity references, for which the parent of the xml node is the
 * entity declaration, not the entity reference.  In this case, the traversal parent IS the entity reference node.
 *
 * The traversal parent is stored as a pointer to the relevant CongEditorNode, rather than a CongNodePtr.
 *
 */
struct CongEditorNode
{
	GObject object;

	CongEditorNodeDetails *priv;
};

struct CongEditorNodeClass
{
	GObjectClass klass;

	/* Methods? */
#if 1
	void (*create_areas) (CongEditorNode *editor_node,
			      const CongAreaCreationInfo *creation_info);
#else
	/* Simplistic interface for now: */
	CongEditorArea* (*generate_block_area) (CongEditorNode *editor_node);

	CongEditorLineFragments* (*generate_line_areas_recursive) (CongEditorNode *editor_node,
								   gint line_width,
								   gint initial_indent);
#endif

	void (*line_regeneration_required) (CongEditorNode *editor_node);
	
	enum CongFlowType (*get_flow_type) (CongEditorNode *editor_node);
};

GType
cong_editor_node_get_type (void);

CongEditorNode*
cong_editor_node_construct (CongEditorNode *editor_node,
			    CongEditorWidget3* widget,
			    CongTraversalNode *traversal_node);

/*
 * Factory method for creating editor nodes of an appropriate sub-class
 */
CongEditorNode*
cong_editor_node_manufacture (CongEditorWidget3* widget,
			      CongTraversalNode *traversal_node);

CongEditorWidget3*
cong_editor_node_get_widget (CongEditorNode *editor_node);

CongDocument*
cong_editor_node_get_document (CongEditorNode *editor_node);

CongNodePtr
cong_editor_node_get_node (CongEditorNode *editor_node);

CongTraversalNode*
cong_editor_node_get_traversal_node (CongEditorNode *editor_node);

CongEditorNode*
cong_editor_node_get_traversal_parent (CongEditorNode *editor_node);

gboolean
cong_editor_node_is_selected (CongEditorNode *editor_node);

/*
  This function should only be called by the editor widget internals:
 */
void
cong_editor_node_private_set_selected (CongEditorNode *editor_node,
				       gboolean is_selected);


#if 1
void 
cong_editor_node_create_areas (CongEditorNode *editor_node,
			       const CongAreaCreationInfo *creation_info);
#else
CongEditorArea*
cong_editor_node_generate_block_area (CongEditorNode *editor_node);

/* This doesn't actually add the areas anywhere; this has to be done separately (to avoid reparenting issues when the span tags embellish their children's lines: */
CongEditorLineFragments*
cong_editor_node_generate_line_areas_recursive (CongEditorNode *editor_node,
						gint line_width,
						gint initial_indent);
#endif

void
cong_editor_node_line_regeneration_required (CongEditorNode *editor_node);

enum CongFlowType
cong_editor_node_get_flow_type (CongEditorNode *editor_node);

/**
 * cong_editor_node_is_referenced_entity_decl
 * @editor_node:
 *
 *  Entity decls can be visited in the tree both below the DTD node, and below each entity ref node that references them.
 *  This function returns TRUE iff the editor_node represents the latter case.
 *  This is useful e.g. if you want to know the "effective siblings" of the node, which should be the other entity decls in the
 *  former case, and should be NULL in the latter case.
 *
 * Returns:
 */
gboolean
cong_editor_node_is_referenced_entity_decl (CongEditorNode *editor_node);

/* May not always succeed; if called during the node creation, the relevant editor_node might not have been created yet: */
CongEditorNode*
cong_editor_node_get_prev (CongEditorNode *editor_node);

CongEditorNode*
cong_editor_node_get_next (CongEditorNode *editor_node);

#if 1
CongEditorLineManager*
cong_editor_node_get_line_manager_for_children (CongEditorNode *editor_node);

void
cong_editor_node_set_line_manager_for_children (CongEditorNode *editor_node,
						CongEditorLineManager *line_manager);
#else
/* Get the child policy; should only be needed by internals of widget implementation: */
CongEditorChildPolicy*
cong_editor_node_get_child_policy (CongEditorNode *editor_node);

/* Set the child policy; should only be needed by internals of widget implementation: */
void
cong_editor_node_set_child_policy (CongEditorNode *editor_node,
				   CongEditorChildPolicy *child_policy);

/* Get the parent's child policy; should only be needed by internals of widget implementation: */
CongEditorChildPolicy*
cong_editor_node_get_parents_child_policy (CongEditorNode *editor_node);

/* Set the parent's child policy; should only be needed by internals of widget implementation: */
void
cong_editor_node_set_parents_child_policy (CongEditorNode *editor_node,
					   CongEditorChildPolicy *child_policy);
#endif


/**
 * cong_editor_node_create_block_area:
 *
 * @editor_node: the #CongEditorNode
 * @creation_info:
 * @block_area: the block-style area that will represent this editor node.
 * @allow_children: Can this node have children?  If so, then the block area must be a subclass of #CongEditorAreaContainer
 *
 * Utility function for implementing the create_areas function for node subclasses that expect to create a single block area.
 */
void
cong_editor_node_create_block_area (CongEditorNode *editor_node,
				    const CongAreaCreationInfo *creation_info,
				    CongEditorArea *block_area,
				    gboolean allow_children);

/* Utility placeholder function for node subclasses that don't really create an area themselves (e.g. for the document, for DTDs etc;
   might be empty, might not; not sure yet. */
void
cong_editor_node_empty_create_area (CongEditorNode *editor_node,
				    const CongAreaCreationInfo *creation_info,
				    gboolean allow_children);

G_END_DECLS

#endif
