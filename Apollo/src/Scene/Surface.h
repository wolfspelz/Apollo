// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Surface_H_INCLUDED)
#define Surface_H_INCLUDED

#include "Element.h"

class Surface
{
public:
  Surface(ApHandle hSurface)
    :hAp_(hSurface)
    ,bVisible_(0)
    ,nX_(0)
    ,nY_(0)
    ,nW_(0)
    ,nH_(0)
    ,pSurface_(0)
    ,pCairo_(0)
    #if defined(WIN32)
    ,hInstance_(NULL)
    ,hWnd_(NULL)
    ,hBitmap_(NULL)
    ,pBits_(0)
    ,dcMemory_(NULL)
    ,hOldBitmap_(NULL)
    #endif // WIN32
  {}
  virtual ~Surface();

  int Create();
  void Destroy();
  void SetPosition(int nX, int nY, int nW, int nH);
  void SetVisibility(int bVisible);
  
  Element* FindElement(const String& sPath);
  void CreateElement(const String& sPath);
  void DeleteElement(const String& sPath);

  void MeasureText(const String& sText, const String& sFont, double fSize, int nFlags, TextExtents& te);

  void Draw();

#if defined(WIN32)
  static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
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

  Element root_;

  cairo_surface_t *pSurface_;
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
