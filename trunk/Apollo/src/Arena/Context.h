// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Context_H_INCLUDED)
#define Context_H_INCLUDED

class Context
{
public:
  Context(ApHandle hContext);
  virtual ~Context();

  void SetVisibility(int bVisible);
  void SetPosition(int nX, int nY);
  void SetSize(int nW, int nH);

  static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  int CreateSurface();
protected:
  void DestroySurface();
  void ShowSurface();
  void MoveSurface();
  void SizeSurface();
  void DrawSurface();

protected:
  ApHandle hAp_;

  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;

  static int nCntWindows_;
  HINSTANCE hInstance_;
  HWND hWnd_;
  HBITMAP hBitmap_;
  unsigned char* pBits_;
};

#endif // Context_H_INCLUDED
