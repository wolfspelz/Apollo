// ============================================================================
//
// Apollo
//
// ============================================================================

#include "WindowHandle.h"

int Apollo::WindowHandle::operator<(const Apollo::WindowHandle& w) const
{
#if defined(WIN32)
  return hWnd_ < w.hWnd_ ? 1 : 0;
 #else
  #error Not implemented for this OS
#endif
}
