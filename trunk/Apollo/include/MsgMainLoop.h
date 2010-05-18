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
  ApIN LPSTR lpCmdLine;
  ApIN int nCmdShow;
  ApOUT int wParam;
};

class Msg_Win32_GetInstance: public ApRequestMessage
{
public:
  Msg_Win32_GetInstance() : ApRequestMessage("Win32_GetInstance"), hInstance(NULL) {}
  ApOUT HINSTANCE hInstance;
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

// main -> mainloop
class Msg_MainLoop_EndLoop: public ApRequestMessage
{
public:
  Msg_MainLoop_EndLoop() : ApRequestMessage("MainLoop_EndLoop") {}
};

// mainloop ->
class Msg_MainLoop_EventLoopBegin: public ApNotificationMessage
{
public:
  Msg_MainLoop_EventLoopBegin() : ApNotificationMessage("MainLoop_EventLoopBegin") {}
};

// mainloop ->
class Msg_MainLoop_EventLoopBeforeEnd: public ApFilterMessage
{
public:
  Msg_MainLoop_EventLoopBeforeEnd() : ApFilterMessage("MainLoop_EventLoopBeforeEnd"), nDelaySec(0), nWaitCount(0) {}
  ApINOUT int nDelaySec;
  ApINOUT int nWaitCount;
};

// -> mainloop
class Msg_MainLoop_ModuleFinished: public ApNotificationMessage
{
public:
  Msg_MainLoop_ModuleFinished() : ApNotificationMessage("MainLoop_ModuleFinished") {}
};

// mainloop ->
class Msg_MainLoop_EventLoopEnd: public ApNotificationMessage
{
public:
  Msg_MainLoop_EventLoopEnd() : ApNotificationMessage("MainLoop_EventLoopEnd") {}
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
