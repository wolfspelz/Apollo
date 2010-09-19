// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Element_H_INCLUDED)
#define Element_H_INCLUDED

#include <cairo.h>
#include <cairo-win32.h>

class Color
{
public:
  Color(double r, double g, double b, double a)
    :r_(r)
    ,g_(g)
    ,b_(b)
    ,a_(a)
  {}

  double r_;
  double g_;
  double b_;
  double a_;
};

class Element
{
public:
  Element() {}
  virtual ~Element() {}

  virtual void Draw(cairo_t* cr) = 0;

protected:
};

class Shape: public Element
{
public:
  Shape(
    bool bFill, float fFillColorRed, float fFillColorGreen, float fFillColorBlue, float fFillColorAlpha, 
    bool bStroke, float fStrokeWidth, float fStrokeColorRed, float fStrokeColorGreen, float fStrokeColorBlue, float fStrokeColorAlpha
    )
    :fill(bFill) ,fill_color(fFillColorRed, fFillColorGreen, fFillColorBlue, fFillColorAlpha)
    ,stroke(bStroke) ,stroke_width(fStrokeWidth) ,stroke_color(fStrokeColorRed, fStrokeColorGreen, fStrokeColorBlue, fStrokeColorAlpha)
  {}
  virtual ~Shape() {}

  void Draw(cairo_t* cr) = 0;

protected:
  bool fill;
  Color fill_color;
  bool stroke;
  Color stroke_color;
  double stroke_width;
};

class RectangleX: public Shape
{
public:
  RectangleX(
    bool bFill, float fFillColorRed, float fFillColorGreen, float fFillColorBlue, float fFillColorAlpha, 
    bool bStroke, float fStrokeWidth, float fStrokeColorRed, float fStrokeColorGreen, float fStrokeColorBlue, float fStrokeColorAlpha,
    float fX, float fY, float fW, float fH
    )
    :Shape(
      bFill, fFillColorRed, fFillColorGreen, fFillColorBlue, fFillColorAlpha, 
      bStroke, fStrokeWidth, fStrokeColorRed, fStrokeColorGreen, fStrokeColorBlue, fStrokeColorAlpha
    )
    ,x_(fX)
    ,y_(fY)
    ,width_(fW)
    ,height_(fH)
  {}
  virtual ~RectangleX() {}

  void Draw(cairo_t* cr);

protected:
  double x_;
  double y_;
  double width_;
  double height_;
};

#endif // Element_H_INCLUDED
