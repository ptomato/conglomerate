#include <stdlib.h>

#define GTK_ENABLE_BROKEN

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <ttree.h>
#include <xml.h>
#include "global.h"

GtkStyle *style_white;

void xv_style_r(GtkWidget *widget, gpointer data);


static gint xv_section_head_expose(GtkWidget *w, GdkEventExpose *event, TTREE *x)
{
	GdkGC *gc;
	int str_width;
	TTREE *n0;

	str_width = gdk_string_width(ft, xml_frag_name_nice(x));
	str_width = str_width > 300 ? str_width : 300;

	
	str_width = w->allocation.width - 4;
	
	gc = ds_gc_get(ds_global, x, 0);
	if (!gc) gc = w->style->black_gc;
	
	/* Fill the header box */

	gdk_draw_rectangle(w->window, gc, 1, 1, 1, str_width + 2, w->allocation.height - 4);
	
	/* Frame the coloured rectangle */

	gdk_draw_line(w->window, w->style->black_gc, 0, 0, str_width + 3, 0);
	gdk_draw_line(w->window, w->style->black_gc, 3, w->allocation.height - 1 - 4,
								str_width + 3, w->allocation.height - 1 - 4);
	gdk_draw_line(w->window, w->style->black_gc, str_width + 3, 0,
								str_width + 3, w->allocation.height - 1 - 4);
	gdk_draw_line(w->window, w->style->black_gc, 0, 0,
								0, w->allocation.height - 1 - 4);

	/* White out at right */

	gdk_draw_rectangle(w->window, w->style->white_gc, 1, str_width + 4, 0,
										 w->allocation.width - (str_width + 4),
										 w->allocation.height - 4);

	/* White out below */

	gdk_draw_rectangle(w->window, w->style->white_gc, 1,
										 4, w->allocation.height - 4,
										 w->allocation.width, w->allocation.height);

	/* Lines on left */
	
	/* Coloured line in the middle */

	gdk_draw_line(w->window, gc, 1, w->allocation.height - 4, 1, w->allocation.height);
	gdk_draw_line(w->window, gc, 2, w->allocation.height - 4, 2, w->allocation.height);

	/* Black vertical lines right and left */

	gdk_draw_line(w->window, w->style->black_gc, 0, w->allocation.height - 4, 0,
								w->allocation.height);

	gdk_draw_line(w->window, w->style->black_gc, 3, w->allocation.height - 4, 3,
								w->allocation.height);

	/* Section identifier */

	gdk_draw_string(w->window, ft, w->style->black_gc, 4, 2 + ft_asc,
									ds_name_get(x));

	/* Metadata indicator */
	
	n0 = x->child;
	n0 = ttree_branch_walk_str(n0, "tag_span metadata");
	if (n0)
	{
		gdk_draw_string(w->window, ft, w->style->black_gc, 
										w->allocation.width - 4 - gdk_string_width(ft, "meta"),
										2 + ft_asc, "meta");
	}

	/* Medios indicator */
	
	n0 = x->child;
	n0 = ttree_branch_walk_str(n0, "tag_span metadata tag_span metadata.sourceset");
	if (n0)
	{
		gdk_draw_string(w->window, ft, w->style->black_gc, 
										w->allocation.width - 4 - gdk_string_width(ft, "meta") -
										10 - gdk_string_width(ft, "fuentes"),
										2 + ft_asc, "fuentes");
	}
}

GtkWidget *xv_section_head(TTREE *ds, TTREE *x)
{
	TTREE *n0;
	GtkWidget *vbox, *title;

	vbox = gtk_vbox_new(FALSE, 0);
	title = gtk_drawing_area_new();
	gtk_box_pack_start(GTK_BOX(vbox), title, TRUE, TRUE, 0);
	gtk_drawing_area_size(GTK_DRAWING_AREA(title), 300, ft_asc + ft_desc + 4 /* up and down borders */ + 4 /* space below */);
	gtk_signal_connect(GTK_OBJECT(title), "expose_event",
										 (GtkSignalFunc) xv_section_head_expose, x);
	gtk_signal_connect(GTK_OBJECT(title), "configure_event",
										 (GtkSignalFunc) xv_section_head_expose, x);
  gtk_widget_set_events(title, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

	gtk_widget_show(title);
	gtk_widget_show(vbox);
	return(vbox);
}


static gint xv_fragment_head_expose(GtkWidget *w, GdkEventExpose *event, TTREE *x)
{
	GdkGC *gc;
	int str_width;
	int i;
	
	str_width = gdk_string_width(ft, xml_frag_name_nice(x));
	str_width = str_width > 150 ? str_width : 150;

	gc = ds_gc_get(ds_global, xml_frag_exit(x), 0);
	if (!gc) gc = w->style->black_gc;

	/* Fill the header box */

	/* ========== */
	
	gdk_draw_rectangle(w->window, gc, 1, 1, 0, str_width + 2, w->allocation.height - 4);

	/*           / */

	for (i = 1; i < w->allocation.height; i++)
	{
		gdk_draw_line(w->window, gc, str_width + 3, i,
									str_width + (w->allocation.height - i + 1 - 4), i);
	}


	/* Frame the coloured <thing> */

	gdk_draw_line(w->window, w->style->black_gc, 3, 0, str_width + 3 + w->allocation.height - 4, 0);

	gdk_draw_line(w->window, w->style->black_gc, 3, w->allocation.height - 1 - 4,
								str_width + 3, w->allocation.height - 1 - 4);

	gdk_draw_line(w->window, w->style->black_gc,
								str_width + 2 + w->allocation.height - 4, 0,
								str_width + 3, w->allocation.height - 1 - 4);

	gdk_draw_line(w->window, w->style->black_gc, 0, 0,
								0, w->allocation.height - 1 - 4);

	/* White out at right */

	gdk_draw_rectangle(w->window, w->style->white_gc, 1,
										 str_width + 3 + w->allocation.height - 4, 0,
										 w->allocation.width - (str_width + 4 + w->allocation.height - 4),
										 w->allocation.height - 4);

	/* White out below */

	gdk_draw_rectangle(w->window, w->style->white_gc, 1,
										 4, w->allocation.height - 4,
										 w->allocation.width, w->allocation.height);

	/* Lines on left */
	
	/* Coloured line in the middle */

	gdk_draw_line(w->window, gc, 1, w->allocation.height - 4, 1, w->allocation.height);
	gdk_draw_line(w->window, gc, 2, w->allocation.height - 4, 2, w->allocation.height);

	/* Black vertical lines right and left */

	gdk_draw_line(w->window, w->style->black_gc, 0, w->allocation.height - 4, 0,
								w->allocation.height);

	gdk_draw_line(w->window, w->style->black_gc, 3, w->allocation.height - 4, 3,
								w->allocation.height);

	/* Section identifier */

	gdk_draw_string(w->window, ft, w->style->black_gc, 4, 2 + ft_asc,
									ds_name_get(x));
	
	return(TRUE);
}


GtkWidget *xv_fragment_head(TTREE *ds, TTREE *x)
{
	TTREE *n0;
	GtkWidget *title;

	title = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(title), 200, ft_asc + ft_desc + 4 /* framing and inside space */ + 4 /* below space */);
	gtk_signal_connect(GTK_OBJECT(title), "expose_event",
										 (GtkSignalFunc) xv_fragment_head_expose, x);
	gtk_signal_connect(GTK_OBJECT(title), "configure_event",
										 (GtkSignalFunc) xv_fragment_head_expose, x);
  gtk_widget_set_events(title, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);

	gtk_widget_show(title);
	return(title);
}


static gint xv_fragment_tail_expose(GtkWidget *w, GdkEventExpose *event, GdkGC *gc)
{
	if (!gc) gc = w->style->black_gc;
	
	/* Coloured line in the middle */

	gdk_draw_line(w->window, gc, 1, 0, 1, w->allocation.height);
	gdk_draw_line(w->window, gc, 2, 0, 2, w->allocation.height);

	/* Black vertical line on left */

	gdk_draw_line(w->window, w->style->black_gc, 0, 0, 0,
								w->allocation.height);

	/* Fill */
	
	gdk_draw_line(w->window, gc, 3, 1, 6, 1);
	gdk_draw_line(w->window, gc, 3, 2, 5, 2);
	gdk_draw_line(w->window, gc, 3, 3, 4, 3);
	
  /* _ */
	
	gdk_draw_line(w->window, w->style->black_gc, 3, 0, 7, 0);
	
	/* / */
	
	gdk_draw_line(w->window, w->style->black_gc, 7, 0, 3, 4);
	
	return(TRUE);
}


GtkWidget *xv_fragment_tail(TTREE *ds, TTREE *x)
{
	GdkColor gcol;
	GtkStyle *style;
	GtkWidget *line;
	int i;

	line = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(line), 8, 5);
	gtk_signal_connect(GTK_OBJECT(line), "expose_event",
										 (GtkSignalFunc) xv_fragment_tail_expose, ds_gc_get(ds, xml_frag_exit(x), 0));
	gtk_signal_connect(GTK_OBJECT(line), "configure_event",
										 (GtkSignalFunc) xv_fragment_tail_expose, ds_gc_get(ds, xml_frag_exit(x), 0));
  gtk_widget_set_events(line, GDK_EXPOSURE_MASK);

	xv_style_r(line, style_white);
	gtk_widget_show(line);
	return(line);
}


static gint xv_section_indent_expose(GtkWidget *w, GdkEventExpose *event, GdkGC *gc)
{
	if (!gc) gc = w->style->black_gc;
	/* Coloured line in the middle */

	gdk_draw_line(w->window, gc, 1, 0, 1, w->allocation.height);
	gdk_draw_line(w->window, gc, 2, 0, 2, w->allocation.height);

	/* Black vertical lines right and left */

	gdk_draw_line(w->window, w->style->black_gc, 0, 0, 0,
								w->allocation.height);

	gdk_draw_line(w->window, w->style->black_gc, 3, 0, 3,
								w->allocation.height);
	
	return(TRUE);
}


static gint xv_section_vline_and_space_expose(GtkWidget *w, GdkEventExpose *event, GdkGC *gc)
{
	if (!gc) gc = w->style->black_gc;
	/* Coloured line in the middle */
	
	gdk_draw_line(w->window, gc, 1, 0, 1, w->allocation.height);
	gdk_draw_line(w->window, gc, 2, 0, 2, w->allocation.height);
	
	/* Black vertical lines right and left */

	gdk_draw_line(w->window, w->style->black_gc, 0, 0, 0,
								w->allocation.height);

	gdk_draw_line(w->window, w->style->black_gc, 3, 0, 3,
								w->allocation.height);

	/* White space */
	
	gdk_draw_rectangle(w->window, w->style->white_gc, 1, 4, 0, 5, w->allocation.height);
	
	return(TRUE);
}


static gint xv_section_tail_expose(GtkWidget *w, GdkEventExpose *event, GdkGC *gc)
{
	GdkColor gcol;

	if (!gc) gc = w->style->black_gc;
	
#if 0	
	/* Start with desired background fill colour */

	gtk_style_set_background(w->style, w->window, GTK_STATE_NORMAL);
#endif

	/* Clear to white */

	gdk_draw_rectangle(w->window, w->style->white_gc, 1, 0, 0,
										 w->allocation.width,
										 w->allocation.height);

	/* | */
	
	gdk_draw_line(w->window, w->style->black_gc, 0, 0, 0, 9);
	
	/* / */
	
	gdk_draw_line(w->window, w->style->black_gc, 0, 9, 9, 0);
	
	/* _ */
	
	gdk_draw_line(w->window, w->style->black_gc, 9, 0, 3, 0);

	/* Fill */

	gdk_draw_line(w->window, gc, 1, 0, 2, 0);
	gdk_draw_line(w->window, gc, 1, 1, 7, 1);
	gdk_draw_line(w->window, gc, 1, 2, 6, 2);
	gdk_draw_line(w->window, gc, 1, 3, 5, 3);
	gdk_draw_line(w->window, gc, 1, 4, 4, 4);
	gdk_draw_line(w->window, gc, 1, 5, 3, 5);
	gdk_draw_line(w->window, gc, 1, 6, 2, 6);
	gdk_draw_line(w->window, gc, 1, 7, 1, 7);
	
	return(TRUE);
}


GtkWidget *xv_section_vline(TTREE *ds, TTREE *x)
{
	GtkWidget *line;

	line = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(line), 4, 0);
	gtk_signal_connect(GTK_OBJECT(line), "expose_event",
										 (GtkSignalFunc) xv_section_indent_expose, ds_gc_get(ds, x, 0));
	gtk_signal_connect(GTK_OBJECT(line), "configure_event",
										 (GtkSignalFunc) xv_section_indent_expose, ds_gc_get(ds, x, 0));
  gtk_widget_set_events(line, GDK_EXPOSURE_MASK);


	gtk_widget_show(line);
	return(line);
}


GtkWidget *xv_section_vline_and_space(TTREE *ds, TTREE *x)
{
	GtkWidget *line;

	line = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(line), 8, 0);
	gtk_signal_connect(GTK_OBJECT(line), "expose_event",
										 (GtkSignalFunc) xv_section_vline_and_space_expose, ds_gc_get(ds, x, 0));
	gtk_signal_connect(GTK_OBJECT(line), "configure_event",
										 (GtkSignalFunc) xv_section_vline_and_space_expose, ds_gc_get(ds, x, 0));
  gtk_widget_set_events(line, GDK_EXPOSURE_MASK);


	gtk_widget_show(line);
	return(line);
}


GtkWidget *xv_section_data(TTREE *x, TTREE *ds, int collapsed)
{
	GtkWidget *hbox, *line;
	struct xed *xed;

	hbox = gtk_hbox_new(FALSE, 0);

	line = xv_section_vline_and_space(ds, collapsed ? xml_frag_exit(xml_frag_exit(x)) :
													xml_frag_exit(x));

	gtk_box_pack_start(GTK_BOX(hbox), line, FALSE, TRUE, 0);

	xed = xmledit_new(x, ds);
#if 0
	gtk_box_pack_start(GTK_BOX(hbox), xed->w, FALSE, TRUE, 0);
#else
	gtk_box_pack_start(GTK_BOX(hbox), xed->w, TRUE, TRUE, 0);
#endif

#if 1
	gtk_widget_show(line);
	gtk_widget_show(hbox);
	gtk_widget_show(xed->w);
	gtk_widget_queue_resize(xed->w);
#endif
	return(hbox);
}


GtkWidget *xv_section_tail(TTREE *ds, TTREE *x)
{
	GdkColor gcol;
	GtkStyle *style;
	GtkWidget *line;
	int i;

	line = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(line), 10, 10);
	gtk_signal_connect(GTK_OBJECT(line), "expose_event",
										 (GtkSignalFunc) xv_section_tail_expose, ds_gc_get(ds, x, 0));
	gtk_signal_connect(GTK_OBJECT(line), "configure_event",
										 (GtkSignalFunc) xv_section_tail_expose, ds_gc_get(ds, x, 0));
  gtk_widget_set_events(line, GDK_EXPOSURE_MASK);

	gtk_widget_show(line);
	return(line);
}

TTREE *xv_editor_elements_skip(TTREE *x, TTREE *ds)
{
	TTREE *x0;

	for ( ; x; x = xml_frag_next(x))
	{
    int type = xml_frag_type(x);
		char *name = xml_frag_name_nice(x);

		if (type == XML_TAG_SPAN && ds_element_structural(ds, name))
		{
			return(xml_frag_prev(x));
		}
	}

	return(x);
}

void xv_style_r(GtkWidget *widget, gpointer data)
{
	GtkStyle *style;

	style = (GtkStyle *) data;
	gtk_widget_set_style(widget, style);

	if (GTK_IS_CONTAINER(widget))
	{
		gtk_container_foreach(GTK_CONTAINER(widget),
													xv_style_r, style);
	}
}

GtkWidget *xv_element_new(TTREE *x, TTREE *ds, GtkWidget *root, int collapsed, GtkWidget *tree_parent)
{
	GdkGCValuesMask gc_values_mask = GDK_GC_FOREGROUND /* | GDK_GC_FONT */;
	GdkGCValues     gc_values;

	TTREE *x_orig;
	TTREE *n0, *n1;
	GtkWidget *frame, *sub, *hbox, *poot, *glaebb_item, *glaebb_tree;
	struct xed *xed;
	unsigned int col;
	int i;

	glaebb_tree = 0;

	glaebb_item = gtk_tree_item_new_with_label(ds_name_get(x));
	gtk_tree_append(GTK_TREE(tree_parent), glaebb_item);
	tpopup_init(glaebb_item, x);

	gtk_widget_show(glaebb_item);

#if 0
	frame = gtk_event_box_new();
	gtk_widget_show(frame);
#endif


	x_orig = x;
#if 0	
	gtk_container_add(GTK_CONTAINER(frame), root);
#endif
	
	
	xv_style_r(root, style_white);


	x = xml_frag_enter(x);
	if (!x) return(0);

	for ( ; x; x = xml_frag_next(x))
	{
    int type = xml_frag_type(x);
		char *name = xml_frag_name_nice(x);

		if (type == XML_TAG_SPAN)
		{
			if (ds_element_structural(ds, name))
			{
				if (ds_element_collapse(ds, name))
				{
					if (!glaebb_tree)
					{
						glaebb_tree = gtk_tree_new();
						gtk_tree_item_set_subtree(GTK_TREE_ITEM(glaebb_item), glaebb_tree);
					}
					
					gtk_box_pack_start(GTK_BOX(root), xv_fragment_head(ds, x), FALSE, TRUE, 0);
					sub = xv_element_new(x, ds, root, 1, glaebb_tree);
					gtk_box_pack_start(GTK_BOX(root), xv_fragment_tail(ds, x), FALSE, TRUE, 0);
				}
				else
				{
					/* New structural element */
					
					if (!glaebb_tree)
					{
						glaebb_tree = gtk_tree_new();
						gtk_tree_item_set_subtree(GTK_TREE_ITEM(glaebb_item), glaebb_tree);
					}
					
					hbox = gtk_hbox_new(FALSE, 0);
					gtk_widget_show(hbox);

					gtk_box_pack_start(GTK_BOX(root), hbox, FALSE, TRUE, 0);
					gtk_box_pack_start(GTK_BOX(hbox), xv_section_vline_and_space(ds, xml_frag_exit(x)), FALSE, TRUE, 0);
					xv_style_r(hbox, style_white);
					poot = xv_section_head(ds, x);
					gtk_box_pack_start(GTK_BOX(hbox), poot, TRUE, TRUE, 0);
					sub = xv_element_new(x, ds, poot, 0, glaebb_tree);


					sub = xv_section_tail(ds, x);
					xv_style_r(sub, style_white);
					gtk_box_pack_start(GTK_BOX(poot), sub, FALSE, TRUE, 0);
				}
			}
			else if (ds_element_span(ds, name) ||
							 ds_element_insert(ds, name))
			{
				/* New editor window */
				
				sub = xv_section_data(x, ds, collapsed);
				if (sub)
				{
					gtk_box_pack_start(GTK_BOX(root), sub, FALSE, TRUE, 0);
					xv_style_r(sub, style_white);
				}
				
				x = xv_editor_elements_skip(x, ds);
			}
		}
		else if (type == XML_DATA)
		{
			/* New editor window */

			sub = xv_section_data(x, ds, collapsed);
			if (sub)
			{
				gtk_box_pack_start(GTK_BOX(root), sub, FALSE, TRUE, 0);
				xv_style_r(sub, style_white);
			}
			
			x = xv_editor_elements_skip(x, ds);
		}
	}

	xv_style_r(sub, style_white);
	gtk_tree_item_expand(GTK_TREE_ITEM(glaebb_item));
	return(root);
}

struct xview *xmlview_new(TTREE *x, TTREE *displayspec)
{
	struct xview *xv;
	GdkColor gcol;
	GtkWidget *w, *glaebb_tree;
	int i;

	g_message("xmlview_new called\n");
	
	curs.set = 0;
	curs.xed = 0;
	curs.w = 0;
	selection.xed = 0;
	selection.t0 = selection.t1 = 0;
	
	xv = malloc(sizeof(*xv));
	memset(xv, 0, sizeof(*xv));


	xv->x = x;

	gcol.blue = 0xffff;
	gcol.green = 0xffff;
	gcol.red = 0xffff;

	style_white = gtk_widget_get_default_style();
	style_white = gtk_style_copy(style_white);

	gdk_colormap_alloc_color(window->style->colormap, &gcol, 0, 1);

	for (i = 0; i < 5; i++) style_white->bg[i] = gcol;

	xv->tree = gtk_tree_item_new_with_label("Document");
	gtk_tree_append(GTK_TREE(tree), xv->tree);
	gtk_widget_show(xv->tree);

	glaebb_tree = gtk_tree_new();
	gtk_tree_item_set_subtree(GTK_TREE_ITEM(xv->tree), glaebb_tree);

	xv->w = gtk_vbox_new(FALSE, 0);
#if 1
	gtk_widget_show(xv->w);
#endif
	
	x = xml_frag_enter(x->child);  /* Root node specific */

	for ( ; x; x = xml_frag_next(x))
	{
	  int type = xml_frag_type(x);
	  char *name = xml_frag_name_nice(x);

	  g_message("examining frag \"%s\"\n",name);
		
		if (type == XML_TAG_SPAN && ds_element_structural(displayspec, name))
		{
			/* New element */

			w = xv_section_head(displayspec, x);
			gtk_box_pack_start(GTK_BOX(xv->w), w, TRUE, TRUE, 0);
			
			xv_element_new(x, displayspec, xv->w, 0, glaebb_tree);

			w = xv_section_tail(displayspec, x);
			xv_style_r(w, style_white);
			gtk_box_pack_start(GTK_BOX(xv->w), w, FALSE, TRUE, 0);
		}
	}

	gtk_tree_item_expand(GTK_TREE_ITEM(xv->tree));
	
#if 1
	gtk_widget_show_all(xv->w);
#endif
	
	return(xv);
}

void xmlview_destroy(int free_xml)
{
	if (!xv) return;

	gtk_widget_destroy(xv->tree);
	gtk_widget_destroy(xv->w);
	if (free_xml) ttree_branch_remove(xv->x);
	
	free(xv);
	xv = 0;
}
