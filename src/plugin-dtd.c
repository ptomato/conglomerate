/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * plugin-dtd.c
 *
 * Plugin for manipulating DTD files
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
#include "cong-plugin.h"
#include "cong-error-dialog.h"
#include "cong-dispspec.h"

#include "cong-fake-plugin-hooks.h"

#include "cong-util.h"


/* Internal function declarations: */
static xmlDtdPtr 
load_dtd (const gchar *uri, 
	  GtkWindow *toplevel_window);

static xmlDocPtr
make_rng_from_dtd (xmlDtdPtr dtd);

static void
element_callback_generate_rng_from_dtd (xmlElementPtr dtd_element,
					gpointer user_data);

static void
attribute_callback_generate_rng_from_dtd (xmlElementPtr dtd_element,
					  xmlAttributePtr attr,
					  gpointer user_data);

static void
add_content_subtree_to_rng (CongNodePtr node_parent,
			    xmlElementContentPtr content);

/* Plugin hooks: */
gboolean dtd_importer_mime_filter(CongImporter *importer, const gchar *mime_type, gpointer user_data)
{
	g_return_val_if_fail(importer, FALSE);
	g_return_val_if_fail(mime_type, FALSE);

	if (0==strcmp(mime_type,"text/x-dtd")) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void dtd_to_xds_importer_action_callback(CongImporter *importer, const gchar *uri, const gchar *mime_type, gpointer user_data, GtkWindow *toplevel_window)
{
	xmlDtdPtr dtd;

	g_message("dtd_to_xds_importer_action_callback");

	dtd = load_dtd(uri, toplevel_window);

	if (dtd) {
		gchar *name = g_strdup_printf(_("Autogenerated document type based on %s"), uri);
		gchar *description = g_strdup_printf(_("This Conglomerate display specification was automatically generated from %s"), uri);

		CongDispspec *dispspec = cong_dispspec_new_generate_from_dtd(dtd, name, description);
		xmlDocPtr xml_doc = cong_dispspec_make_xml(dispspec);

		g_free(name);
		g_free(description);

		/* Free up the temporary dispspec: */
		cong_dispspec_delete(dispspec);

		/* Free up the DTD: */
		xmlFreeDtd(dtd);

		/* Do appropriate UI stuff: */
		cong_ui_new_document_from_imported_xml(xml_doc,
						       toplevel_window);
	}
}


void dtd_to_rng_importer_action_callback(CongImporter *importer, const gchar *uri, const gchar *mime_type, gpointer user_data, GtkWindow *toplevel_window)
{
	xmlDtdPtr dtd;

	g_message("dtd_to_rng_importer_action_callback");

#if 1
	dtd = load_dtd(uri, toplevel_window);

	if (dtd) {
		xmlDocPtr xml_doc = make_rng_from_dtd(dtd);

		/* Free up the DTD: */
		xmlFreeDtd(dtd);

		/* Do appropriate UI stuff: */
		cong_ui_new_document_from_imported_xml(xml_doc,
						       toplevel_window);
	}
#else
	CONG_DO_UNIMPLEMENTED_DIALOG_WITH_BUGZILLA_ID(toplevel_window, "Importing DTD as RELAX NG Schema", 118768);
#endif

}

void dtd_to_w3c_xml_schema_importer_action_callback(CongImporter *importer, const gchar *uri, const gchar *mime_type, gpointer user_data, GtkWindow *toplevel_window)
{
	g_message("dtd_to_w3c_xml_schema_importer_action_callback");

	CONG_DO_UNIMPLEMENTED_DIALOG(toplevel_window, "Importing DTD as W3C XML Schema");
}

void dtd_to_schematron_importer_action_callback(CongImporter *importer, const gchar *uri, const gchar *mime_type, gpointer user_data, GtkWindow *toplevel_window)
{
	g_message("dtd_to_schematron_importer_action_callback");

	CONG_DO_UNIMPLEMENTED_DIALOG(toplevel_window, "Importing DTD as Schematron Schema");
}

void dtd_to_examplotron_importer_action_callback(CongImporter *importer, const gchar *uri, const gchar *mime_type, gpointer user_data, GtkWindow *toplevel_window)
{
	g_message("dtd_to_examplotron_importer_action_callback");

	CONG_DO_UNIMPLEMENTED_DIALOG(toplevel_window, "Importing DTD as Examplotron Schema");
}



 /* would be exposed as "plugin_register"? */
gboolean plugin_dtd_plugin_register(CongPlugin *plugin)
{
	g_return_val_if_fail(plugin, FALSE);

	cong_plugin_register_importer(plugin, 
				      _("Convert DTD into a Conglomerate Display Specification"), 
				      _("Import a DTD file, creating a Conglomerate display specification file."),
				      "dtd-to-xds-import",
				      dtd_importer_mime_filter,
				      dtd_to_xds_importer_action_callback,
				      NULL);

	cong_plugin_register_importer(plugin, 
				      _("Convert DTD into a Relax NG schema"), 
				      _("Import a DTD file, converting it into a RELAX NG Schema."),
				      "dtd-to-rng-import",
				      dtd_importer_mime_filter,
				      dtd_to_rng_importer_action_callback,
				      NULL);

	cong_plugin_register_importer(plugin, 
				      _("Convert DTD into W3C XML Schema"), 
				      _("Import a DTD file, converting it into a W3C XML Schema."),
				      "dtd-to-w3c-xml-schema-import",
				      dtd_importer_mime_filter,
				      dtd_to_w3c_xml_schema_importer_action_callback,
				      NULL);
	
	cong_plugin_register_importer(plugin, 
				      _("Convert DTD into a Schematron file"), 
				      _("Import a DTD file, converting it into a Schematron Schema."),
				      "dtd-to-schematron-import",
				      dtd_importer_mime_filter,
				      dtd_to_schematron_importer_action_callback,
				      NULL);
	
	cong_plugin_register_importer(plugin, 
				      _("Convert DTD into an Examplotron file"), 
				      _("Import a DTD file, converting it into an Examplotron Schema."),
				      "dtd-to-examplotron-import",
				      dtd_importer_mime_filter,
				      dtd_to_examplotron_importer_action_callback,
				      NULL);
	
	return TRUE;
}

/* exposed as "plugin_configure"? legitimate for it not to be present */
gboolean plugin_dtd_plugin_configure(CongPlugin *plugin)
{
	g_return_val_if_fail(plugin, FALSE);

	return TRUE;
}

/* Internal function definitions: */
static xmlDtdPtr 
load_dtd (const gchar *uri, 
	  GtkWindow *toplevel_window)
{
	xmlDtdPtr dtd;
	GnomeVFSURI *vfs_uri;
	gchar *local_path;

	g_return_val_if_fail(uri, NULL);

	vfs_uri = gnome_vfs_uri_new(uri);
	local_path = cong_util_get_local_path_from_uri(vfs_uri);
	gnome_vfs_uri_unref(vfs_uri);

	dtd = xmlIOParseDTD(NULL, 
			    xmlParserInputBufferCreateFilename	(local_path,
								 XML_CHAR_ENCODING_NONE),
			    XML_CHAR_ENCODING_NONE);

	return dtd;
}

static xmlDocPtr
make_rng_from_dtd (xmlDtdPtr dtd)
{
	xmlDocPtr xml_doc;
	CongNodePtr root_node;

	g_return_val_if_fail (dtd, NULL);


	/* Build up the document and its content: */
	xml_doc = xmlNewDoc("1.0");
	
	root_node = xmlNewDocNode(xml_doc,
				  NULL, /* xmlNsPtr ns, */
				  "grammar",
				  NULL);
	
	xmlDocSetRootElement(xml_doc,
			     root_node);

	/* FIXME: The start tag? */
	
	/* Add <define> tags for all the elements and attributes: */
	cong_dtd_for_each_element (dtd,
				   element_callback_generate_rng_from_dtd,
				   root_node);
	return xml_doc;	

}

static void
element_callback_generate_rng_from_dtd (xmlElementPtr dtd_element,
					gpointer user_data)
{
	CongNodePtr root_node = (CongNodePtr)user_data;
	CongNodePtr node_define;
	
	node_define = xmlNewDocNode(root_node->doc,
				    NULL,
				    "define",
				    NULL);			
	xmlAddChild (root_node, 
		     node_define);

	xmlSetProp (node_define,
		    "name",
		    dtd_element->name);

	/* Create the <element> tag: */
	{
		CongNodePtr node_element = xmlNewDocNode(root_node->doc,
							 NULL,
							 "element",
							 NULL);
		xmlAddChild (node_define, 
			     node_element);

		xmlSetProp (node_element,
			    "name",
			    dtd_element->name);

		/* set up the content model */
		{
			cong_dtd_for_each_attribute (dtd_element,
						     attribute_callback_generate_rng_from_dtd,
						     node_element);
			
			if (dtd_element->content) {
				add_content_subtree_to_rng (node_element,
							    dtd_element->content);
			}
		}
	}	
}

static void
attribute_callback_generate_rng_from_dtd (xmlElementPtr dtd_element,
					  xmlAttributePtr attr,
					  gpointer user_data)
{
	CongNodePtr node_element = (CongNodePtr)user_data;
	CongNodePtr node_occurrence = NULL;
	CongNodePtr node_attribute;

	switch (attr->def) {
	default: g_assert_not_reached ();
	case XML_ATTRIBUTE_NONE:
	case XML_ATTRIBUTE_IMPLIED:
		/* Attribute is optional: */
		node_occurrence = xmlNewDocNode(node_element->doc,
						NULL,
						"optional",
						NULL);
		
		xmlAddChild (node_element,
			     node_occurrence);
		break;
	case XML_ATTRIBUTE_REQUIRED:
	case XML_ATTRIBUTE_FIXED:
		/* Attribute is required; no need to wrap with an occurrence tag */
		node_occurrence = node_element;
		break;
	}

	g_assert (node_occurrence);

	node_attribute= xmlNewDocNode(node_element->doc,
				      NULL,
				      "attribute",
				      NULL);			
	xmlAddChild (node_occurrence,
		     node_attribute);
	
	xmlSetProp (node_attribute,
		    "name",
		    attr->name);

	/* FIXME: do we need to handle the default? */

	/* Define content model: */
	switch (attr->atype) {
	default: g_assert_not_reached ();
	case XML_ATTRIBUTE_CDATA:
		/* no need to add anything? */
		break;
	case XML_ATTRIBUTE_ID:
		/* FIXME */
		break;

	case XML_ATTRIBUTE_IDREF:
		/* FIXME */
		break;

	case XML_ATTRIBUTE_IDREFS:
		/* FIXME */
		break;

	case XML_ATTRIBUTE_ENTITY:
		/* FIXME */
		break;

	case XML_ATTRIBUTE_ENTITIES:
		/* FIXME */
		break;

	case XML_ATTRIBUTE_NMTOKEN:
		/* FIXME */
		break;

	case XML_ATTRIBUTE_NMTOKENS:
		/* FIXME */
		break;

	case XML_ATTRIBUTE_ENUMERATION:
		/* Add a <choice> tag: */
		{
			CongNodePtr node_choice = xmlNewDocNode(node_element->doc,
								NULL,
								"choice",
								NULL);
			xmlAddChild (node_attribute, 
				     node_choice);

			/* Add the enum values: */
			{
				xmlEnumerationPtr iter;	

				for (iter = attr->tree; iter; iter=iter->next) {
					CongNodePtr node_value = xmlNewDocNode(node_element->doc,
									       NULL,
									       "value",
									       iter->name);
					xmlAddChild (node_choice,
						     node_value);
					
				}
			}	
		}
		break;

	case XML_ATTRIBUTE_NOTATION:	
		/* FIXME */
		break;
	}
}

static void
add_content_subtree_to_rng (CongNodePtr node_parent,
			    xmlElementContentPtr content)
{
	CongNodePtr node_occurrence = NULL;

	switch (content->ocur) {
	default: g_assert_not_reached ();
	case XML_ELEMENT_CONTENT_ONCE:
		/* then we don't need to wrap the content with an occurrence tag: */
		node_occurrence = node_parent;
		break;

	case XML_ELEMENT_CONTENT_OPT:
		node_occurrence = xmlNewDocNode(node_parent->doc,
						NULL,
						"optional",
						NULL);
		xmlAddChild (node_parent, 
			     node_occurrence);
		break;

	case XML_ELEMENT_CONTENT_MULT:
		node_occurrence = xmlNewDocNode(node_parent->doc,
						NULL,
						"zeroOrMore",
						NULL);
		xmlAddChild (node_parent, 
			     node_occurrence);
		break;
		
	case XML_ELEMENT_CONTENT_PLUS:
		node_occurrence = xmlNewDocNode(node_parent->doc,
						NULL,
						"oneOrMore",
						NULL);
		xmlAddChild (node_parent, 
			     node_occurrence);
		break;
	}
		
	g_assert (node_occurrence);

	switch (content->type) {
	default: g_assert_not_reached ();
	case XML_ELEMENT_CONTENT_PCDATA:
		/* Add an empty <text> tag: */
		{
			CongNodePtr node_text = xmlNewDocNode(node_occurrence->doc,
							      NULL,
							      "text",
							      NULL);
			xmlAddChild (node_occurrence, 
				     node_text);
		}
		break;
	case XML_ELEMENT_CONTENT_ELEMENT:
		/* Add a <ref name="foobar"> tag: */
		{
			CongNodePtr node_ref = xmlNewDocNode(node_occurrence->doc,
							     NULL,
							     "ref",
							     NULL);
			xmlAddChild (node_occurrence, 
				     node_ref);

			xmlSetProp (node_ref,
				    "name",
				    content->name);
		}		
		break;
	case XML_ELEMENT_CONTENT_SEQ:
		/* Add a <group> tag, and recurse; optimise away the cae where the parent is a <group> tag: */
		{
			if (cong_node_is_tag (node_occurrence, NULL, "group")) {
				add_content_subtree_to_rng (node_occurrence,
							    content->c1);
				add_content_subtree_to_rng (node_occurrence,
							    content->c2);
			} else {
				CongNodePtr node_group = xmlNewDocNode(node_occurrence->doc,
								       NULL,
								       "group",
								       NULL);
				xmlAddChild (node_occurrence, 
					     node_group);
				
				add_content_subtree_to_rng (node_group,
							    content->c1);
				add_content_subtree_to_rng (node_group,
							    content->c2);
			}
		}
		break;
	case XML_ELEMENT_CONTENT_OR:
		/* The naive implementation is to add a <choice> tag, and recurse.
		   But if we have something like ( tag-a | tag-b | tag-c | ... | tag-z) in the DTD, we get a tree of ( choice tag-a (choice tag-b (choice tag-c ( ... choice tag-y tag-z))))
		   which creates a grim-looking RNG tree.

		   So we spot simple <choice> in the tag above, and merge into it if possible; <choice> is associative and hence bracketing should make difference...
		 */
		{
			if (cong_node_is_tag (node_occurrence, NULL, "choice")) {
				/* Optimised case: */
				add_content_subtree_to_rng (node_occurrence,
							    content->c1);
				add_content_subtree_to_rng (node_occurrence,
							    content->c2);
			} else {
				/* Non-optimised case: */
				CongNodePtr node_choice = xmlNewDocNode(node_occurrence->doc,
									NULL,
									"choice",
									NULL);
				xmlAddChild (node_occurrence, 
					     node_choice);
				
				add_content_subtree_to_rng (node_choice,
							    content->c1);
				add_content_subtree_to_rng (node_choice,
							    content->c2);
			}
		}
		break;
	}
}

