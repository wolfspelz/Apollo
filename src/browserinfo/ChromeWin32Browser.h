// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(ChromeWin32Browser_h_INCLUDED)
#define ChromeWin32Browser_h_INCLUDED

#include "Win32Browser.h"

class ChromeWin32Browser : public Win32Browser
{
public:
  ChromeWin32Browser(Apollo::WindowHandle& win) : Win32Browser(win) {}

  void OnTimer();
};

#endif // !defined(ChromeWin32Browser_h_INCLUDED)
