/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-widget.c
 *
 * Copyright (C) 2002 David Malcolm
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
#include "cong-editor-widget-impl.h"

/* Prototypes of the handler functions: */
static void on_document_coarse_update(CongView *view);
static void on_document_node_make_orphan(CongView *view, CongNodePtr node);
static void on_document_node_add_after(CongView *view, CongNodePtr node, CongNodePtr older_sibling);
static void on_document_node_add_before(CongView *view, CongNodePtr node, CongNodePtr younger_sibling);
static void on_document_node_set_parent(CongView *view, CongNodePtr node, CongNodePtr adoptive_parent); /* added to end of child list */
static void on_document_node_set_text(CongView *view, CongNodePtr node, const xmlChar *new_content);

#define DEBUG_EDITOR_WIDGET_VIEW 1

/* Definitions of the handler functions: */
static void on_document_coarse_update(CongView *view)
{
	CongEditorWidgetView *editor_widget_view;

	g_return_if_fail(view);

	#if DEBUG_EDITOR_WIDGET_VIEW
	g_message("CongEditorWidgetView - on_document_coarse_update\n");
	#endif

	editor_widget_view = CONG_EDITOR_WIDGET_VIEW(view);

	/* Ignore for now */
}

static void on_document_node_make_orphan(CongView *view, CongNodePtr node)
{
	CongEditorWidgetView *editor_widget_view;

	g_return_if_fail(view);
	g_return_if_fail(node);

	#if DEBUG_EDITOR_WIDGET_VIEW
	g_message("CongEditorWidgetView - on_document_node_make_orphan\n");
	#endif

	editor_widget_view = CONG_EDITOR_WIDGET_VIEW(view);

	/* FIXME: unimplemented */

}

static void on_document_node_add_after(CongView *view, CongNodePtr node, CongNodePtr older_sibling)
{
	CongEditorWidgetView *editor_widget_view;

	g_return_if_fail(view);
	g_return_if_fail(node);
	g_return_if_fail(older_sibling);

	#if DEBUG_EDITOR_WIDGET_VIEW
	g_message("CongEditorWidgetView - on_document_node_add_after\n");
	#endif

	editor_widget_view = CONG_EDITOR_WIDGET_VIEW(view);

	/* FIXME: unimplemented */
}

static void on_document_node_add_before(CongView *view, CongNodePtr node, CongNodePtr younger_sibling)
{
	CongEditorWidgetView *editor_widget_view;

	g_return_if_fail(view);
	g_return_if_fail(node);
	g_return_if_fail(younger_sibling);

	#if DEBUG_EDITOR_WIDGET_VIEW
	g_message("CongEditorWidgetView - on_document_node_add_before\n");
	#endif

	editor_widget_view = CONG_EDITOR_WIDGET_VIEW(view);

	/* FIXME: unimplemented */

}

static void on_document_node_set_parent(CongView *view, CongNodePtr node, CongNodePtr adoptive_parent)
{
	CongEditorWidgetView *editor_widget_view;

	g_return_if_fail(view);
	g_return_if_fail(node);
	g_return_if_fail(adoptive_parent);

	#if DEBUG_EDITOR_WIDGET_VIEW
	g_message("CongEditorWidgetView - on_document_node_set_parent\n");
	#endif

	editor_widget_view = CONG_EDITOR_WIDGET_VIEW(view);

	/* FIXME: unimplemented */

}

static void on_document_node_set_text(CongView *view, CongNodePtr node, const xmlChar *new_content)
{
	CongEditorWidgetView *editor_widget_view;

	g_return_if_fail(view);
	g_return_if_fail(node);
	g_return_if_fail(new_content);

	#if DEBUG_EDITOR_WIDGET_VIEW
	g_message("CongEditorWidgetView - on_document_node_set_text\n");
	#endif

	editor_widget_view = CONG_EDITOR_WIDGET_VIEW(view);

	/* FIXME: unimplemented */

}


/* Event handlers for widget: */
static gboolean expose_event_handler(GtkWidget *w, GdkEventExpose *event, gpointer user_data)
{
	CongDocument *doc;
	CongEditorWidget *editor_widget = CONG_EDITOR_WIDGET(w);
	CongEditorWidgetDetails* details = GET_DETAILS(editor_widget);

	g_message("expose_event_handler");

	doc = cong_editor_widget_get_document(editor_widget);

	/* Fill the rectangle with the background colour: */
	gdk_draw_rectangle(GDK_DRAWABLE(w->window),
			   w->style->white_gc,
			   TRUE, /* gint filled, */
			   event->area.x,
			   event->area.y,
			   event->area.width,
			   event->area.height);	

	/* Render the ElementEditors: */
	cong_element_editor_recursive_render(details->root_editor, &event->area);

	/* For now we render all of them */

	return TRUE;
}

static gboolean configure_event_handler(GtkWidget *w, GdkEventConfigure *event, gpointer user_data)
{
	CongEditorWidget *editor_widget = CONG_EDITOR_WIDGET(w);
	CongEditorWidgetDetails* details = GET_DETAILS(editor_widget);

	g_message("configure_event_handler");

	/* Pass all of the allocation to root editor; this will recursively allocate space to its children: */
	cong_element_editor_set_allocation(details->root_editor, 
					   event->x,
					   event->y,
					   event->width,
					   event->height);

	return TRUE;
}

static gboolean button_press_event_handler(GtkWidget *w, GdkEventButton *event, gpointer user_data)
{
	CongEditorWidget *editor_widget = CONG_EDITOR_WIDGET(w);
	CongEditorWidgetDetails* details = GET_DETAILS(editor_widget);

	g_message("button_press_event_handler");

	cong_element_editor_on_button_press(details->root_editor, event);

	return TRUE;
}

#if 0
void recursively_populate_ui(CongEditorView *editor_view,
			     CongNodePtr x, 
			     int collapsed)
{
	CongNodePtr x_orig;
	GtkWidget *sub = NULL;
	CongDocument *doc;
	CongDispspec *ds;

	g_return_if_fail(editor_view);
	
	doc = editor_view->view.doc;
	ds = cong_document_get_dispspec(doc);
      	
	x_orig = x;
	
	x = cong_node_first_child(x);
	if (!x) return;

	for ( ; x; )
	{
		enum CongNodeType node_type = cong_node_type(x);
		const char *name = xml_frag_name_nice(x);

		/* g_message("Examining frag %s\n",name); */

		if (node_type == CONG_NODE_TYPE_ELEMENT)
		{
			CongDispspecElement* element = cong_dispspec_lookup_element(ds, name);

			if (element) {
				if (cong_dispspec_element_is_structural(element)) {
					if (cong_dispspec_element_collapseto(element)) {
						gtk_box_pack_start(GTK_BOX(root), xv_fragment_head(ds, x), FALSE, TRUE, 0);
						
						/* Recurse here: */
						cong_editor_recursively_populate_ui(editor_view, x, root, TRUE);
						
						gtk_box_pack_start(GTK_BOX(root), xv_fragment_tail(ds, x), FALSE, TRUE, 0);
					} else {
						/* New structural element */
						CongSectionHead *section_head;
						GtkWidget *poot;
						GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
						gtk_widget_show(hbox);
						
						gtk_box_pack_start(GTK_BOX(root), hbox, FALSE, TRUE, 0);
						gtk_box_pack_start(GTK_BOX(hbox), xv_section_vline_and_space(ds, cong_node_parent(x)), FALSE, TRUE, 0);
						xv_style_r(hbox, style_white);
						section_head = cong_section_head_new(doc, x);
						poot = section_head->vbox;
						gtk_box_pack_start(GTK_BOX(hbox), poot, TRUE, TRUE, 0);
								
						/* Recurse here: */
						cong_editor_recursively_populate_ui(editor_view, x, poot, FALSE);
								
						sub = xv_section_tail(ds, x);
						xv_style_r(sub, style_white);
						gtk_box_pack_start(GTK_BOX(poot), sub, FALSE, TRUE, 0);
					}
				} else if (cong_dispspec_element_is_span(element) ||
					   CONG_ELEMENT_TYPE_INSERT == cong_dispspec_element_type(element)) {
				        /* New editor window */
				
					sub = xv_section_data(x, doc, ds, collapsed);
					if (sub) {
						gtk_box_pack_start(GTK_BOX(root), sub, FALSE, TRUE, 0);
						xv_style_r(sub, style_white);
					}
				
					x = xv_editor_elements_skip(x, ds);
				} else if (CONG_ELEMENT_TYPE_EMBED_EXTERNAL_FILE==cong_dispspec_element_type(element)) {
					CongSectionHead *section_head;
					GtkWidget *poot;
					GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
					gtk_widget_show(hbox);

					gtk_box_pack_start(GTK_BOX(root), hbox, FALSE, TRUE, 0);
					gtk_box_pack_start(GTK_BOX(hbox), xv_section_vline_and_space(ds, cong_node_parent(x)), FALSE, TRUE, 0);
					xv_style_r(hbox, style_white);
					section_head = cong_section_head_new(doc,x);
					poot = section_head->vbox;
					gtk_box_pack_start(GTK_BOX(hbox), poot, TRUE, TRUE, 0);
				        /* xv_style_r(poot, style_white); */
				
					sub = xv_section_embedded(doc, x,ds,collapsed);
					gtk_box_pack_start(GTK_BOX(poot), sub, FALSE, TRUE, 0);
					
					sub = xv_section_tail(ds, x);
				        /* xv_style_r(sub, style_white); */
					gtk_box_pack_start(GTK_BOX(poot), sub, FALSE, TRUE, 0);
				}
			}
		}
		else if (node_type == CONG_NODE_TYPE_TEXT)
		{
			/* New editor window */

			sub = xv_section_data(x, doc, ds, collapsed);
			if (sub)
			{
				gtk_box_pack_start(GTK_BOX(root), sub, FALSE, TRUE, 0);
				xv_style_r(sub, style_white);
			}
			
			x = xv_editor_elements_skip(x, ds);
		}

		if (x) {
			x = cong_node_next(x);
		}
	}

	xv_style_r(sub, style_white);
}
#endif

void populate_widget(CongEditorWidget *widget)
{
#if 0
	GdkColor gcol;
	GtkWidget *w;
	int i;
	CongNodePtr x;

	CongDocument *doc;
	CongDispspec *displayspec;

	g_return_if_fail(widget);
	
	doc = editor_view->view.doc;
	displayspec = cong_document_get_dispspec(doc);
		
	x = cong_document_get_root(doc);

	for ( ; x; x = cong_node_next(x))
	{
		enum CongNodeType type = cong_node_type(x);

		const char *name = xml_frag_name_nice(x);

		g_message("examining frag \"%s\", type = %s\n", name, cong_node_type_description(type));
		
		if (type == CONG_NODE_TYPE_ELEMENT && cong_dispspec_element_structural(displayspec, name))
		{
			/* New element */
			CongSectionHead *section_head;
			GtkWidget* head;
			section_head = cong_section_head_new(doc, x);
			head = section_head->vbox;
#error
			gtk_box_pack_start(GTK_BOX(editor_view->inner), head, TRUE, TRUE, 0);
			
			cong_editor_recursively_populate_ui(editor_view, x, head, FALSE);

			w = xv_section_tail(displayspec, x);
			xv_style_r(w, style_white);
			gtk_box_pack_start(GTK_BOX(head), w, FALSE, TRUE, 0);
		}
	}
#else
	/* FIXME: unimplemented */
	CongEditorWidget *editor_widget = CONG_EDITOR_WIDGET(widget);
	CongEditorWidgetDetails *details = GET_DETAILS(editor_widget);
	CongDocument *doc;
	CongDispspec *displayspec;
	CongSectionHeadEditor *section_head;

	g_return_if_fail(widget);
	
	doc = details->view->view.doc;
	displayspec = cong_document_get_dispspec(doc);

	section_head = cong_section_head_editor_new(widget, cong_document_get_root(doc));

	details->root_editor = CONG_ELEMENT_EDITOR(section_head);
#endif
}


/* Public interface: */

GtkWidget* cong_editor_widget_new(CongDocument *doc)
{
	CongEditorWidget *widget;
	CongEditorWidgetDetails *details;
	CongEditorWidgetView *view;

	g_return_val_if_fail(doc, NULL);

	widget = GTK_DRAWING_AREA(gtk_drawing_area_new());

	details = g_new0(CongEditorWidgetDetails,1);
	view = g_new0(CongEditorWidgetView,1);

	g_object_set_data(G_OBJECT(widget),
			  "details",
			  details);
	details->widget = widget;
	details->view = view;
	view->widget = widget;
	
	view->view.doc = doc;
	view->view.klass = g_new0(CongViewClass,1);
	view->view.klass->on_document_coarse_update = on_document_coarse_update;
	view->view.klass->on_document_node_make_orphan = on_document_node_make_orphan;
	view->view.klass->on_document_node_add_after = on_document_node_add_after;
	view->view.klass->on_document_node_add_before = on_document_node_add_before;
	view->view.klass->on_document_node_set_parent = on_document_node_set_parent;
	view->view.klass->on_document_node_set_text = on_document_node_set_text;

	cong_document_register_view( doc, CONG_VIEW(view) );

	gtk_signal_connect(GTK_OBJECT(widget), 
			   "expose_event",
			   (GtkSignalFunc) expose_event_handler, 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(widget), 
			   "configure_event",
			   (GtkSignalFunc) configure_event_handler, 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(widget), 
			   "button_press_event",
			   (GtkSignalFunc) button_press_event_handler, 
			   NULL);

	gtk_widget_set_events(GTK_WIDGET(widget), GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

	populate_widget(widget);

	/* Recursively update all the size requisitions: */
	cong_element_editor_get_size_requisition(details->root_editor);

	gtk_widget_set_size_request(GTK_WIDGET(widget),
				    details->root_editor->requisition.width,
				    details->root_editor->requisition.height);

	return GTK_WIDGET(widget);
}

CongDocument *cong_editor_widget_get_document(CongEditorWidget *editor_widget)
{
	CongEditorWidgetDetails *details;

	g_return_val_if_fail(editor_widget, NULL);

	details = GET_DETAILS(editor_widget);

	return details->view->view.doc;
}

