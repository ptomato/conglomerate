/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-object.h
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

#ifndef __CONG_OBJECT_H__
#define __CONG_OBJECT_H__

G_BEGIN_DECLS

/* Macros for declaring GObject subclasses */
#define PRIVATE(x) ((x)->priv)

/* Debugging system, to try to make it easier to spot reference leaks: */
void 
cong_object_debug_instance_init (GObject *object);

void
cong_object_debug_finalize (GObject *object);

guint32
cong_object_debug_get_instance_count (void);

guint32
cong_object_debug_get_instance_count_for_class (GObjectClass *klass);

#if 1
#define CONG_OBJECT_DEBUG_INSTANCE_INIT(object) cong_object_debug_instance_init (object)
#define CONG_OBJECT_DEBUG_FINALIZE(object) cong_object_debug_finalize (object)
#else
#define CONG_OBJECT_DEBUG_INSTANCE_INIT(object) ((void)0)
#define CONG_OBJECT_DEBUG_FINALIZE(object) ((void)0)
#endif

/* Macros for declaring and defining GObject subclasses in various ways: */
#define CONG_DECLARE_CLASS_BEGIN(MyClass, my_class, BaseClass) \
extern GType my_class##_get_type (void); \
typedef struct MyClass##Class MyClass##Class; \
typedef struct MyClass##Private MyClass##Private; \
struct MyClass \
{ \
	BaseClass base_obj; \
	MyClass##Private *priv; \
}; \
struct MyClass##Class \
{ \
	BaseClass##Class base_class; \

#define CONG_DECLARE_CLASS_END() \
};

#define CONG_DECLARE_CLASS_PUBLIC_DATA(MyClass, my_class, BaseClass, PublicDataDecls, ClassFunctionDecls) \
extern GType my_class##_get_type (void); \
typedef struct MyClass##Class MyClass##Class; \
typedef struct MyClass##Private MyClass##Private; \
struct MyClass \
{ \
	BaseClass base_obj; \
        PublicDataDecls \
}; \
struct MyClass##Class \
{ \
	BaseClass##Class base_class; \
        ClassFunctionDecls \
};

#define CONG_DECLARE_CLASS(MyClass, my_class, BaseClass) \
CONG_DECLARE_CLASS_BEGIN(MyClass, my_class, BaseClass) \
CONG_DECLARE_CLASS_END()

/* Macros for defining GObject subclasses: */
#define CONG_DEFINE_CLASS_BEGIN(MyClass, my_class, MY_CLASS, BaseClass, BASE_CLASS_TYPE) \
static void my_class##_finalize (GObject *object); \
static void my_class##_dispose (GObject *object); \
\
GNOME_CLASS_BOILERPLATE(MyClass, my_class, BaseClass, BASE_CLASS_TYPE) \
\
static void \
my_class##_instance_init (MyClass *obj) \
{ \
obj->priv = g_new0 (MyClass##Private, 1); \
CONG_OBJECT_DEBUG_INSTANCE_INIT( G_OBJECT(obj)); \
} \
static void \
my_class##_finalize (GObject *object) \
{ \
	MyClass *my_obj = MY_CLASS(object); \
        CONG_OBJECT_DEBUG_FINALIZE (object); \
	g_assert (my_obj->priv); \
	g_free (my_obj->priv); \
	my_obj->priv = NULL; \
	G_OBJECT_CLASS (parent_class)->finalize (object); \
} \
static void \
my_class##_class_init (MyClass##Class *klass) \
{ \
	G_OBJECT_CLASS (klass)->finalize = my_class##_finalize; \
	G_OBJECT_CLASS (klass)->dispose = my_class##_dispose; \

#define CONG_DEFINE_CLASS_END() \
};

#define CONG_DEFINE_CLASS_PUBLIC_DATA(MyClass, my_class, MY_CLASS, BaseClass, BASE_CLASS_TYPE, ClassInitCodeFragment) \
static void my_class##_finalize (GObject *object); \
static void my_class##_dispose (GObject *object); \
\
GNOME_CLASS_BOILERPLATE(MyClass, my_class, BaseClass, BASE_CLASS_TYPE) \
\
static void \
my_class##_instance_init (MyClass *obj) \
{ \
CONG_OBJECT_DEBUG_INSTANCE_INIT (G_OBJECT (obj)); \
} \
static void \
my_class##_finalize (GObject *object) \
{ \
	/*MyClass *my_obj = MY_CLASS(object);*/ \
        CONG_OBJECT_DEBUG_FINALIZE (object); \
	G_OBJECT_CLASS (parent_class)->finalize (object); \
} \
static void \
my_class##_class_init (MyClass##Class *klass) \
{ \
	G_OBJECT_CLASS (klass)->finalize = my_class##_finalize; \
	G_OBJECT_CLASS (klass)->dispose = my_class##_dispose; \
        ClassInitCodeFragment \
};

#define CONG_DEFINE_EMPTY_DISPOSE(my_class) \
static void my_class##_dispose (GObject *object) {}

#define CONG_DEFINE_CLASS(MyClass, my_class, MY_CLASS, BaseClass, BASE_CLASS_TYPE) \
CONG_DEFINE_CLASS_BEGIN(MyClass, my_class, MY_CLASS, BaseClass, BASE_CLASS_TYPE) \
CONG_DEFINE_CLASS_END() \
CONG_DEFINE_EMPTY_DISPOSE(my_class)

G_END_DECLS

#endif
