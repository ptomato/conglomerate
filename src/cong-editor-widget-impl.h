/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-widget-impl.h
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


/*
 * Implementation details for the CongEditorWidget
 */
#ifndef __CONG_EDITOR_WIDGET_IMPL_H__
#define __CONG_EDITOR_WIDGET_IMPL_H__

G_BEGIN_DECLS

/* currently implemented as a GtkDrawingArea with user_data "details" pointing to a CongEditorWidgetDetails */

/*
  The idea: we have a tree of CongElementEditors (or should these be CongNodeEditors?)

  We pipe the tree notifications in at the root and let each editor manage its children, creating and destroying as necessary.

  Initially we have two types of ElementEditor: span text edit, and structural wrapper.  Could have more, and also support plugins.

  Other children (handy as "use cases") might be:
  - table editor
  - MathML editor

  Issues:  where are the children stored?  perhaps in the derived classes?

  Idea:  Render and click methods to be passed offset coords by parent; this way we make all of this state extrinsic?
*/
typedef struct CongElementEditor CongElementEditor;
typedef struct CongElementEditorClass CongElementEditorClass;
typedef struct CongDummyElementEditor CongDummyElementEditor;
typedef struct CongSectionHeadEditor CongSectionHeadEditor;
typedef struct CongSpanTextEditor CongSpanTextEditor;

#define CONG_ELEMENT_EDITOR(x) ((CongElementEditor*)(x))
#define CONG_DUMMY_ELEMENT_EDITOR(x) ((CongDummyElementEditor*)(x))
#define CONG_SECTION_HEAD_EDITOR(x) ((CongSectionHeadEditor*)(x))
#define CONG_SPAN_TEXT_EDITOR(x) ((CongSpanTextEditor*)(x))

struct CongElementEditor
{
	const CongElementEditorClass *klass;

	CongEditorWidget *widget;

	/* An element editor actually applies to a range of sibling elements; could be a single node: */
	CongNodePtr first_node;
	CongNodePtr final_node;

	GdkRectangle window_area; /* allocated area in window space */
	GtkRequisition requisition;
};

struct CongElementEditorClass
{
	/* Methods? */
	const gchar *name;
	void (*on_recursive_delete)(CongElementEditor *element_editor);
	void (*on_recursive_self_test)(CongElementEditor *element_editor);
	gboolean (*on_document_event)(CongElementEditor *element_editor, CongDocumentEvent *event);
	void (*get_size_requisition)(CongElementEditor *element_editor, int width_hint);
	void (*allocate_child_space)(CongElementEditor *element_editor);
	void (*recursive_render)(CongElementEditor *element_editor, const GdkRectangle *window_rect);
	void (*on_button_press)(CongElementEditor *element_editor, GdkEventButton *event);
};

void cong_element_editor_recursive_render(CongElementEditor *element_editor, const GdkRectangle *window_rect);
void cong_element_editor_on_button_press(CongElementEditor *element_editor, GdkEventButton *event);

typedef struct CongEditorWidgetDetails CongEditorWidgetDetails;
typedef struct CongEditorWidgetView CongEditorWidgetView;
#define CONG_EDITOR_WIDGET_VIEW(x) ((CongEditorWidgetView*)(x))

struct CongEditorWidgetDetails
{
	CongEditorWidget *widget;
	CongEditorWidgetView *view;

	CongElementEditor *root_editor;
};

/* The widget "owns" a CongView, which in turn, holds a ptr back to its widget. */
struct CongEditorWidgetView
{
	CongView view;

	CongEditorWidget *widget;
};

/* Macro for getting details of a widget; this will eventually be a simple field lookup */
#define GET_DETAILS(editor_widget) ((CongEditorWidgetDetails*)(g_object_get_data(G_OBJECT(editor_widget), "details")))

CongNodePtr cong_element_editor_get_first_node(CongElementEditor *element_editor);
CongNodePtr cong_element_editor_get_final_node(CongElementEditor *element_editor);
gboolean cong_element_editor_responsible_for_node(CongElementEditor *element_editor, CongNodePtr node);
void cong_element_editor_recursive_delete(CongElementEditor *element_editor);
void cong_element_editor_recursive_self_test(CongElementEditor *element_editor);
gboolean cong_element_editor_on_document_event(CongElementEditor *element_editor, CongDocumentEvent *event);
void cong_element_editor_get_size_requisition(CongElementEditor *element_editor, int width_hint);
void cong_element_editor_set_allocation(CongElementEditor *element_editor,
					gint x,
					gint y,
					gint width,
					gint height);
/* these are in window coords */


CongElementEditor *cong_dummy_element_editor_new(CongEditorWidget *widget, CongNodePtr node);
CongElementEditor *cong_section_head_editor_new(CongEditorWidget *widget, CongNodePtr node);
CongElementEditor *cong_span_text_editor_new(CongEditorWidget *widget, CongNodePtr first_node, CongNodePtr final_node);

CongFont*
cong_span_text_editor_get_font(CongSpanTextEditor *span_text, enum CongFontRole role);

G_END_DECLS

#endif