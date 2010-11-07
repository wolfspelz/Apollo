// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Element_H_INCLUDED)
#define Element_H_INCLUDED

#include "Apollo.h"
#include "GraphicsContext.h"

#if defined(AP_TEST)
#include "SceneModuleTester.h"
#endif // #if defined(AP_TEST)

class Element;
class Scene;

typedef StringPointerTree<Element*> ElementList;
typedef StringPointerTreeNode<Element*> ElementNode;
typedef StringPointerTreeIterator<Element*> ElementIterator;

class RectangleElement;
class TextElement;
class ShapeElement;
class ImageElement;
class SensorElement;

class Color
{
public:
  Color()
    :r(0.0)
    ,g(0.0)
    ,b(0.0)
    ,a(0.0)
  {}
  double r;
  double g;
  double b;
  double a;
};

class Element
{
public:
  Element(Scene* pScene)
    :pScene_(pScene)
    ,pChildren_(0)
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

  virtual void Draw(DrawContext& gc) {};
  void MouseEvent(EventContext& gc, double fX, double fY) {}

  void DrawRecursive(DrawContext& gc);  
  void MouseEventRecursive(EventContext& gc, double fX, double fY);

  Element* FindElement(const String& sPath);
  void AddChild(const String& sName, Element* pElement);
  Element* CreateElement(const String& sPath);
  int DeleteElement(const String& sPath);
  void GetChildren(Apollo::ValueList& vlChildren);

  void Translate(double fX, double fY);
  void Scale(double fX, double fY);
  void Rotate(double fAngle);
  void Hide(int bHide);
  void CopyMode(int nMode);

  void GetTranslate(double& fX, double& fY);
  void GetScale(double& fX, double& fY);
  void GetRotate(double& fRotate);

  inline virtual bool IsShape() { return false; }
  inline virtual bool IsRectangle() { return false; }
  inline virtual bool IsImage() { return false; }
  inline virtual bool IsText() { return false; }
  inline virtual bool IsSensor() { return false; }

  ShapeElement* AsShape();
  RectangleElement* AsRectangle();
  TextElement* AsText();
  ImageElement* AsImage();
  SensorElement* AsSensor();

protected:
  void CheckSaveRestore();

protected:
  Scene* pScene_;
  ElementList* pChildren_;

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
