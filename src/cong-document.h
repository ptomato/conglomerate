/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-document.h
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

#ifndef __CONG_DOCUMENT_H__
#define __CONG_DOCUMENT_H__

G_BEGIN_DECLS

/**
   CongDocument functions
 */

/* takes ownership of xml_doc; the new CongDocument is created with a reference count of 1; any views constructed of the document will increment its reference count */
CongDocument*
cong_document_new_from_xmldoc(xmlDocPtr xml_doc, CongDispspec *ds, const gchar *url);

/* FIXME: eventually this will be a GObject, and the ref/unref functions will be redundant: */
void
cong_document_ref(CongDocument *doc);

void
cong_document_unref(CongDocument *doc);

xmlDocPtr
cong_document_get_xml(CongDocument *doc);

CongNodePtr
cong_document_get_root(CongDocument *doc);

CongDispspec*
cong_document_get_dispspec(CongDocument *doc);

gchar*
cong_document_get_filename(CongDocument *doc);
/* caller is responsible for freeeing */

gchar*
cong_document_get_full_uri(CongDocument *doc);
/* caller is responsible for freeeing */

gchar*
cong_document_get_parent_uri(CongDocument *doc);
/* caller is responsible for freeeing */

const CongXMLChar*
cong_document_get_dtd_public_identifier(CongDocument *doc);
/* NULL if not present */

void
cong_document_save(CongDocument *doc, 
		   const char* filename,
		   GtkWindow *parent_window);

gboolean
cong_document_is_modified(CongDocument *doc);

void
cong_document_set_modified(CongDocument *doc, gboolean modified);

void
cong_document_set_primary_window(CongDocument *doc, CongPrimaryWindow *window);

void 
cong_document_set_url(CongDocument *doc, const gchar *url);

glong
cong_document_get_seconds_since_last_save_or_load(const CongDocument *doc);

/* MVC-related methods on the document: */
void cong_document_coarse_update(CongDocument *doc);
void cong_document_node_make_orphan(CongDocument *doc, CongNodePtr node);
void cong_document_node_add_after(CongDocument *doc, CongNodePtr node, CongNodePtr older_sibling);
void cong_document_node_add_before(CongDocument *doc, CongNodePtr node, CongNodePtr younger_sibling);
void cong_document_node_set_parent(CongDocument *doc, CongNodePtr node, CongNodePtr adoptive_parent); /* added to end of child list */
void cong_document_node_set_text(CongDocument *doc, CongNodePtr node, const xmlChar *new_content);
void cong_document_tag_remove(CongDocument *doc, CongNodePtr x);
void cong_document_on_selection_change(CongDocument *doc);
void cong_document_on_cursor_change(CongDocument *doc);

/* These functions internally ref and unref the document:, as well as adding the view to the doc's list */
void cong_document_register_view(CongDocument *doc, CongView *view);
void cong_document_unregister_view(CongDocument *doc, CongView *view);

/* cursor and selections are now properties of the document: */
CongCursor* cong_document_get_cursor(CongDocument *doc);
CongSelection* cong_document_get_selection(CongDocument *doc);

/** 
    Every node within a document has an implied language, although the logic to determine 
    this may depend on the dispspec, on attributes of the nodes, and perhaps even on user prefererences (and hence the value may change unexpectedly): 

    FIXME: what are the ownership/ref count semantics of this function?
*/
PangoLanguage*
cong_document_get_language_for_node(CongDocument *doc, 
				    CongNodePtr node);

/* Various UI hooks: */
void cong_document_cut_selection(CongDocument *doc);
void cong_document_copy_selection(CongDocument *doc);
void cong_document_paste_selection(CongDocument *doc, GtkWidget *widget);
void cong_document_view_source(CongDocument *doc);


G_END_DECLS

#endif
