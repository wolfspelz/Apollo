// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Graphics.h"

void Shape::SetFillColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
  bFillColor_ = true;
  cFill_.r = fRed;
  cFill_.g = fGreen;
  cFill_.b = fBlue;
  cFill_.a = fAlpha;
}

void Shape::SetStrokeColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
  bStrokeColor_ = true;
  cStroke_.r = fRed;
  cStroke_.g = fGreen;
  cStroke_.b = fBlue;
  cStroke_.a = fAlpha;
}

void Shape::SetStrokeWidth(float fWidth)
{
  bStrokeColor_ = true;
  fStrokeWidth_ = fWidth;
}

// ----------------------------------------------------------

void RectangleX::Draw(cairo_t* cr)
{
  cairo_rectangle(cr, fX_, fY_, fW_, fH_);

  if (bFillColor_) {
    cairo_set_source_rgba(cr, cFill_.r, cFill_.g, cFill_.b, cFill_.a > 0.99 ? 0.99 : cFill_.a);
    if (bStrokeColor_) {
      cairo_fill_preserve(cr);
    } else {
      cairo_fill(cr);
    }
  }

  if (bStrokeColor_) {
    cairo_set_line_width(cr, fStrokeWidth_);
    cairo_set_source_rgba(cr, cStroke_.r, cStroke_.g, cStroke_.b, cStroke_.a > 0.99 ? 0.99 : cStroke_.a);
    cairo_stroke(cr);
  }
}

