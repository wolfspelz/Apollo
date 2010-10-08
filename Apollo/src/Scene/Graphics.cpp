// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "Local.h"
#include "Graphics.h"

void Shape::SetFillColor(double fRed, double fGreen, double fBlue, double fAlpha)
{
  bFillColor_ = true;
  cFill_.r = fRed;
  cFill_.g = fGreen;
  cFill_.b = fBlue;
  cFill_.a = fAlpha;
}

void Shape::SetStrokeColor(double fRed, double fGreen, double fBlue, double fAlpha)
{
  bStrokeColor_ = true;
  cStroke_.r = fRed;
  cStroke_.g = fGreen;
  cStroke_.b = fBlue;
  cStroke_.a = fAlpha;
}

void Shape::SetStrokeWidth(double fWidth)
{
  bStrokeColor_ = true;
  fStrokeWidth_ = fWidth;
}

void Shape::FillAndStroke(GraphicsContext& gc)
{
  if (bFillColor_) {
    cairo_set_source_rgba(gc.Cairo(), cFill_.r, cFill_.g, cFill_.b, cFill_.a > 0.99 ? 0.99 : cFill_.a);
    if (bStrokeColor_) {
      cairo_fill_preserve(gc.Cairo());
    } else {
      cairo_fill(gc.Cairo());
    }
  }

  if (bStrokeColor_) {
    cairo_set_line_width(gc.Cairo(), fStrokeWidth_);
    cairo_set_source_rgba(gc.Cairo(), cStroke_.r, cStroke_.g, cStroke_.b, cStroke_.a > 0.99 ? 0.99 : cStroke_.a);
    cairo_stroke(gc.Cairo());
  }
}

// ----------------------------------------------------------

void RectangleX::Draw(GraphicsContext& gc)
{
//  cairo_rectangle(gc.Cairo(), fX_, gc.nH_ - fY_ - fH_, fW_, fH_); // -V
  cairo_rectangle(gc.Cairo(), fX_, fY_, fW_, fH_);

  FillAndStroke(gc);
}

// ----------------------------------------------------------

void ImageX::SetImageData(const Apollo::Image& image)
{
  DeleteImageFile();

  image_.CopyData_PreMultiplyAlpha_mem_RGBA_to_cairo_ARGB_which_actually_is_BGRA_in_mem_on_little_endian(image);
  bData_ = true;
}

void ImageX::DeleteImageData()
{
  bData_ = false;
  image_.Free();
}

void ImageX::SetImageFile(const String& sFile)
{
  if (sFile.empty()) {
    DeleteImageFile();
  } else {
    DeleteImageData();

    bFile_ = true;
    sFile_ = sFile;
  }
}

void ImageX::DeleteImageFile()
{
  bFile_ = false;
  sFile_ = "";
}

void ImageX::GetSize(GraphicsContext& gc, double& fW, double& fH)
{  
  if (bData_) {
    fW = image_.Width();
    fH = image_.Height();
  } else if (bFile_) {
    cairo_surface_t* pImage = cairo_image_surface_create_from_png(sFile_);
    if (pImage) {
      fW = cairo_image_surface_get_width(pImage);
      fH = cairo_image_surface_get_height(pImage);
      cairo_surface_destroy(pImage);
    }
  }
}

void ImageX::Draw(GraphicsContext& gc)
{
  cairo_surface_t* pImage = 0;
  
  if (bData_) {
    pImage = cairo_image_surface_create_for_data((unsigned char *) image_.Pixels(), CAIRO_FORMAT_ARGB32, image_.Width(), image_.Height(), image_.Width() * 4);
  } else if (bFile_) {
    pImage = cairo_image_surface_create_from_png(sFile_);
  }

  if (pImage) {
    //int nImageW = cairo_image_surface_get_width(pImage);
    int nImageH = cairo_image_surface_get_height(pImage);
    cairo_set_source_surface(gc.Cairo(), pImage, fX_, fY_);
//    cairo_set_source_surface(gc.Cairo(), pImage, fX_, gc.nH_ - fY_ - nImageH); // -V
    cairo_paint(gc.Cairo());

    cairo_surface_destroy(pImage);
  }
}

// ----------------------------------------------------------

void TextX::Draw(GraphicsContext& gc)
{
//  cairo_move_to(gc.Cairo(), fX_, gc.nH_ - fY_); // -V
  cairo_move_to(gc.Cairo(), fX_, fY_);
  cairo_select_font_face(gc.Cairo(), sFont_, nFlags_ & Italic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL, nFlags_ & Bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(gc.Cairo(), fSize_);
  cairo_text_path(gc.Cairo(), sText_);

  FillAndStroke(gc);
}

void TextX::Measure(GraphicsContext& gc, TextExtents& te)
{
  cairo_select_font_face(gc.Cairo(), sFont_, nFlags_ & Italic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL, nFlags_ & Bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(gc.Cairo(), fSize_);

  cairo_text_extents_t extents;
  cairo_text_extents(gc.Cairo(), sText_, &extents);

  te.fBearingX_ = extents.x_bearing;
  te.fBearingY_ = extents.y_bearing;
  te.fWidth_ = extents.width;
  te.fHeight_ = extents.height;
  te.fAdvanceX_ = extents.x_advance;
  te.fAdvanceY_ = extents.y_advance;
}
