// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Graphics_H_INCLUDED)
#define Graphics_H_INCLUDED

#include "Apollo.h"

#if defined(AP_TEST)
#include "SceneModuleTester.h"
#endif // #if defined(AP_TEST)

#include "GraphicsContext.h"

typedef struct _Color
{
  double r;
  double g;
  double b;
  double a;
} Color;

class Graphics
{
public:
  Graphics() {}
  virtual ~Graphics() {}

  virtual void Draw(GraphicsContext& gc) = 0;

  virtual bool IsRectangle() { return false; }
  virtual bool IsShape() { return false; }
  virtual bool IsText() { return false; }

#if defined(AP_TEST)
  friend class SceneModuleTester;
#endif
};

class Shape: public Graphics
{
public:
  Shape()
    :bFillColor_(false)
    ,bStrokeColor_(false)
    ,fStrokeWidth_(1.0)
  {}
  virtual ~Shape() {}

  virtual bool IsShape() { return true; }

  virtual void SetFillColor(double fRed, double fGreen, double fBlue, double fAlpha);
  virtual void SetStrokeColor(double fRed, double fGreen, double fBlue, double fAlpha);
  virtual void SetStrokeWidth(double fWidth);

  void Draw(GraphicsContext& gc) = 0;

protected:
  void FillAndStroke(GraphicsContext& gc);

protected:
  bool bFillColor_;
  Color cFill_;
  bool bStrokeColor_;
  Color cStroke_;
  double fStrokeWidth_;
};

class RectangleX: public Shape
{
public:
  RectangleX(double fX, double fY, double fW, double fH)
    :fX_(fX)
    ,fY_(fY)
    ,fW_(fW)
    ,fH_(fH)
  {}
  virtual ~RectangleX() {}

  virtual bool IsRectangle() { return true; }

  virtual void SetCoordinates(double fX, double fY, double fW, double fH) { fX_ = fX; fY_ = fY; fW_ = fW; fH_ = fH; }

  void Draw(GraphicsContext& gc);

protected:
  double fX_;
  double fY_;
  double fW_;
  double fH_;
};

class TextExtents
{
public:
  double fBearingX_;
  double fBearingY_;
  double fWidth_;
  double fHeight_;
  double fAdvanceX_;
  double fAdvanceY_;
};

class TextX: public Shape
{
public:
  // Same as Msg_Scene_SetText::FontFlags
  typedef enum _FontFlags { Normal = 0
    ,Italic = 1 << 0
    ,Bold = 1 << 1
    ,LastFlag = 1 << 2
  } FontFlags;

  TextX(double fX, double fY, const String& sText, const String& sFont, double fSize, int nFlags)
    :fX_(fX)
    ,fY_(fY)
    ,sText_(sText)
    ,sFont_(sFont)
    ,fSize_(fSize)
    ,nFlags_(nFlags)
  {}
  virtual ~TextX() {}

  virtual bool IsText() { return true; }

  virtual void SetCoordinates(double fX, double fY) { fX_ = fX; fY_ = fY; }
  virtual void SetString(const String& sText) { sText_ = sText; }
  virtual void SetFont(const String& sFont, double fSize, int nFlags) { sFont_ = sFont; fSize_ = fSize; nFlags_ = nFlags; }

  void Measure(GraphicsContext& gc, TextExtents& te);

  void Draw(GraphicsContext& gc);

protected:
  double fX_;
  double fY_;
  double fSize_;
  String sText_;
  String sFont_;
  int nFlags_;
};

#endif // Graphics_H_INCLUDED
