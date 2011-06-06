// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Win32FirefoxBrowser_h_INCLUDED)
#define Win32FirefoxBrowser_h_INCLUDED

#include "Browser.h"

class Win32Browser : public Browser
{
public:
  Win32Browser(Apollo::WindowHandle& win) : Browser(win) {}

protected:
  virtual void AdjustStackingOrder();
};

// ---------------------------------------------------------

class Win32FirefoxBrowser : public Win32Browser
{
public:
  Win32FirefoxBrowser(Apollo::WindowHandle& win) : Win32Browser(win) {}

  void OnTimer();
};

#endif // !defined(Win32FirefoxBrowser_h_INCLUDED)
