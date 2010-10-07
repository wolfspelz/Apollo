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
    ,bTransform_(false)
    ,fTranslateX_(0.0)
    ,fTranslateY_(0.0)
    ,fScaleX_(1.0)
    ,fScaleY_(1.0)
    ,fRotate_(0.0)
  {}
  virtual ~Element();

  virtual void Draw(GraphicsContext& gc);

  Element* FindElement(const String& sPath);
  int CreateElement(const String& sPath);
  int DeleteElement(const String& sPath);

  void Translate(double fX, double fY);
  void Scale(double fX, double fY);
  void Rotate(double fAngle);

  void CreateRectangle(double fX, double fY, double fW, double fH);
  void CreateImage(double fX, double fY, Apollo::Image& image);
  void CreateText(double fX, double fY, const String& sText, const String& sFont, double fSize, int nFlags);

  void SetFillColor(double fRed, double fGreen, double fBlue, double fAlpha);
  void SetStrokeColor(double fRed, double fGreen, double fBlue, double fAlpha);
  void SetStrokeWidth(double fWidth);
  void SetImageData(const Apollo::Image& image);
  void SetImageFile(const String& sFile);

protected:
  ElementList* pChildren_;
  Graphics* pGraphics_;

  bool bTransform_;
  double fTranslateX_;
  double fTranslateY_;
  double fScaleX_;
  double fScaleY_;
  double fRotate_;

#if defined(AP_TEST)
  friend class SceneModuleTester;
#endif
};

#endif // Element_H_INCLUDED
