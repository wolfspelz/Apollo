// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Element_H_INCLUDED)
#define Element_H_INCLUDED

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

class Element;

typedef StringPointerTree<Element*> ElementList;
typedef StringPointerTreeNode<Element*> ElementNode;
typedef StringPointerTreeIterator<Element*> ElementIterator;

class Element
{
public:
  Element() {}
  virtual ~Element() {}

  virtual void DrawAll(cairo_t* cr);
  virtual void Draw(cairo_t* cr) = 0;

  Element* FindElement(const String& sPath);
  void AddElement(const String& sPath, Element* pElement);

protected:
  ElementList list_;

#if defined(AP_TEST)
  friend class SceneModuleTester;
#endif
};

class Node: public Element
{
public:
  Node() {}
  virtual ~Node() {}

  void Draw(cairo_t* cr) {}

#if defined(AP_TEST)
  friend class SceneModuleTester;
#endif
};

class Shape: public Element
{
public:
  Shape()
    :bFillColor_(false)
    ,bStrokeColor_(false)
  {}
  virtual ~Shape() {}

  void SetFillColor(float fRed, float fGreen, float fBlue, float fAlpha);
  void SetStrokeColor(float fWidth, float fRed, float fGreen, float fBlue, float fAlpha);

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

  void SetCoordinates(float fX, float fY, float fW, float fH) { fX_ = fX; fY_ = fY; fW_ = fW; fH_ = fH; }

  void Draw(cairo_t* cr);

protected:
  double fX_;
  double fY_;
  double fW_;
  double fH_;
};

#endif // Element_H_INCLUDED
