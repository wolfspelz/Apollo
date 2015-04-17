// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(MsgMainLoop_h_INCLUDED)
#define MsgMainLoop_h_INCLUDED

#include "ApMessage.h"

// main -> mainloop
#if defined(WIN32) && !defined(_CONSOLE)

class Msg_MainLoop_Win32Loop: public ApRequestMessage
{
public:
  Msg_MainLoop_Win32Loop() : ApRequestMessage("MainLoop_Win32Loop"), hInstance(NULL), hPrevInstance(NULL), lpCmdLine(0), nCmdShow(0), wParam(0) {}
  ApIN HINSTANCE hInstance;
  ApIN HINSTANCE hPrevInstance;
  ApIN LPWSTR lpCmdLine;
  ApIN int nCmdShow;
  ApOUT int wParam;
};

class Msg_Win32_GetInstance: public ApRequestMessage
{
public:
  Msg_Win32_GetInstance() : ApRequestMessage("Win32_GetInstance"), hInstance(NULL) {}
  static HINSTANCE _()
  {
    HINSTANCE hInstance = NULL;
    Msg_Win32_GetInstance msg;
    int ok = msg.Request();
    if (ok) {
      hInstance = msg.hInstance;
    }
    return hInstance;
  }
  ApOUT HINSTANCE hInstance;
};

class Msg_Win32_GetMainWindow: public ApRequestMessage
{
public:
  Msg_Win32_GetMainWindow() : ApRequestMessage("Win32_GetMainWindow"), hWnd(NULL) {}
  static HWND _()
  {
    HWND hWnd = NULL;
    Msg_Win32_GetMainWindow msg;
    int ok = msg.Request();
    if (ok) {
      hWnd = msg.hWnd;
    }
    return hWnd;
  }
  ApOUT HWND hWnd;
};

class Msg_Win32_WndProcMessage: public ApRequestMessage
{
public:
  Msg_Win32_WndProcMessage() : ApRequestMessage("Win32_WndProcMessage"), hWnd(NULL), message(0), wParam(0), lParam(0), lResult(0) {}
  ApIN HWND hWnd;
  ApIN UINT message;
  ApIN WPARAM wParam;
  ApIN LPARAM lParam;
  ApOUT LRESULT lResult;
};

#elif defined(LINUX) || defined(MAC) || defined(_CONSOLE)

class Msg_MainLoop_Loop: public ApRequestMessage
{
public:
  Msg_MainLoop_Loop() : ApRequestMessage("MainLoop_Loop"), argc(0), argv(NULL), nReturnCode(0) {}
  ApIN int argc;
  ApIN char** argv;
  ApOUT int nReturnCode;
};

#else

  #error MainLoop message not implemented for this OS

#endif

// mainloop ->
class Msg_MainLoop_EventLoopBegin: public ApNotificationMessage
{
public:
  Msg_MainLoop_EventLoopBegin() : ApNotificationMessage("MainLoop_EventLoopBegin") {}
};

// main -> mainloop
class Msg_MainLoop_EndLoop: public ApRequestMessage
{
public:
  Msg_MainLoop_EndLoop() : ApRequestMessage("MainLoop_EndLoop") {}
};

// mainloop ->
class Msg_MainLoop_EventLoopDelayEnd: public ApFilterMessage
{
public:
  Msg_MainLoop_EventLoopDelayEnd() : ApFilterMessage("MainLoop_EventLoopDelayEnd"), nDelaySec(0), nWaitCount(0) {}
  ApINOUT int nDelaySec;
  ApINOUT int nWaitCount;
};

// mainloop ->
class Msg_MainLoop_EventLoopEnd: public ApNotificationMessage
{
public:
  Msg_MainLoop_EventLoopEnd() : ApNotificationMessage("MainLoop_EventLoopEnd") {}
};

// -> mainloop
class Msg_MainLoop_ModuleFinished: public ApNotificationMessage
{
public:
  Msg_MainLoop_ModuleFinished() : ApNotificationMessage("MainLoop_ModuleFinished") {}
};

// mainloop ->
class Msg_MainLoop_ConsoleInput: public ApNotificationMessage
{
public:
  Msg_MainLoop_ConsoleInput() : ApNotificationMessage("MainLoop_ConsoleInput") {}
  ApIN String sCmdLine;
};

// mainloop ->
class Msg_MainLoop_EventLoopIdle: public ApNotificationMessage
{
public:
  Msg_MainLoop_EventLoopIdle() : ApNotificationMessage("MainLoop_EventLoopIdle") {}
};

#endif // !defined(MsgMainLoop_h_INCLUDED)
