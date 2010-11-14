// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Primitives.h"
#include "Scene.h"

double ShapeElement::PI = 3.14159265358979323;

void ShapeElement::SetPosition(double fX, double fY)
{
  fX_ = fX;
  fY_ = fY;
}

void ShapeElement::SetFillColor(double fRed, double fGreen, double fBlue, double fAlpha)
{
  bFillColor_ = true;
  bFillImageFile_ = false;
  cFill_.r = fRed;
  cFill_.g = fGreen;
  cFill_.b = fBlue;
  cFill_.a = fAlpha;
}

void ShapeElement::SetStrokeColor(double fRed, double fGreen, double fBlue, double fAlpha)
{
  bStrokeColor_ = true;
  bStrokeImageFile_ = false;
  cStroke_.r = fRed;
  cStroke_.g = fGreen;
  cStroke_.b = fBlue;
  cStroke_.a = fAlpha;
}

void ShapeElement::SetStrokeImageFile(const String& sFile)
{
  bStrokeColor_ = false;
  bStrokeImageFile_ = true;
  sStrokeImageFile_ = sFile;
}

void ShapeElement::SetFillImageFile(const String& sFile)
{
  bFillColor_ = false;
  bFillImageFile_ = true;
  sFillImageFile_ = sFile;
}

void ShapeElement::SetStrokeImageOffset(double fX, double fY)
{
  fStrokeImageX_ = fX;
  fStrokeImageY_ = fY;
}

void ShapeElement::SetFillImageOffset(double fX, double fY)
{
  fFillImageX_ = fX;
  fFillImageY_ = fY;
}

void ShapeElement::SetStrokeWidth(double fWidth)
{
  fStrokeWidth_ = fWidth;
}

void ShapeElement::FillAndStroke(DrawContext& gc)
{
  // Fill

  cairo_surface_t* pFillImage = 0;
  if (bFillImageFile_) {
    pFillImage = cairo_image_surface_create_from_png(sFillImageFile_);
    if (pFillImage) {
      cairo_scale(gc.Cairo(), 1.0, -1.0);
      cairo_set_source_surface(gc.Cairo(), pFillImage, fFillImageX_, fFillImageY_);
      cairo_scale(gc.Cairo(), 1.0, -1.0);
    }
  }

  if (bFillColor_) {
    cairo_set_source_rgba(gc.Cairo(), cFill_.r, cFill_.g, cFill_.b, cFill_.a > 0.99 ? 0.99 : cFill_.a);
  }

  if (bFillColor_ || bFillImageFile_) {
    if (bStrokeColor_ || bStrokeImageFile_) {
      cairo_fill_preserve(gc.Cairo());
    } else {
      cairo_fill(gc.Cairo());
    }
  }

  if (pFillImage) {
    cairo_surface_destroy(pFillImage);
    pFillImage = 0;
  }

  // Stroke

  cairo_surface_t* pStrokeImage = 0;
  if (bStrokeImageFile_) {
    pStrokeImage = cairo_image_surface_create_from_png(sStrokeImageFile_);
    if (pStrokeImage) {
      cairo_scale(gc.Cairo(), 1.0, -1.0);
      cairo_set_source_surface(gc.Cairo(), pStrokeImage, fStrokeImageX_, fStrokeImageY_);
      cairo_scale(gc.Cairo(), 1.0, -1.0);
    }
  }

  if (bStrokeColor_) {
    cairo_set_source_rgba(gc.Cairo(), cStroke_.r, cStroke_.g, cStroke_.b, cStroke_.a > 0.99 ? 0.99 : cStroke_.a);
  }
  if (bStrokeColor_ || bStrokeImageFile_) {
    cairo_set_line_width(gc.Cairo(), fStrokeWidth_);
    cairo_stroke(gc.Cairo());
  }

  if (pStrokeImage) {
    cairo_surface_destroy(pStrokeImage);
    pStrokeImage = 0;
  }
}

// ----------------------------------------------------------

void RectangleElement::Draw(DrawContext& gc)
{
  if (HasFillOrStroke()) {
    if (gc.bLogDraw_) {
      gc.sLogDraw_.appendf(" x=%.3f y=%.3f w=%.3f h=%.3f", fX_, fY_, fW_, fH_);
    }

    switch (nCorners_) {
      case NormalCorners:
        cairo_rectangle(gc.Cairo(), fX_, fY_, fW_, fH_);
        break;

      case RoundCorners:
        {
          double fRadius = fRadius_;
          double fH2 = fH_ / 2.0;
          double fW2 = fW_ / 2.0;
          if (fRadius == 0.0 || fRadius > fW2 || fRadius > fH2) {
            fRadius = fW2 < fH2 ? fW2 / 2.0 : fH2 / 2.0;
          }

          if (gc.bLogDraw_) {
            gc.sLogDraw_.appendf(" round=%.3f", fRadius);
          }

          cairo_move_to(gc.Cairo(), fX_, fY_ + fRadius);
	        cairo_arc(gc.Cairo(), fX_ + fRadius, fY_ + fRadius, fRadius, PI, -PI / 2);
	        cairo_line_to(gc.Cairo(), fX_ + fW_ - fRadius, fY_);
	        cairo_arc(gc.Cairo(), fX_ + fW_ - fRadius, fY_ + fRadius, fRadius, -PI / 2, 0);
	        cairo_line_to(gc.Cairo(), fX_ + fW_, fY_ + fH_ - fRadius);
	        cairo_arc(gc.Cairo(), fX_ + fW_ - fRadius, fY_ + fH_ - fRadius, fRadius, 0, PI / 2);
	        cairo_line_to(gc.Cairo(), fX_ + fRadius, fY_ + fH_);
	        cairo_arc(gc.Cairo(), fX_ + fRadius, fY_ + fH_ - fRadius, fRadius, PI / 2, PI);
	        cairo_close_path(gc.Cairo());

        }
        break;

      case CurvedCorners:
        if (gc.bLogDraw_) {
          gc.sLogDraw_.appendf(" curved");
        }

        cairo_move_to(gc.Cairo(), fX_, fY_ + fH_ / 2);
	      cairo_curve_to(gc.Cairo(), fX_, fY_, fX_, fY_, fX_ + fW_ / 2, fY_);
	      cairo_curve_to(gc.Cairo(), fX_ + fW_, fY_, fX_ + fW_, fY_, fX_ + fW_, fY_ + fH_ / 2);
	      cairo_curve_to(gc.Cairo(), fX_ + fW_, fY_ + fH_, fX_ + fW_, fY_ + fH_, fX_ + fW_ / 2, fY_ + fH_);
	      cairo_curve_to(gc.Cairo(), fX_, fY_ + fH_, fX_, fY_ + fH_, fX_, fY_ + fH_ / 2);
        break;
    }
    
    FillAndStroke(gc);
  }
}

// ----------------------------------------------------------

void ImageElement::SetData(const Apollo::Image& image)
{
  DeleteFile();

  image_.CopyData_PreMultiplyAlpha_mem_RGBA_to_cairo_ARGB_which_actually_is_BGRA_in_mem_on_little_endian(image);
  bData_ = true;
}

void ImageElement::DeleteData()
{
  bData_ = false;
  image_.Free();
}

void ImageElement::SetFile(const String& sFile)
{
  if (sFile.empty()) {
    DeleteFile();
  } else {
    DeleteData();

    bFile_ = true;
    sFile_ = sFile;
  }
}

void ImageElement::DeleteFile()
{
  bFile_ = false;
  sFile_ = "";
}

void ImageElement::GetSize(DrawContext& gc, double& fW, double& fH)
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
      pImage = 0;
    }
  }
}

void ImageElement::Draw(DrawContext& gc)
{
  cairo_surface_t* pImage = 0;
  
  if (bData_) {
    if (gc.bLogDraw_) {
      gc.sLogDraw_.appendf(" data");
    }
    pImage = cairo_image_surface_create_for_data((unsigned char *) image_.Pixels(), CAIRO_FORMAT_ARGB32, image_.Width(), image_.Height(), image_.Width() * 4);
  } else if (bFile_) {
    if (gc.bLogDraw_) {
      gc.sLogDraw_.appendf(" %s", StringType(sFile_));
    }
    pImage = cairo_image_surface_create_from_png(sFile_);
  }

  if (pImage) {
    cairo_status_t nCairoStatus = cairo_surface_status(pImage);
    if (nCairoStatus != CAIRO_STATUS_SUCCESS) {
      if (bData_) {
        apLog_Error((LOG_CHANNEL, "ImageElement::Draw", "cairo_image_surface_create_for_data failed err=%d:%s w=%d h=%d", nCairoStatus, cairo_status_to_string(nCairoStatus), image_.Width(), image_.Height()));
        DeleteData();
        this->image_.Free();
      } else if (bFile_) {
        apLog_Error((LOG_CHANNEL, "ImageElement::Draw", "cairo_image_surface_create_from_pngfailed err=%d:%s file=%s", nCairoStatus, cairo_status_to_string(nCairoStatus), StringType(sFile_)));
        DeleteFile();
      }
    } else {

      //int nImageW = cairo_image_surface_get_width(pImage);
      int nImageH = cairo_image_surface_get_height(pImage);
      cairo_scale(gc.Cairo(), 1.0, -1.0);
      cairo_translate(gc.Cairo(), 0.0, -nImageH);
      cairo_set_source_surface(gc.Cairo(), pImage, fX_, -fY_);
      cairo_translate(gc.Cairo(), 0.0, nImageH);
      cairo_scale(gc.Cairo(), 1.0, -1.0);
      if (fAlpha_ > 0.99) {
        cairo_paint(gc.Cairo());
      } else {
        cairo_paint_with_alpha(gc.Cairo(), fAlpha_);
      }

    }

    cairo_surface_destroy(pImage);
  }
}

// ----------------------------------------------------------

void TextElement::Draw(DrawContext& gc)
{
  if (HasFillOrStroke()) {
    if (gc.bLogDraw_) {
      gc.sLogDraw_.appendf(" x=%.3f y=%.3f fl=%d \"%s\"", fX_, fY_, nFlags_, StringType(sText_));
    }

    cairo_move_to(gc.Cairo(), fX_, fY_);
    cairo_select_font_face(gc.Cairo(), sFont_, nFlags_ & Italic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL, nFlags_ & Bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(gc.Cairo(), fSize_);
    cairo_scale(gc.Cairo(), 1.0, -1.0);
    cairo_text_path(gc.Cairo(), sText_);
    cairo_scale(gc.Cairo(), 1.0, -1.0);

    FillAndStroke(gc);
  }
}

void TextElement::GetExtents(DrawContext& gc, TextExtents& te)
{
  cairo_select_font_face(gc.Cairo(), sFont_, nFlags_ & Italic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL, nFlags_ & Bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(gc.Cairo(), fSize_);

  cairo_text_extents_t extents;
  cairo_text_extents(gc.Cairo(), sText_, &extents);

  te.fBearingX_ = extents.x_bearing;
  te.fBearingY_ = -extents.y_bearing;
  te.fWidth_ = extents.width;
  te.fHeight_ = extents.height;
  te.fAdvanceX_ = extents.x_advance;
  te.fAdvanceY_ = extents.y_advance;
}

// ----------------------------------------------------------

void SensorElement::Draw(DrawContext& gc)
{
  if (HasFillOrStroke()) {
    cairo_rectangle(gc.Cairo(), fX_, fY_, fW_, fH_);
    FillAndStroke(gc);
  }
}

void SensorElement::MouseCaptured()
{
  bCaptured_ = 1;
}

void SensorElement::MouseReleased()
{
  bCaptured_ = 0;
}

void SensorElement::MouseEvent(EventContext& gc, double fX, double fY)
{
  int bHit = 0;
  int bInside = 0;

  fX -= fTranslateX_;
  fY -= fTranslateY_;

  if (bCaptured_) {
    bHit = 1;
  }

  if (fX_ < fX && fX_ + fW_ > fX && fY_ < fY && fY_ + fH_ > fY) {
    bInside = 1;
    bHit = 1;
  }

  int nEvent = 0;

  if (gc.bTimer_) {

    if (!bHit && bSentMouseMove_) {

      Msg_Scene_MouseEvent msg;
      msg.hScene = pScene_->apHandle();
      msg.sPath = sPath_;
      msg.nEvent = EventContext::MouseOut;
      msg.nButton = EventContext::NoMouseButton;
      msg.fX = fX;
      msg.fY = fY;
      msg.bInside = bInside;
      msg.Send();
      nEvent = msg.nEvent;

      bSentMouseMove_ = 0;
    }

  } else {

    if (bHit) {
      Msg_Scene_MouseEvent msg;
      msg.hScene = pScene_->apHandle();
      msg.sPath = sPath_;
      msg.nEvent = gc.nEvent_;
      msg.nButton = gc.nButton_;
      msg.fX = fX;
      msg.fY = fY;
      msg.bInside = bInside;
      msg.Send();
      nEvent = msg.nEvent;

      gc.bFired_ = 1;

      if (gc.nEvent_ == EventContext::MouseMove) {
        bSentMouseMove_ = 1;
      }
    }

  }

  //apLog_Debug((LOG_CHANNEL, "SensorElement::MouseEvent", "timer=%d hit=%d x=%d y=%d ev=%d", gc.bTimer_, bHit, (int) fX, (int) fY, nEvent));
}

