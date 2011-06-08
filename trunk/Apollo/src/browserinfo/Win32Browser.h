// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(Win32Browser_h_INCLUDED)
#define Win32Browser_h_INCLUDED

#include "Browser.h"

class Win32Browser : public Browser
{
public:
  Win32Browser(Apollo::WindowHandle& win) : Browser(win) {}

protected:
  virtual void AdjustStackingOrder();
};

#endif // !defined(Win32Browser_h_INCLUDED)
