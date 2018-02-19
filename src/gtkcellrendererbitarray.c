/**
 * @file gtkcellrendererbitarray.c
 *
 * @brief Custom Cell Rederer for GBitArray.
 *
 *  Sun Oct 10 02:31:32 2004
 *  Copyright  2004  Alejandro Claro
 *  aleo@apollyon.no-ip.com
 */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
/* INCLUDES *****************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#ifdef ENABLE_NLS
#define P_(String) dgettext(GETTEXT_PACKAGE "-properties",String)
#else 
#define P_(String) (String)
#endif

#include "gbitarray.h"
#include "gtkcellrendererbitarray.h"

/* PRIVATE FUNCTIONS PROTOTYPES *********************************************/

static void gtk_cell_renderer_bitarray_init(GtkCellRendererBitarray *cellbitarray);
static void gtk_cell_renderer_bitarray_class_init(GtkCellRendererBitarrayClass *klass);
static void gtk_cell_renderer_bitarray_get_property(GObject *object, guint param_id, GValue *value, GParamSpec *pspec);
static void gtk_cell_renderer_bitarray_set_property(GObject *object, guint param_id, const GValue *value, GParamSpec *pspec);
static void gtk_cell_renderer_bitarray_get_size(GtkCellRenderer *cell, GtkWidget *widget, GdkRectangle *cell_area, gint *x_offset, gint *y_offset, gint *width, gint *height);
static void gtk_cell_renderer_bitarray_render(GtkCellRenderer *cell, GdkWindow *window, GtkWidget *widget, GdkRectangle *background_area, GdkRectangle *cell_area, GdkRectangle *expose_area, guint flags);

/* TYPEDEF AND ENUMS ********************************************************/

enum
{
  PROP_FIRSTBIT = 1,
  PROP_BITS,
  PROP_GBITARRAY
};

#define MIN_CELL_WIDTH  100
#define MIN_CELL_HEIGHT 10

/* GLOBALS ******************************************************************/

static gpointer parent_class;

/* FUNCTIONS ****************************************************************/

/**
 * @brief here register cell renderer pieces type with the GObject 
 *        type system if it hasn't done so yet. 
 */
GType
gtk_cell_renderer_bitarray_get_type(void)
{
  static GType gtk_cell_bitarray_type = 0;

  if (!gtk_cell_bitarray_type)
  {
    static const GTypeInfo gtk_cell_bitarray_info =
    {
      sizeof(GtkCellRendererBitarrayClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) gtk_cell_renderer_bitarray_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(GtkCellRendererBitarray),
      0,    /* n_preallocs */
      (GInstanceInitFunc) gtk_cell_renderer_bitarray_init,
      NULL
    };

    gtk_cell_bitarray_type = g_type_register_static(GTK_TYPE_CELL_RENDERER,
                                              "GtkCellRendererBitarray",
                                              &gtk_cell_bitarray_info, 0);
  }

  return gtk_cell_bitarray_type;
}

/**
 * @brief set some default properties of the parent(GtkCellRenderer).
 *
 * @param cellrendererpieces: the Pieces Cell Renderer.
 */
static void
gtk_cell_renderer_bitarray_init(GtkCellRendererBitarray *cellbitarray)
{
  GtkCellRenderer *cell = GTK_CELL_RENDERER(cellbitarray);

  cellbitarray->first_bit = 0;
  cellbitarray->bits = 0;
  cellbitarray->bit_array = NULL;
  
  cell->mode = GTK_CELL_RENDERER_MODE_INERT;
  cell->xpad = 2;
  cell->ypad = 2;

  return;
}

/**
 *  @brief override the parent's functions that we need to implement.
 *
 * @param  klass: the Pieces Cell Renderer Class.
 */
static void
gtk_cell_renderer_bitarray_class_init(GtkCellRendererBitarrayClass *klass)
{
  GtkCellRendererClass *cell_class   = GTK_CELL_RENDERER_CLASS(klass);
  GObjectClass         *object_class = G_OBJECT_CLASS(klass);

  parent_class = g_type_class_peek_parent(klass);
  
  object_class->get_property = gtk_cell_renderer_bitarray_get_property;
  object_class->set_property = gtk_cell_renderer_bitarray_set_property;

  cell_class->get_size = gtk_cell_renderer_bitarray_get_size;
  cell_class->render   = gtk_cell_renderer_bitarray_render;

  g_object_class_install_property(object_class, PROP_FIRSTBIT,
                                  g_param_spec_uint("first_bit",
                                                    P_("First Bit"),
                                                    P_("The first bit in the bit array for this cell"),
                                                    0, G_MAXUINT, 0,
                                                    G_PARAM_READWRITE));

  g_object_class_install_property(object_class, PROP_BITS,
                                  g_param_spec_uint("bits",
                                                    P_("bits"),
                                                    P_("The number of bits for this cell"),
                                                    0, G_MAXUINT, 0,
                                                    G_PARAM_READWRITE));

  g_object_class_install_property(object_class, PROP_GBITARRAY,
                                  g_param_spec_object("bit_array",
                                                      P_("Bit Array"),
                                                      P_("The GBitArray Object"),
                                                      G_TYPE_BITARRAY,
                                                      G_PARAM_READWRITE));

  return;
}

/**
 * @brief get the properties.
 */
static void
gtk_cell_renderer_bitarray_get_property(GObject *object, guint param_id,
                                        GValue  *value,  GParamSpec *psec)
{
  GtkCellRendererBitarray *cell = GTK_CELL_RENDERER_BITARRAY(object);

  switch (param_id)
  {
    case PROP_FIRSTBIT:
      g_value_set_uint(value, cell->first_bit);
      break;
    case PROP_BITS:
      g_value_set_uint(value, cell->bits);
      break;
    case PROP_GBITARRAY:
      g_value_set_object(value, (gpointer)cell->bit_array);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, psec);
  }
  
  return;
}

/**
 * @brief set properties.
 */
static void
gtk_cell_renderer_bitarray_set_property(GObject *object, guint param_id,
                                        const GValue *value, GParamSpec *pspec)
{
  GtkCellRendererBitarray *cell = GTK_CELL_RENDERER_BITARRAY(object);

  switch (param_id)
  {
    case PROP_FIRSTBIT:
      cell->first_bit =  g_value_get_uint(value);
      break;
    case PROP_BITS:
      cell->bits =  g_value_get_uint(value);
      break;
    case PROP_GBITARRAY:
      cell->bit_array = G_BITARRAY(g_value_get_object(value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
  }
  
  return;
}

/**
 * @brief  new cell renderer.
 *
 * @return a new GtkCellRenderer that is really a GtkCellRendererBitarray.
 */
GtkCellRenderer *
gtk_cell_renderer_bitarray_new(void)
{
  return g_object_new(GTK_TYPE_CELL_RENDERER_BITARRAY, NULL);
}

/**
 * @brief crucial - calculate the size of the cell, taking into account
 *        padding and alignment properties of parent.
 */
static void
gtk_cell_renderer_bitarray_get_size(GtkCellRenderer *cell, GtkWidget *widget,
                                    GdkRectangle *cell_area, gint *x_offset,
                                    gint *y_offset, gint *width, gint *height)
{
  gint calc_width;
  gint calc_height;

  if (cell_area)
  {
    calc_width = MAX(MIN_CELL_WIDTH, cell_area->width);
    calc_height = MAX(MIN_CELL_HEIGHT, cell_area->height);

    if (x_offset)
    {
      *x_offset = cell->xalign*(cell_area->width - calc_width);
      *x_offset = MAX(*x_offset, 0);
    }

    if (y_offset)
    {
      *y_offset = cell->yalign*(cell_area->height - calc_height);
      *y_offset = MAX(*y_offset, 0);
    }
  }
  else
  {
    calc_width  = MIN_CELL_WIDTH;
    calc_height = MIN_CELL_HEIGHT;
  }

  if (width)
    *width = calc_width;

  if (height)
    *height = calc_height;

  return;
}

/**
 * @brief crucial - do the rendering.
 */
static void
gtk_cell_renderer_bitarray_render(GtkCellRenderer *cell, GdkWindow *window,
                                  GtkWidget *widget, GdkRectangle *background_area,
                                  GdkRectangle *cell_area, GdkRectangle *expose_area,
                                  guint flags)
{
  GdkGC *gc;
  GdkColor linecolor;
  guint16 rbg, gbg, bbg, rfg, gfg, bfg;
  gdouble frac;
  guint x, y, w, h, x_offset, y_offset, thick;
  guint i, j, p1, p2, good;
  GtkCellRendererBitarray *cellarray = GTK_CELL_RENDERER_BITARRAY(cell);
  
  gtk_cell_renderer_bitarray_get_size(cell, widget, cell_area, (gint*)&x_offset, 
                                      (gint*)&y_offset, (gint*)&w, (gint*)&h);

  x = cell_area->x + cell->xpad + x_offset;
  y = cell_area->y + cell->ypad + y_offset;
  w -= 2*cell->xpad;
  h -= 2*cell->ypad;

  gc = gdk_gc_new(window);

  gdk_gc_set_rgb_fg_color(gc, &widget->style->fg[GTK_STATE_NORMAL]);
  gdk_draw_rectangle(window, gc, TRUE, x, y, w, h);

  thick = MAX(widget->style->xthickness, widget->style->ythickness);
  if(thick <=0)
    thick = 1;

  x += thick;
  y += thick;
  w -= 2*thick;
  h -= 2*thick;

  gdk_gc_set_rgb_fg_color(gc, &widget->style->bg[GTK_STATE_NORMAL]);
  gdk_draw_rectangle(window, gc, TRUE, x, y, w, h); 
    
  rbg = widget->style->bg[GTK_STATE_NORMAL].red;
  rbg -= 0.1*rbg;
  gbg = widget->style->bg[GTK_STATE_NORMAL].green;
  gbg -= 0.1*gbg;
  bbg = widget->style->bg[GTK_STATE_NORMAL].blue;
  bbg -= 0.1*bbg;
  rfg = widget->style->bg[GTK_STATE_SELECTED].red;
  gfg = widget->style->bg[GTK_STATE_SELECTED].green;
  bfg = widget->style->bg[GTK_STATE_SELECTED].blue;
  linecolor.pixel = 0;

  if(cellarray->bit_array != NULL)
  {
    for(i = 0, good = 0; i < w; i++, good = 0)
    {
      p1 = (i*cellarray->bits)/w;
      p2 = ((i+1)*cellarray->bits)/w;
      if(p2 == p1)
       p2++;

      for(j = p1; j < p2; j++)
      {
        if(g_bitarray_get_bit(cellarray->bit_array, cellarray->first_bit+j))
          good++;
      }
 
      if(good>0)
      {
        frac = ((gdouble)good)/(p2-p1);
        linecolor.red = rbg + frac*(rfg-rbg);
        linecolor.green = gbg + frac*(gfg-gbg);
        linecolor.blue = bbg + frac*(bfg-bbg);
        gdk_gc_set_rgb_fg_color(gc, &linecolor); 
        gdk_draw_rectangle(window, gc, TRUE, x+i, y, 1, h);    
      }
    }
  }

  g_object_unref(G_OBJECT(gc));
  return;  
}
