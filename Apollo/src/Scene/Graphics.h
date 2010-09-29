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

#include <cairo.h>
#include <cairo-win32.h>

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

  virtual void Draw(cairo_t* cr) = 0;

  virtual bool IsRectangle() { return false; }
  virtual bool IsShape() { return false; }

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

  virtual void SetFillColor(float fRed, float fGreen, float fBlue, float fAlpha);
  virtual void SetStrokeColor(float fRed, float fGreen, float fBlue, float fAlpha);
  virtual void SetStrokeWidth(float fWidth);

  void Draw(cairo_t* cr) = 0;

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
  RectangleX(float fX, float fY, float fW, float fH)
    :fX_(fX)
    ,fY_(fY)
    ,fW_(fW)
    ,fH_(fH)
  {}
  virtual ~RectangleX() {}

  virtual bool IsRectangle() { return true; }

  virtual void SetCoordinates(float fX, float fY, float fW, float fH) { fX_ = fX; fY_ = fY; fW_ = fW; fH_ = fH; }

  void Draw(cairo_t* cr);

protected:
  double fX_;
  double fY_;
  double fW_;
  double fH_;
};

#endif // Graphics_H_INCLUDED
