/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-area-unknown-tag.c
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
#include "cong-editor-area-unknown-tag.h"
#include <libgnome/gnome-macros.h>

#include "cong-app.h"
#include "cong-editor-area-text.h"
#include "cong-editor-area-composer.h"
#include "cong-editor-area-spacer.h"

#define PRIVATE(x) ((x)->private)

struct CongEditorAreaUnknownTagDetails
{
	CongEditorArea *outer_vcompose;
	CongEditorArea *inner_row;
	CongEditorArea *inner_area;
};

/* Method implementation prototypes: */
static void 
update_requisition (CongEditorArea *area, 
		    int width_hint);

static void
allocate_child_space (CongEditorArea *area);

static void 
for_all (CongEditorArea *editor_area, 
	 CongEditorAreaCallbackFunc func, 
	 gpointer user_data);

static void
add_child (CongEditorAreaContainer *area_container,
	   CongEditorArea *child);

/* GObject boilerplate stuff: */
GNOME_CLASS_BOILERPLATE(CongEditorAreaUnknownTag, 
			cong_editor_area_unknown_tag,
			CongEditorAreaBin,
			CONG_EDITOR_AREA_BIN_TYPE );

static void
cong_editor_area_unknown_tag_class_init (CongEditorAreaUnknownTagClass *klass)
{
	CongEditorAreaClass *area_klass = CONG_EDITOR_AREA_CLASS(klass);
	CongEditorAreaContainerClass *container_klass = CONG_EDITOR_AREA_CONTAINER_CLASS(klass);

	area_klass->update_requisition = update_requisition;
	area_klass->allocate_child_space = allocate_child_space;
	area_klass->for_all = for_all;

	container_klass->add_child = add_child;

}

static void
cong_editor_area_unknown_tag_instance_init (CongEditorAreaUnknownTag *area_unknown_tag)
{
	area_unknown_tag->private = g_new0(CongEditorAreaUnknownTagDetails,1);
}


/* Exported function definitions: */
CongEditorArea*
cong_editor_area_unknown_tag_construct (CongEditorAreaUnknownTag *area_unknown_tag,
					CongEditorWidget3 *editor_widget,
					const gchar *tagname)
{
	gchar *tag_string_begin;
	gchar *tag_string_end;

	const gchar *colour_string = cong_ui_get_colour_string(CONG_NODE_TYPE_ELEMENT);

	cong_editor_area_bin_construct (CONG_EDITOR_AREA_BIN(area_unknown_tag),
					editor_widget);

#if 0
	tag_string_begin = g_strdup_printf("<span foreground=\"%s\">&lt;%s&gt;</span>",colour_string, tagname);
	tag_string_end = g_strdup_printf("<span foreground=\"%s\">/&lt;%s&gt;</span>",colour_string, tagname);
#else
	tag_string_begin = g_strdup_printf("<%s>",tagname);
	tag_string_end = g_strdup_printf("</%s>",tagname);
#endif


	PRIVATE(area_unknown_tag)->outer_vcompose = cong_editor_area_composer_new (editor_widget,
					       GTK_ORIENTATION_VERTICAL,
					       0);
	
	cong_editor_area_container_add_child ( CONG_EDITOR_AREA_CONTAINER(PRIVATE(area_unknown_tag)->outer_vcompose),
					       cong_editor_area_text_new (editor_widget,
									  cong_app_singleton()->fonts[CONG_FONT_ROLE_TITLE_TEXT], 
									  tag_string_begin)
					       );
	
	PRIVATE(area_unknown_tag)->inner_row = cong_editor_area_composer_new (editor_widget,
						   GTK_ORIENTATION_HORIZONTAL,
						   0);
	
	cong_editor_area_container_add_child ( CONG_EDITOR_AREA_CONTAINER(PRIVATE(area_unknown_tag)->outer_vcompose),
					       PRIVATE(area_unknown_tag)->inner_row);	

	cong_editor_area_container_add_child ( CONG_EDITOR_AREA_CONTAINER(PRIVATE(area_unknown_tag)->inner_row),
					       cong_editor_area_spacer_new (editor_widget,
									    GTK_ORIENTATION_HORIZONTAL,
									    50));

	PRIVATE(area_unknown_tag)->inner_area = cong_editor_area_bin_new (editor_widget);
	cong_editor_area_container_add_child ( CONG_EDITOR_AREA_CONTAINER(PRIVATE(area_unknown_tag)->inner_row),
					       PRIVATE(area_unknown_tag)->inner_area);	
	
	cong_editor_area_container_add_child ( CONG_EDITOR_AREA_CONTAINER(PRIVATE(area_unknown_tag)->outer_vcompose),
					       cong_editor_area_text_new (editor_widget,
									  cong_app_singleton()->fonts[CONG_FONT_ROLE_TITLE_TEXT], 
									  tag_string_end)
					       );

	g_free (tag_string_begin);
	g_free (tag_string_end);
	return CONG_EDITOR_AREA (area_unknown_tag);
}

CongEditorArea*
cong_editor_area_unknown_tag_new (CongEditorWidget3 *editor_widget,
				  const gchar *tagname)

{
	g_message("cong_editor_area_unknown_tag_new(%s)", tagname);

	return cong_editor_area_unknown_tag_construct
		(g_object_new (CONG_EDITOR_AREA_UNKNOWN_TAG_TYPE, NULL),
		 editor_widget,
		 tagname);
}

/* Method implementation definitions: */
static void 
update_requisition (CongEditorArea *area, 
		    int width_hint)
{
	const GtkRequisition *child_req;

	CongEditorAreaUnknownTag *unknown_tag = CONG_EDITOR_AREA_UNKNOWN_TAG(area);

	if (PRIVATE(unknown_tag)->outer_vcompose) {

		cong_editor_area_update_requisition (PRIVATE(unknown_tag)->outer_vcompose, 
						     width_hint);
		
		child_req = cong_editor_area_get_requisition (PRIVATE(unknown_tag)->outer_vcompose);
		g_assert(child_req);
		
		cong_editor_area_set_requisition (area,
						  child_req->width,
						  child_req->height);
	}
}

static void
allocate_child_space (CongEditorArea *area)
{
	CongEditorAreaUnknownTag *unknown_tag = CONG_EDITOR_AREA_UNKNOWN_TAG(area);

	if (PRIVATE(unknown_tag)->outer_vcompose) {
		const GdkRectangle *rect = cong_editor_area_get_window_coords(area);

		cong_editor_area_set_allocation (PRIVATE(unknown_tag)->outer_vcompose,
						 rect->x,
						 rect->y,
						 rect->width,
						 rect->height);
	}

}

static void 
for_all (CongEditorArea *editor_area, 
	 CongEditorAreaCallbackFunc func, 
	 gpointer user_data)
{
	CongEditorAreaUnknownTag *unknown_tag = CONG_EDITOR_AREA_UNKNOWN_TAG(editor_area);

	if (PRIVATE(unknown_tag)->outer_vcompose) {
		(*func)(PRIVATE(unknown_tag)->outer_vcompose, user_data);
	}
}

static void
add_child (CongEditorAreaContainer *area_container,
	   CongEditorArea *child)
{
	CongEditorAreaUnknownTag *unknown_tag = CONG_EDITOR_AREA_UNKNOWN_TAG(area_container);

	g_assert(PRIVATE(unknown_tag)->inner_area);

	cong_editor_area_container_add_child ( CONG_EDITOR_AREA_CONTAINER( PRIVATE(unknown_tag)->inner_area),
					       child);
}
