// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Scene_H_INCLUDED)
#define Scene_H_INCLUDED

#include "Element.h"
#include "Primitives.h"

class RootElement: public Element
{
public:
  RootElement()
    :Element(0)
  {}
  virtual ~RootElement() {}

  void SetScene(Scene* pScene) { pScene_ = pScene; }
};

//----------------------------------------------------------

class Scene
{
public:
  Scene(const ApHandle& hScene)
    :hAp_(hScene)
    ,bLogDraw_(0)
    ,bVisible_(0)
    ,nX_(0)
    ,nY_(0)
    ,nW_(0)
    ,nH_(0)
    ,bAutoDraw_(false)
    ,bAutoDrawAsync_(false)
    ,autoDrawInterval_(0, 100000)
    ,bTimerRunning_(0)
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
    root_.SetScene(this);
  }
  virtual ~Scene();

  inline ApHandle apHandle() { return hAp_; }
  inline int LogDraw() { return bLogDraw_; }

  int Create();
  void Destroy();
  void SetPosition(int nX, int nY, int nW, int nH);
  void SetVisibility(int bVisible);

  void DeleteAutoDraw();
  void SetAutoDraw(int nMilliSec, int bAsync);
  void AutoDraw();

  int HasElement(const String& sPath);
  Element* GetElement(const String& sPath);
  Element* CreateElement(const String& sPath);
  void DeleteElement(const String& sPath);

  void AddElement(const String& sPath, Element* pElement);
  RectangleElement* CreateRectangle(const String& sPath);
  ImageElement* CreateImage(const String& sPath);
  TextElement* CreateText(const String& sPath);
  SensorElement* CreateSensor(const String& sPath);

  void GetTextExtents(const String& sText, const String& sFont, double fSize, int nFlags, TextExtents& te);
  void GetImageSizeFromData(const Apollo::Image& image, double& fW, double& fH);
  void GetImageSizeFromFile(const String& sFile, double& fW, double& fH);

  void CaptureMouse(const String& sPath);
  void ReleaseMouse();

  void Draw();
  void OnAutoDrawTimer();

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
  int bLogDraw_;
  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;

  bool bAutoDraw_;
  bool bAutoDrawAsync_;
  Apollo::TimeValue autoDrawInterval_;
  Apollo::TimeValue lastDraw_;
  int bTimerRunning_;

  RootElement root_;

  cairo_surface_t *pCairoSurface_;
  cairo_t *pCairo_;
  String sCaptureMouseElement_;

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

#endif // Scene_H_INCLUDED
