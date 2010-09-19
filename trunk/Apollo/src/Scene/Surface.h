// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Surface_H_INCLUDED)
#define Surface_H_INCLUDED

#include "Element.h"

typedef StringPointerTree<Element*> ElementList;
typedef StringPointerTreeNode<Element*> ElementNode;
typedef StringPointerTreeIterator<Element*> ElementIterator;

class Surface
{
public:
  Surface(ApHandle hSurface);
  virtual ~Surface();

  int Create();
  void Destroy();
  void SetPosition(int nX, int nY, int nW, int nH);
  void SetVisibility(int bVisible);
  void SetRectangle(
    const String& sPath,
    int bFill, double fFillRed, double fFillGreen, double fFillBlue, double fFillAlpha,
    int bStroke, double fStrokeWidth, double fStrokeRed, double fStrokeGreen, double fStrokeBlue, double fStrokeAlpha,
    double fX, double fY, double fW, double fH
  );
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

  ElementList graph_;

#if defined(WIN32)
  static int nCntWindows_;
  HINSTANCE hInstance_;
  HWND hWnd_;
  HBITMAP hBitmap_;
  unsigned char* pBits_;
#endif // WIN32
};

#endif // Surface_H_INCLUDED
