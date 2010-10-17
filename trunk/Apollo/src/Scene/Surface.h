// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Surface_H_INCLUDED)
#define Surface_H_INCLUDED

#include "Element.h"

//class SensorBox
//{
//public:
//  SensorBox(const ApHandle& hSensor, int nLeft, int nTop, int nRight, int nBottom)
//    :hAp_(hSensor)
//    ,nLeft_(nLeft)
//    ,nTop_(nTop)
//    ,nRight_(nRight)
//    ,nBottom_(nBottom)
//  {}
//
//  bool operator<(const SensorBox& other) const
//  {
//    if (nRight_ < other.nRight_) {
//      return true;
//    } else {
//      return hAp_ < other.hAp_;
//    }
//  }
//
//  ApHandle hAp_;
//  int nLeft_;
//  int nTop_;
//  int nRight_;
//  int nBottom_;
//};
//
//template <class INT_ID>
//class SensorBoxTreeNode : public TreeNode<SensorBox, INT_ID>
//{
//public:
//  inline SensorBox& Key(void) { return TreeNode<SensorBox, INT_ID>::key(); }
//  inline INT_ID& Value(void) { return TreeNode<SensorBox, INT_ID>::item(); }
//};
//
//template <class INT_ID>
//class SensorBoxTree : public Tree<SensorBox, INT_ID, LessThan<SensorBox> >
//{
//public:
//  inline SensorBoxTreeNode<INT_ID>* Find(const SensorBox &ext_id) { return (SensorBoxTreeNode<INT_ID>*) Tree<SensorBox, INT_ID, LessThan<SensorBox> >::Find(ext_id); }
//  inline SensorBoxTreeNode<INT_ID>* Next(SensorBoxTreeNode<INT_ID>* entry) { return (SensorBoxTreeNode<INT_ID>*) Tree<SensorBox, INT_ID, LessThan<SensorBox> >::Next(entry); }
//  inline int Get(const SensorBox &ext_id, INT_ID &int_id) { return Tree<SensorBox, INT_ID, LessThan<SensorBox> >::Get(ext_id, int_id); }
//  SensorBoxTreeNode<INT_ID>* FindByHandle(const ApHandle& hSensor);
//};
//
//template <class INT_ID>
//class SensorBoxTreeIterator : public TreeIterator<SensorBox, INT_ID, LessThan<SensorBox> >
//{
//public:
//  SensorBoxTreeIterator(const SensorBoxTree<INT_ID> &tree, int set_first = 1):TreeIterator<SensorBox, INT_ID, LessThan<SensorBox> >(tree, set_first) {}
//  inline SensorBoxTreeNode<INT_ID>* Next(void) { return (SensorBoxTreeNode<INT_ID>* ) TreeIterator<SensorBox, INT_ID, LessThan<SensorBox> >::Next(); }
//};
//
//template <class INT_ID>
//SensorBoxTreeNode<INT_ID>* SensorBoxTree<INT_ID>::FindByHandle(const ApHandle& hSensor)
//{
//  SensorBoxTreeNode<INT_ID>* pResult = 0;
//
//  SensorBoxTreeNode<INT_ID>* pNode = 0;
//  for (SensorBoxTreeIterator<INT_ID> iter(*this); pNode = iter.Next(); ) {
//    if (pNode->Key().hAp_ == hSensor) {
//      pResult = pNode;
//      break;
//    }
//  }
//
//  return pResult;
//}
//
//typedef SensorBoxTree<Element*> SensorList;
//typedef SensorBoxTreeNode<Element*> SensorListNode;
//typedef SensorBoxTreeIterator<Element*> SensorListIterator;

//----------------------------------------------------------

class RootElement: public Element
{
public:
  RootElement()
    :Element(0)
  {}
  virtual ~RootElement() {}

  void SetSurface(Surface* pSurface) { pSurface_ = pSurface; }
};

//----------------------------------------------------------

class Surface
{
public:
  Surface(const ApHandle& hSurface)
    :hAp_(hSurface)
    ,bVisible_(0)
    ,nX_(0)
    ,nY_(0)
    ,nW_(0)
    ,nH_(0)
    ,bAutoDraw_(false)
    ,bAutoDrawAsync_(false)
    ,autoDrawInterval_(0, 100000)
    ,pCairoSurface_(0)
    ,pCairo_(0)
    #if defined(WIN32)
    ,hInstance_(NULL)
    ,hWnd_(NULL)
    ,hBitmap_(NULL)
    ,pBits_(0)
    ,dcMemory_(NULL)
    ,hOldBitmap_(NULL)
    #endif // WIN32
  {
    root_.SetSurface(this);
  }
  virtual ~Surface();

  inline ApHandle apHandle() { return hAp_; }

  int Create();
  void Destroy();
  void SetPosition(int nX, int nY, int nW, int nH);
  void SetVisibility(int bVisible);

  void DeleteAutoDraw();
  void SetAutoDraw(int nMilliSec, int bAsync);
  void AutoDraw();

  Element* FindElement(const String& sPath);
  void CreateElement(const String& sPath);
  void DeleteElement(const String& sPath);

  void MeasureText(const String& sText, const String& sFont, double fSize, int nFlags, TextExtents& te);
  void GetImageSizeFromData(const Apollo::Image& image, double& fW, double& fH);
  void GetImageSizeFromFile(const String& sFile, double& fW, double& fH);

  //void AddSensor(const ApHandle& hSensor);
  //void RemoveSensor(const ApHandle& hSensor);

  void Draw();

#if defined(WIN32)
  static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT HandleMouseEvent(int nEvent, int nButton, LPARAM lParam);
#endif // WIN32

protected:
  int CreateBitmap();
  void DestroyBitmap();
  void EraseBackground();

protected:
  ApHandle hAp_;
  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;

  bool bAutoDraw_;
  bool bAutoDrawAsync_;
  Apollo::TimeValue autoDrawInterval_;
  Apollo::TimeValue lastDraw_;

  RootElement root_;
  //SensorList sensors_;

  cairo_surface_t *pCairoSurface_;
  cairo_t *pCairo_;
#if defined(WIN32)
  static int nCntWindows_;
  HINSTANCE hInstance_;
  HWND hWnd_;
  HBITMAP hBitmap_;
  unsigned char* pBits_;
  HDC dcMemory_;
  HBITMAP hOldBitmap_;
#endif // WIN32

#if defined(AP_TEST)
  friend class SceneModuleTester;
#endif
};

#endif // Surface_H_INCLUDED
