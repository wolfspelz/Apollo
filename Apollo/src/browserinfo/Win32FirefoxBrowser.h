// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Win32FirefoxBrowser_h_INCLUDED)
#define Win32FirefoxBrowser_h_INCLUDED

#include "Browser.h"

class Win32FirefoxBrowser : public Browser
{
public:
  Win32FirefoxBrowser(Apollo::WindowHandle& win)
    :Browser(win)
  {}

  void OnTimer();

protected:
  void AdjustStackingOrder();
  void PutWindowBeforeBrowser(HWND hWnd);
};

#endif // !defined(Win32FirefoxBrowser_h_INCLUDED)
