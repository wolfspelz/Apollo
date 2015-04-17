// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Firefox3Win32Browser_h_INCLUDED)
#define Firefox3Win32Browser_h_INCLUDED

#include "Win32Browser.h"

class Firefox3Win32Browser : public Win32Browser
{
public:
  Firefox3Win32Browser(Apollo::WindowHandle& win) : Win32Browser(win) {}

  void OnTimer();
};

#endif // !defined(Firefox3Win32Browser_h_INCLUDED)
