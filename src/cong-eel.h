/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/** 
 *  cong-eel.c
 *
 *  Contains code copied and pasted from eel-2.0, to avoid dependency issues.
 *  Everything gets prefixed with a "cong_"
 */

#ifndef __CONG_EEL_H__
#define __CONG_EEL_H__

G_BEGIN_DECLS

/* From eel-gdk-extensions.h: */
gboolean            cong_eel_rectangle_contains             (const GdkRectangle  *rectangle,
							    int                  x,
							    int                  y);

guint32             cong_eel_rgb16_to_rgb                   (gushort              r,
							    gushort              g,
							    gushort              b);
guint32             cong_eel_gdk_color_to_rgb               (const GdkColor      *color);
char *              cong_eel_gdk_rgb_to_color_spec          (guint32              color);



G_END_DECLS

#endif