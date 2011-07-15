// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(WindowHandle_h_INCLUDED)
#define WindowHandle_h_INCLUDED

#include "Apollo.h"
#include "ApExports.h"

AP_NAMESPACE_BEGIN

class APOLLO_API WindowHandle
{
#if defined(WIN32)
public:
  WindowHandle() : hWnd_(NULL) {}
  WindowHandle(const WindowHandle& h) : hWnd_(h.hWnd_) {}
  WindowHandle(HWND hWnd) : hWnd_(hWnd) {}
  inline void operator =(HWND hWnd) { hWnd_ = hWnd; }
  inline operator HWND() { return hWnd_; }
  inline bool isValid() { return hWnd_ != NULL; }
protected:
  HWND hWnd_;
 #else
  #error Not implemented for this OS
#endif

public:
  // For LessThan<T>::operator () (const T &lhs, const T &rhs) const
  int operator<(const WindowHandle& s) const;
};

AP_NAMESPACE_END

#endif // !defined(WindowHandle_h_INCLUDED)
