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
  Surface(ApHandle hSurface);
  virtual ~Surface();

  int Create();
  void Destroy();
  void SetPosition(int nX, int nY, int nW, int nH);
  void SetVisibility(int bVisible);
  
  Element* FindElement(const String& sPath);
  void CreateElement(const String& sPath);
  void DeleteElement(const String& sPath);

  void SetRectangle(const String& sPath, double fX, double fY, double fW, double fH);
  void SetFillColor(const String& sPath, double fRed, double fGreen, double fBlue, double fAlpha);
  void SetStrokeColor(const String& sPath, double fRed, double fGreen, double fBlue, double fAlpha);
  void SetStrokeWidth(const String& sPath, double fWidth);

  void Draw();

#if defined(WIN32)
  static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif // WIN32

protected:
  ApHandle hAp_;
  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;

  Element root_;

#if defined(WIN32)
  static int nCntWindows_;
  HINSTANCE hInstance_;
  HWND hWnd_;
  HBITMAP hBitmap_;
  unsigned char* pBits_;
#endif // WIN32
};

#endif // Surface_H_INCLUDED
