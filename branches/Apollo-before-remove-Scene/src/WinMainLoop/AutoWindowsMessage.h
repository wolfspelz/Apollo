// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(AutoWindowsMessage_h_INCLUDED)
#define AutoWindowsMessage_h_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

class AutoWindowsMessage
{
public:
  AutoWindowsMessage(HWND _wnd, int _msg);
  ~AutoWindowsMessage();

  // this one will do the work
  virtual void Handle();

  int Send();
  void Post();
  void Release();

  HWND wnd_;
  UINT msg_;
  int delete_;
};

#endif // !defined(AutoWindowsMessage_h_INCLUDED)
