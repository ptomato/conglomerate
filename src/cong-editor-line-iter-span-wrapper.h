/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * cong-editor-line-iter-span-wrapper.h
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

#ifndef __CONG_EDITOR_LINE_ITER_SPAN_WRAPPER_H__
#define __CONG_EDITOR_LINE_ITER_SPAN_WRAPPER_H__

#include "cong-editor-line-iter.h"
#include "cong-editor-line-manager-span-wrapper.h"

G_BEGIN_DECLS

typedef struct CongEditorLineIterSpanWrapper CongEditorLineIterSpanWrapper;

#define CONG_EDITOR_LINE_ITER_SPAN_WRAPPER_TYPE	  (cong_editor_line_iter_span_wrapper_get_type ())
#define CONG_EDITOR_LINE_ITER_SPAN_WRAPPER(obj)         G_TYPE_CHECK_INSTANCE_CAST (obj, CONG_EDITOR_LINE_ITER_SPAN_WRAPPER_TYPE, CongEditorLineIterSpanWrapper)
#define CONG_EDITOR_LINE_ITER_SPAN_WRAPPER_CLASS(klass) G_TYPE_CHECK_CLASS_CAST (klass, CONG_EDITOR_LINE_ITER_SPAN_WRAPPER_TYPE, CongEditorLineIterSpanWrapperClass)
#define IS_CONG_EDITOR_LINE_ITER_SPAN_WRAPPER(obj)      G_TYPE_CHECK_INSTANCE_TYPE (obj, CONG_EDITOR_LINE_ITER_SPAN_WRAPPER_TYPE)

CONG_DECLARE_CLASS_PUBLIC_DATA (CongEditorLineIterSpanWrapper, cong_editor_line_iter_span_wrapper, CongEditorLineIter,
				CongEditorLineIter *outer_iter;
				CongEditorArea *prev_area_on_current_line;
				, )

CongEditorLineIterSpanWrapper*
cong_editor_line_iter_span_wrapper_new (CongEditorLineManagerSpanWrapper *span_wrapper,
					CongEditorLineIter *outer_iter);

G_END_DECLS

#endif



