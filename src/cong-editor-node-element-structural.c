/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-node-element-structural.c
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
#include "cong-editor-node-element-structural.h"
#include <libgnome/gnome-macros.h>
#include "cong-eel.h"

#include "cong-editor-area-border.h"
#include "cong-editor-area-structural-tag.h"
#include "cong-dispspec.h"

#define PRIVATE(x) ((x)->private)

struct CongEditorNodeElementStructuralDetails
{
	int dummy;
};

static CongEditorArea*
add_area (CongEditorNode *editor_node,
	  CongEditorAreaContainer *parent_area);


/* Exported function definitions: */
GNOME_CLASS_BOILERPLATE(CongEditorNodeElementStructural, 
			cong_editor_node_element_structural,
			CongEditorNodeElement,
			CONG_EDITOR_NODE_ELEMENT_TYPE );

static void
cong_editor_node_element_structural_class_init (CongEditorNodeElementStructuralClass *klass)
{
	CongEditorNodeClass *node_klass = CONG_EDITOR_NODE_CLASS(klass);

	node_klass->add_area = add_area;
}

static void
cong_editor_node_element_structural_instance_init (CongEditorNodeElementStructural *node_element_structural)
{
	node_element_structural->private = g_new0(CongEditorNodeElementStructuralDetails,1);
}

CongEditorNodeElementStructural*
cong_editor_node_element_structural_construct (CongEditorNodeElementStructural *editor_node_element_structural,
					       CongEditorWidget3* editor_widget,
					       CongNodePtr node)
{
	cong_editor_node_element_construct (CONG_EDITOR_NODE_ELEMENT (editor_node_element_structural),
					    editor_widget,
					    node);

	return editor_node_element_structural;
}

CongEditorNode*
cong_editor_node_element_structural_new (CongEditorWidget3* widget,
					 CongNodePtr node)
{
	g_message("cong_editor_node_element_structural_new(%s)", node->name);

	return CONG_EDITOR_NODE( cong_editor_node_element_structural_construct
				 (g_object_new (CONG_EDITOR_NODE_ELEMENT_STRUCTURAL_TYPE, NULL),
				  widget,
				  node));
}

static CongEditorArea*
add_area (CongEditorNode *editor_node,
	  CongEditorAreaContainer *parent_area)
{
	CongEditorArea *outer_area;
	CongEditorArea *inner_area;
	CongDispspecElement *ds_element;
	GdkPixbuf *pixbuf;
	gchar *title_text;

	g_return_val_if_fail (editor_node, NULL);
	g_return_val_if_fail (parent_area, NULL);

	ds_element = cong_editor_node_element_get_dispspec_element (CONG_EDITOR_NODE_ELEMENT(editor_node));

	pixbuf = cong_dispspec_element_get_icon (ds_element);

	title_text = cong_dispspec_element_get_section_header_text (ds_element,
								    cong_editor_node_get_node (editor_node));



	outer_area = cong_editor_area_border_new (cong_editor_node_get_widget (editor_node),
						  5);

	inner_area = cong_editor_area_structural_tag_new (cong_editor_node_get_widget (editor_node),
							  ds_element,
							  pixbuf,
							  title_text);

	if (pixbuf) {
		g_object_unref (G_OBJECT(pixbuf));
	}

	g_free (title_text);

	cong_editor_area_container_add_child (parent_area,
					      outer_area);
	cong_editor_area_container_add_child (CONG_EDITOR_AREA_CONTAINER(outer_area),
					      inner_area);

	return inner_area;
}