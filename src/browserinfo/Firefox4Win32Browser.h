// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Firefox4Win32Browser_h_INCLUDED)
#define Firefox4Win32Browser_h_INCLUDED

#include "Win32Browser.h"

class Firefox4Win32Browser : public Win32Browser
{
public:
  Firefox4Win32Browser(Apollo::WindowHandle& win) : Win32Browser(win) {}

  void OnTimer();
};

#endif // !defined(Firefox4Win32Browser_h_INCLUDED)
