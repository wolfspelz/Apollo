// ============================================================================
//
// Apollo
//
// ============================================================================

#include "AutoWindowsMessage.h"

AutoWindowsMessage::AutoWindowsMessage(HWND _wnd, int _msg)
:wnd_(_wnd)
,msg_(_msg)
,delete_(0)
{
}

AutoWindowsMessage::~AutoWindowsMessage()
{
}

int AutoWindowsMessage::Send()
{
  int result = 0;
  if (wnd_ != NULL) {
    delete_= 0;
    result = ::SendMessage(wnd_, msg_, (LPARAM) this, 0);
    delete_= 1;
    Release();
  }
  return result;
}

void AutoWindowsMessage::Post()
{
  if (wnd_ != NULL) {
    delete_= 1;
    ::PostMessage(wnd_, msg_, (LPARAM) this, 0);
  }
}

void AutoWindowsMessage::Handle()
{
  __asm { int 3 }
}

void AutoWindowsMessage::Release()
{
  if (delete_) {
    delete this;
  }
}
