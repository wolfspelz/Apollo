// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Element.h"

void RectangleX::Draw(cairo_t* cr)
{
  cairo_rectangle(cr, x_, y_, width_, height_);
  if (fill) {
    cairo_set_source_rgba(cr, fill_color.r_, fill_color.g_, fill_color.b_, fill_color.a_);
    if (stroke) {
      cairo_fill_preserve(cr);
    } else {
      cairo_fill(cr);
    }
  }
  if (stroke) {
    cairo_set_line_width(cr, stroke_width);
    cairo_set_source_rgba(cr, stroke_color.r_, stroke_color.g_, stroke_color.b_, stroke_color.a_);
    cairo_stroke(cr);
  }
}

