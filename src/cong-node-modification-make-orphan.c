/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-node-modification-make-orphan.c
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
#include "cong-node-modification-make-orphan.h"
#include <libgnome/gnome-macros.h>
#include "cong-eel.h"

#define PRIVATE(x) ((x)->private)

struct CongNodeModificationMakeOrphanDetails
{
	/* Undo info: */
	CongNodePtr former_parent;
	CongNodePtr former_older_sibling;
};

/* Internal function declarations: */
static void
finalize (GObject *object);

static void
dispose (GObject *object);

static void
undo (CongModification *modification);

static void
redo (CongModification *modification);

/* Exported function definitions: */
GNOME_CLASS_BOILERPLATE(CongNodeModificationMakeOrphan, 
			cong_node_modification_make_orphan,
			CongNodeModification,
			CONG_NODE_MODIFICATION_TYPE );

static void
cong_node_modification_make_orphan_class_init (CongNodeModificationMakeOrphanClass *klass)
{
	G_OBJECT_CLASS (klass)->finalize = finalize;
	G_OBJECT_CLASS (klass)->dispose = dispose;

	CONG_MODIFICATION_CLASS (klass)->undo = undo;
	CONG_MODIFICATION_CLASS (klass)->redo = redo;
}

static void
cong_node_modification_make_orphan_instance_init (CongNodeModificationMakeOrphan *node)
{
	node->private = g_new0(CongNodeModificationMakeOrphanDetails,1);
}

CongNodeModificationMakeOrphan*
cong_node_modification_make_orphan_construct (CongNodeModificationMakeOrphan *node_modification_make_orphan,
					      CongDocument *doc,
					      CongNodePtr node)
{
	cong_node_modification_construct (CONG_NODE_MODIFICATION(node_modification_make_orphan),
					  doc,
					  node);

	if (node->parent) {
		PRIVATE(node_modification_make_orphan)->former_parent = node->parent;
		cong_document_node_ref (doc, PRIVATE(node_modification_make_orphan)->former_parent);		
	}
	if (node->next) {
		PRIVATE(node_modification_make_orphan)->former_older_sibling = node->next;
		cong_document_node_ref (doc, PRIVATE(node_modification_make_orphan)->former_older_sibling);		
	}

	return node_modification_make_orphan;
}

CongModification*
cong_node_modification_make_orphan_new (CongDocument *doc,
					CongNodePtr node)
{
	return CONG_MODIFICATION(cong_node_modification_make_orphan_construct (CONG_NODE_MODIFICATION_MAKE_ORPHAN(g_object_new (CONG_NODE_MODIFICATION_MAKE_ORPHAN_TYPE, NULL)),
									       doc,
									       node));
}

static void
finalize (GObject *object)
{
	CongNodeModificationMakeOrphan *node_modification_make_orphan = CONG_NODE_MODIFICATION_MAKE_ORPHAN (object);

#if DEBUG_MODIFICATION_LIFETIMES
	g_message ("CongNodeModificationMakeOrphan::finalize");
#endif

	g_free (node_modification_make_orphan->private);
	node_modification_make_orphan->private = NULL;
	
	G_OBJECT_CLASS (parent_class)->finalize (object);

}

static void
dispose (GObject *object)
{
	CongNodeModificationMakeOrphan *node_modification_make_orphan = CONG_NODE_MODIFICATION_MAKE_ORPHAN (object);
	CongDocument *doc = cong_modification_get_document(CONG_MODIFICATION(node_modification_make_orphan));

#if DEBUG_MODIFICATION_LIFETIMES
	g_message ("CongNodeModificationMakeOrphan::dispose");
#endif

	g_assert (node_modification_make_orphan->private);
	
	/* Cleanup: */
	if (PRIVATE(node_modification_make_orphan)->former_parent) {
		cong_document_node_unref (doc, 
					  PRIVATE(node_modification_make_orphan)->former_parent);		
		PRIVATE(node_modification_make_orphan)->former_parent = NULL;
	}
	if (PRIVATE(node_modification_make_orphan)->former_older_sibling) {
		cong_document_node_unref (doc, 
					PRIVATE(node_modification_make_orphan)->former_older_sibling);		
		PRIVATE(node_modification_make_orphan)->former_older_sibling = NULL;
	}

	/* Call the parent method: */		
	GNOME_CALL_PARENT (G_OBJECT_CLASS, dispose, (object));
}

static void
undo (CongModification *modification)
{
	CongNodeModificationMakeOrphan *node_modification_make_orphan = CONG_NODE_MODIFICATION_MAKE_ORPHAN (modification);
	CongDocument *doc = cong_modification_get_document (modification);
	CongNodePtr node = cong_node_modification_get_node (CONG_NODE_MODIFICATION(modification));

	cong_document_begin_edit (doc);

	if (PRIVATE(node_modification_make_orphan)->former_parent) {
		if (PRIVATE(node_modification_make_orphan)->former_older_sibling) {
			cong_document_node_add_after (doc, 
						       node, 
						       PRIVATE(node_modification_make_orphan)->former_older_sibling);
		} else {
			cong_document_node_set_parent (doc, 
						       node, 
						       PRIVATE(node_modification_make_orphan)->former_parent);
		}
	} else {
		cong_document_node_make_orphan (doc, 
						node);
	}

	cong_document_end_edit (doc);
}

static void
redo (CongModification *modification)
{
	CongNodeModificationMakeOrphan *node_modification_make_orphan = CONG_NODE_MODIFICATION_MAKE_ORPHAN (modification);
	CongDocument *doc = cong_modification_get_document (modification);
	CongNodePtr node = cong_node_modification_get_node (CONG_NODE_MODIFICATION(modification));

	cong_document_begin_edit (doc);

	cong_document_node_make_orphan (doc, 
					node);

	cong_document_end_edit (doc);
}
