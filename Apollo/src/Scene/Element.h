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
  Element(Surface* pSurface)
    :pSurface_(pSurface)
    ,pChildren_(0)
    ,pGraphics_(0)
    ,bSave_(false)
    ,fTranslateX_(0.0)
    ,fTranslateY_(0.0)
    ,fScaleX_(1.0)
    ,fScaleY_(1.0)
    ,fRotate_(0.0)
    ,nCopyMode_(CAIRO_OPERATOR_OVER)
    ,bHide_(0)
  {}
  virtual ~Element();

  void Draw(DrawContext& gc);
  void MouseEvent(EventContext& gc, double fX, double fY);

  Element* FindElement(const String& sPath);
  int CreateElement(const String& sPath);
  int DeleteElement(const String& sPath);

  void Translate(double fX, double fY);
  void GetTranslate(double& fX, double& fY);
  void Scale(double fX, double fY);
  void Rotate(double fAngle);
  void CopyMode(int nMode);
  void Hide(int bHide);

  Graphics* GetGraphics() { return pGraphics_; }
  void DeleteGraphics();
  void CreateRectangle(double fX, double fY, double fW, double fH);
  void CreateImageFromData(double fX, double fY, const Apollo::Image& image);
  void CreateImageFromFile(double fX, double fY, const String& sFile);
  void CreateText(double fX, double fY, const String& sText, const String& sFont, double fSize, int nFlags);
  void CreateMouseSensor(const String& sPath, double fX, double fY, double fW, double fH);

  void SetRectangle(double fX, double fY, double fW, double fH);
  void SetCoordinates(double fX, double fY);
  void SetFillColor(double fRed, double fGreen, double fBlue, double fAlpha);
  void SetStrokeColor(double fRed, double fGreen, double fBlue, double fAlpha);
  void SetStrokeImageFile(const String& sFile);
  void SetFillImageFile(const String& sFile);
  void SetStrokeImageOffset(double fX, double fY);
  void SetFillImageOffset(double fX, double fY);
  void SetStrokeWidth(double fWidth);
  void SetImageData(const Apollo::Image& image);
  void DeleteImageData();
  void SetImageFile(const String& sFile);
  void DeleteImageFile();
  void SetImageAlpha(double fAlpha);
  void CaptureMouse(const String& sPath);
  void ReleaseMouse();

protected:
  void CheckSaveRestore();

protected:
  Surface* pSurface_;
  ElementList* pChildren_;
  Graphics* pGraphics_;

  bool bSave_;
  double fTranslateX_;
  double fTranslateY_;
  double fScaleX_;
  double fScaleY_;
  double fRotate_;

  int nCopyMode_;

  int bHide_;

#if defined(AP_TEST)
  friend class SceneModuleTester;
#endif
};

#endif // Element_H_INCLUDED
