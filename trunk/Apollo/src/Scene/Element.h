// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Element_H_INCLUDED)
#define Element_H_INCLUDED

#include "Apollo.h"
#include "Graphics.h"

#if defined(AP_TEST)
#include "SceneModuleTester.h"
#endif // #if defined(AP_TEST)

class Element;

typedef StringPointerTree<Element*> ElementList;
typedef StringPointerTreeNode<Element*> ElementNode;
typedef StringPointerTreeIterator<Element*> ElementIterator;

class Element
{
public:
  Element()
    :pChildren_(0)
    ,pGraphics_(0)
  {}
  virtual ~Element();

  virtual void Draw(GraphicsContext& gc);

  Element* FindElement(const String& sPath);
  int CreateElement(const String& sPath);
  int DeleteElement(const String& sPath);

  void SetRectangle(double fX, double fY, double fW, double fH);
  void SetText(double fX, double fY, const String& sText, const String& sFont, double fSize, int nFlags);
  void MeasureText(const String& sText, const String& sFont, double fSize, int nFlags, TextExtents& te);

  void SetFillColor(double fRed, double fGreen, double fBlue, double fAlpha);
  void SetStrokeColor(double fRed, double fGreen, double fBlue, double fAlpha);
  void SetStrokeWidth(double fWidth);

protected:
  ElementList* pChildren_;
  Graphics* pGraphics_;

#if defined(AP_TEST)
  friend class SceneModuleTester;
#endif
};

#endif // Element_H_INCLUDED
