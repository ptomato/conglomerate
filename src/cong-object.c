/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-object.c
 *
 * Copyright (C) 2004 David Malcolm
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
#include "cong-object.h"

/* Internal type aliases: */
typedef struct CongObjectDebugData CongObjectDebugData;
typedef struct CongObjectClassDebugData CongObjectClassDebugData;

/* Internal type declarations: */
struct CongObjectDebugData
{
	guint32 instance_count;
	GHashTable *hash_of_classes;
};

struct CongObjectClassDebugData
{
	guint32 instance_count;
};

/* Internal function declarations: */
static CongObjectDebugData*
cong_object_debug_get_global_data (void);

static CongObjectClassDebugData*
cong_object_debug_get_class_data (GType type);

static void
cong_object_debug_log_event (GObject *object,
			     GType type,
			     const gchar *msg);

/* Exported function definitions: */
void 
cong_object_debug_instance_init (GObject *object,
				 GType type)
{
	CongObjectDebugData *debug_data;
	CongObjectClassDebugData *class_debug_data;
	
	g_assert (object);

	debug_data = cong_object_debug_get_global_data ();
	class_debug_data = cong_object_debug_get_class_data (type);
	g_assert (debug_data);
	g_assert (class_debug_data);

	debug_data->instance_count++;
	class_debug_data->instance_count++;

	cong_object_debug_log_event (object,
				     type,
				     "instance_init");
}

void
cong_object_debug_finalize (GObject *object,
			    GType type)
{
	CongObjectDebugData *debug_data;
	CongObjectClassDebugData *class_debug_data;
	
	g_assert (object);

	debug_data = cong_object_debug_get_global_data ();
	class_debug_data = cong_object_debug_get_class_data (type);
	g_assert (debug_data);
	g_assert (class_debug_data);

	g_assert (debug_data->instance_count>0);
	debug_data->instance_count--;

	g_assert (class_debug_data->instance_count>0);
	class_debug_data->instance_count--;

	cong_object_debug_log_event (object,
				     type,
				     "finalize");
}

guint32
cong_object_debug_get_instance_count (void)
{
	CongObjectDebugData* debug_data;
	
	debug_data = cong_object_debug_get_global_data ();
	g_assert (debug_data);

	return debug_data->instance_count;
}

guint32
cong_object_debug_get_instance_count_for_class (GObjectClass *klass)
{
	return cong_object_debug_get_instance_count_for_type (G_OBJECT_CLASS_TYPE (klass));
}

guint32
cong_object_debug_get_instance_count_for_type (GType type)
{
	CongObjectClassDebugData *class_data = cong_object_debug_get_class_data (type);
	g_assert (class_data);
	
	return class_data->instance_count;
}

/*Internal function definitions: */
static CongObjectDebugData*
cong_object_debug_get_global_data (void)
{
	static CongObjectDebugData *global_debug_data;

	if (NULL==global_debug_data) {
		global_debug_data = g_new0 (CongObjectDebugData, 1);
		global_debug_data->hash_of_classes = g_hash_table_new (g_direct_hash,
								       g_direct_equal);
	}

	g_assert (global_debug_data);
	return global_debug_data;
}


static CongObjectClassDebugData*
cong_object_debug_get_class_data (GType type)
{
	CongObjectDebugData *debug_data;
	CongObjectClassDebugData *class_debug_data;

	debug_data = cong_object_debug_get_global_data ();
	class_debug_data = g_hash_table_lookup (debug_data->hash_of_classes,
						GINT_TO_POINTER (type));

	if (NULL==class_debug_data) {
		class_debug_data = g_new0 (CongObjectClassDebugData, 1);
		
		g_hash_table_insert (debug_data->hash_of_classes,
				     GINT_TO_POINTER (type),
				     class_debug_data);		
	}

	return class_debug_data;
}

static void
cong_object_debug_log_event (GObject *object,
			     GType type,
			     const gchar *msg)
{
	g_assert (object);
	g_assert (msg);

	g_message ("%s: now %i objects (%i of %s)", 
		   msg,
		   cong_object_debug_get_instance_count (),
		   cong_object_debug_get_instance_count_for_type (type),
		   g_type_name (type));	
}

