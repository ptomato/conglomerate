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

#include "cong-fake-plugin-hooks.h"

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
	g_message("dtd_to_xds_importer_action_callback");

	CONG_DO_UNIMPLEMENTED_DIALOG(toplevel_window, "Importing DTD as XDS");
}

void dtd_to_rng_importer_action_callback(CongImporter *importer, const gchar *uri, const gchar *mime_type, gpointer user_data, GtkWindow *toplevel_window)
{
	g_message("dtd_to_rng_importer_action_callback");

	CONG_DO_UNIMPLEMENTED_DIALOG(toplevel_window, "Importing DTD as RELAX NG Schema");
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
