/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-area-composer.c
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
#include "cong-editor-area-composer.h"
#include <libgnome/gnome-macros.h>

#define PRIVATE(x) ((x)->private)

struct CongEditorAreaComposerDetails
{
	GList *list_of_child_details;
	GtkOrientation orientation;
	guint spacing;
};


typedef struct CongEditorAreaComposerChildDetails CongEditorAreaComposerChildDetails;

struct CongEditorAreaComposerChildDetails
{
	CongEditorArea *child;
	gboolean expand;
	gboolean fill;
	guint extra_padding;
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
GNOME_CLASS_BOILERPLATE(CongEditorAreaComposer, 
			cong_editor_area_composer,
			CongEditorAreaContainer,
			CONG_EDITOR_AREA_CONTAINER_TYPE );

static void
cong_editor_area_composer_class_init (CongEditorAreaComposerClass *klass)
{
	CongEditorAreaClass *area_klass = CONG_EDITOR_AREA_CLASS(klass);
	CongEditorAreaContainerClass *container_klass = CONG_EDITOR_AREA_CONTAINER_CLASS(klass);

	area_klass->update_requisition = update_requisition;
	area_klass->allocate_child_space = allocate_child_space;
	area_klass->for_all = for_all;

	container_klass->add_child = add_child;
}

static void
cong_editor_area_composer_instance_init (CongEditorAreaComposer *area_composer)
{
	area_composer->private = g_new0(CongEditorAreaComposerDetails,1);
}

/* Exported function definitions: */
CongEditorArea*
cong_editor_area_composer_construct (CongEditorAreaComposer *area_composer,
				     CongEditorWidget3 *editor_widget,
				     GtkOrientation orientation,
				     guint spacing)
{
	cong_editor_area_container_construct (CONG_EDITOR_AREA_CONTAINER(area_composer),
					      editor_widget);

	PRIVATE(area_composer)->orientation = orientation;
	PRIVATE(area_composer)->spacing = spacing;

	return CONG_EDITOR_AREA (area_composer);
}

CongEditorArea*
cong_editor_area_composer_new (CongEditorWidget3 *editor_widget,
			       GtkOrientation orientation,
			       guint spacing)
{
	g_message("cong_editor_area_composer_new");

	return cong_editor_area_composer_construct
		(g_object_new (CONG_EDITOR_AREA_COMPOSER_TYPE, NULL),
		 editor_widget,
		 orientation,
		 spacing);
}

void
cong_editor_area_composer_pack (CongEditorAreaComposer *area_composer,
				CongEditorArea *child,
				gboolean expand,
				gboolean fill,
				guint extra_padding)
{
	CongEditorAreaComposerChildDetails *child_details;

	g_return_if_fail (IS_CONG_EDITOR_AREA_COMPOSER(area_composer));
	g_return_if_fail (IS_CONG_EDITOR_AREA(child));

	child_details = g_new0(CongEditorAreaComposerChildDetails,1);

	child_details->child = child;
	child_details->expand = expand;
	child_details->fill = fill;
	child_details->extra_padding = extra_padding;

	PRIVATE(area_composer)->list_of_child_details = g_list_append (PRIVATE(area_composer)->list_of_child_details, 
								       child_details);

	/* FIXME: need to flag things as being invalid etc... */

}


/* Method implementation definitions: */
static void 
update_requisition (CongEditorArea *area, 
		    int width_hint)
{
	CongEditorAreaComposer *area_composer = CONG_EDITOR_AREA_COMPOSER(area);
	GtkRequisition result;
	GList *iter;
	int child_count = 0;
	gint extra_padding = 0;

	g_message ("composer::update_requisition");

	result.width = 0;
	result.height = 0;

	/* Get size requisition for all kids, add in the appropriate axis: */
	for (iter = PRIVATE(area_composer)->list_of_child_details; iter; iter=iter->next) {
		CongEditorAreaComposerChildDetails *child_details;
		CongEditorArea *child;
		const GtkRequisition *child_req;

		child_details = (CongEditorAreaComposerChildDetails*)(iter->data);
		child = CONG_EDITOR_AREA(child_details->child);
		g_assert (child);

		extra_padding += child_details->extra_padding;

		cong_editor_area_update_requisition (child, 
						     width_hint);

		child_req = cong_editor_area_get_requisition (child);
		g_assert(child_req);		

		if (PRIVATE(area_composer)->orientation == GTK_ORIENTATION_HORIZONTAL) {
			result.width += child_req->width;
			if (result.height<child_req->height) {
				result.height = child_req->height;
			}
		} else {
			result.height += child_req->height;
			if (result.width<child_req->width) {
				result.width = child_req->width;
			}
		}
		
		child_count++;
	}

	if (child_count>1) {
		if (PRIVATE(area_composer)->orientation == GTK_ORIENTATION_HORIZONTAL) {
			result.width += (PRIVATE(area_composer)->spacing * (child_count-1)) + extra_padding;
		} else {
			result.height += (PRIVATE(area_composer)->spacing * (child_count-1)) + extra_padding;
		}		
	}

	cong_editor_area_set_requisition (area,
					  result.width,
					  result.height);
}

static void
allocate_child_space (CongEditorArea *area)
{
	CongEditorAreaComposer *area_composer = CONG_EDITOR_AREA_COMPOSER(area);
	GList *iter;
	gint x;
	gint y;
	const GdkRectangle *rect = cong_editor_area_get_window_coords(area);
	const GtkRequisition *this_req = cong_editor_area_get_requisition (area);
	gint total_surplus_space = 0;
	gint surplus_space_per_expandable_child = 0;
	guint num_expandable_children = 0;

	x = rect->x;
	y = rect->y;

	/* First pass: calculate the num expandable children, the amount of surplus space and hence the amount per expandable child: */
	{
		for (iter = PRIVATE(area_composer)->list_of_child_details; iter; iter=iter->next) {
			CongEditorAreaComposerChildDetails *child_details;
			CongEditorArea *child;
			
			child_details = (CongEditorAreaComposerChildDetails*)(iter->data);
			child = CONG_EDITOR_AREA(child_details->child);
			
			if (child_details->expand) {
				num_expandable_children++;
			}
		}
		
		if (PRIVATE(area_composer)->orientation == GTK_ORIENTATION_HORIZONTAL) {
			total_surplus_space = rect->width - this_req->width;
		} else {
			total_surplus_space = rect->height - this_req->height;
		}
		
		if (total_surplus_space<0) {
			total_surplus_space = 0;
		}
		
		if (num_expandable_children>0) {
			surplus_space_per_expandable_child = total_surplus_space/num_expandable_children;
			g_message("surplus space per:%i",surplus_space_per_expandable_child);
		}
	}


	/* Second pass: allocate all the space: */
	for (iter = PRIVATE(area_composer)->list_of_child_details; iter; iter=iter->next) {
		CongEditorAreaComposerChildDetails *child_details;
		CongEditorArea *child;
		const GtkRequisition *child_req;
		int child_width;
		int child_height;
		int extra_offset;

		child_details = (CongEditorAreaComposerChildDetails*)(iter->data);
		child = CONG_EDITOR_AREA(child_details->child);

		child_req = cong_editor_area_get_requisition (child);
		g_assert(child_req);

		if (PRIVATE(area_composer)->orientation == GTK_ORIENTATION_HORIZONTAL) {
			child_width = child_req->width;
			child_height = rect->height;
		} else {
			child_width = rect->width;
			child_height = child_req->height;
		}
		extra_offset = child_details->extra_padding;

		if (child_details->expand) {
			extra_offset += surplus_space_per_expandable_child;

			if (child_details->fill) {
				if (PRIVATE(area_composer)->orientation == GTK_ORIENTATION_HORIZONTAL) {
					child_width += surplus_space_per_expandable_child;
				} else {
					child_height += surplus_space_per_expandable_child;
				}
			}
		}

		cong_editor_area_set_allocation (child,
						 x,
						 y,
						 child_width,
						 child_height);
		
		if (PRIVATE(area_composer)->orientation == GTK_ORIENTATION_HORIZONTAL) {
			x += child_req->width + PRIVATE(area_composer)->spacing + extra_offset;
		} else {
			y += child_req->height + PRIVATE(area_composer)->spacing + extra_offset;
		}
	}
}

static void 
for_all (CongEditorArea *editor_area, 
	 CongEditorAreaCallbackFunc func, 
	 gpointer user_data)
{
	GList *iter;
	CongEditorAreaComposer *area_composer = CONG_EDITOR_AREA_COMPOSER(editor_area);

	for (iter = PRIVATE(area_composer)->list_of_child_details; iter; iter=iter->next) {
		CongEditorAreaComposerChildDetails *child_details;
		CongEditorArea *child;

		child_details = (CongEditorAreaComposerChildDetails*)(iter->data);
		child = CONG_EDITOR_AREA(child_details->child);
		
		(*func)(child, user_data);
	}
}


static void
add_child ( CongEditorAreaContainer *area_container,
	    CongEditorArea *child)
{
	CongEditorAreaComposer *area_composer = CONG_EDITOR_AREA_COMPOSER(area_container);

	cong_editor_area_composer_pack (area_composer,
					child,
					TRUE,
					TRUE,
					0);
	
}