// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgMainLoop.h"
#include "ConsoleLoop.h"
#include "STree.h"

static HINSTANCE g_hDllInstance = NULL;

#if defined(WIN32)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  AP_MEMORY_CHECK(dwReason);
  AP_UNUSED_ARG(hModule);
  AP_UNUSED_ARG(dwReason);
  AP_UNUSED_ARG(lpReserved);
  g_hDllInstance= (HINSTANCE) hModule;
  return TRUE;
}
#endif

#define LOG_CHANNEL "ConsoleLoop"
#define MODULE_NAME "ConsoleLoop"

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "MainLoop",
  "Console MainLoop Module",
  "1",
  "The module implements the main application loop.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

CONSOLELOOP_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

class ConsoleLoopModule
{
public:
  ConsoleLoopModule()
    :bEndLoop_(0)
  {}

  //void On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg);
  void On_MainLoop_Loop(Msg_MainLoop_Loop* pMsg);
  void On_MainLoop_EndLoop(Msg_MainLoop_EndLoop* pMsg);

protected:
  int bEndLoop_;
  List lTimer_;
};

//----------------------------------------------------------

typedef ApModuleSingleton<ConsoleLoopModule> ConsoleLoopModuleInstance;

/*
void ConsoleLoopModule::On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg)
{
  int ok = 0;

  ConsoleLoopModule* self = (ConsoleLoopModule*) pMsg->Ref();
  if (self != 0) {
    ok = (TRUE == ::PostMessage(self->hWnd_, self->nWM_APMSG_, (LPARAM) pMsg->pMsg, 0));
  }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
  pMsg->Stop();
}
*/
//----------------------------------------------------------

void ConsoleLoopModule::On_MainLoop_Loop(Msg_MainLoop_Loop* pMsg)
{
  int ok = 1;

  { Msg_MainLoop_EventLoopBegin msg; msg.Send(); }

  while (!bEndLoop_) {
    String sLine;
    int c = 0;

    /*
    while ((c = getchar()) != EOF) {
      if (c == '\n' || c == '\r' ) {
        break;
      } else {
        sLine += (char) c;
      }
    }

    if (sLine != "") {
      Msg_MainLoop_ConsoleInput msg;
      msg.sCmdLine = sLine;
      msg.Send();
    }
    */

    /*
    while (!bEndLoop_) {
      int nMilliSec = 1000;
      TimerElem* e = lTimer_.First();
      if (e != 0) {
        lTimer_.Remove(e);
        //nMilliSec = e->
      }
      Sleep(nMilliSec);
    }
    */

    /*
    SOCKET nSocket = ::socket(PF_INET, SOCK_DGRAM, 0);
    String buf = "x";
    ::send(nSocket, buf.c_str(), 1, 0);
    while (!bEndLoop_) {
      fd_set readfds; FD_ZERO(&readfds);
      fd_set writefds; FD_ZERO(&writefds);
      fd_set exceptfds; FD_ZERO(&exceptfds);
      FD_SET(nSocket, &readfds);
      timeval timeout;
      timeout.tv_sec = 5;
      timeout.tv_usec = 0;
      int nResult = select(1, &readfds, &writefds, &exceptfds, &timeout);
    }
    */
  }

  { Msg_MainLoop_EventLoopEnd msg; msg.Send(); }

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

void ConsoleLoopModule::On_MainLoop_EndLoop(Msg_MainLoop_EndLoop* pMsg)
{
  int ok = 1;

  bEndLoop_ = 1;

  Msg_MainLoop_EventLoopBeforeEnd msg;
  msg.Filter();

  pMsg->apStatus = ok ? ApMessage::Ok : ApMessage::Error;
}

//----------------------------------------------------------

//AP_REFINSTANCE_MSG_HANDLER(ConsoleLoopModule, System_ThreadMessage)
AP_REFINSTANCE_MSG_HANDLER(ConsoleLoopModule, MainLoop_Loop)
AP_REFINSTANCE_MSG_HANDLER(ConsoleLoopModule, MainLoop_EndLoop)

//----------------------------------------------------------

CONSOLELOOP_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  ConsoleLoopModuleInstance::Delete();

  { Msg_MainLoop_Loop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ConsoleLoopModule, MainLoop_Loop), ConsoleLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_MainLoop_EndLoop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ConsoleLoopModule, MainLoop_EndLoop), ConsoleLoopModuleInstance::Get(), ApCallbackPosNormal); }
  //{ Msg_System_ThreadMessage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ConsoleLoopModule, System_ThreadMessage), ConsoleLoopModuleInstance::Get(), ApCallbackPosNormal); }

  return ConsoleLoopModuleInstance::Get() != 0;
}

CONSOLELOOP_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  //{ Msg_System_ThreadMessage msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ConsoleLoopModule, System_ThreadMessage), ConsoleLoopModuleInstance::Get()); }
  { Msg_MainLoop_Loop msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ConsoleLoopModule, MainLoop_Loop), ConsoleLoopModuleInstance::Get()); }
  { Msg_MainLoop_EndLoop msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(ConsoleLoopModule, MainLoop_EndLoop), ConsoleLoopModuleInstance::Get()); }

  ConsoleLoopModuleInstance::Delete();

  return 1;
}
