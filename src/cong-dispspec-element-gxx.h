/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* 
   cong-dispspec-element-gxx.h
   
   This file will be included multiple times, with different results.
*/

GXX_STRUCT_BEGIN_ELEMENT_WITH_CONSTRUCTOR("element", CongDispspecElement, dispspec_element)
     GXX_STRUCT_ATTRIBUTE_ENUM("type", type, TRUE, CONG_ELEMENT_TYPE_UNKNOWN, type_numeration)
     GXX_STRUCT_ATTRIBUTE_STRING("ns", xmlns, FALSE, "")
     GXX_STRUCT_ATTRIBUTE_STRING("tag", tagname, TRUE, "")
     GXX_STRUCT_ATTRIBUTE_ENUM("whitespace", whitespace, FALSE, CONG_WHITESPACE_NORMALIZE, whitespace_numeration)
GXX_STRUCT_END_ELEMENT()

