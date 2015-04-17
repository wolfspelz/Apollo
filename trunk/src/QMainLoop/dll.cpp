// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "dll.h"
#include "Local.h"
#include "MainLoopModule.h"

#if defined(WIN32)
static HINSTANCE g_hDllInstance = NULL;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  AP_MEMORY_CHECK(dwReason);
  g_hDllInstance = (HINSTANCE) hModule;
  AP_UNUSED_ARG(dwReason);
  AP_UNUSED_ARG(lpReserved);
  return TRUE;
}
#endif // defined(WIN32)

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "QMainLoop",
  "QT based MainLoop Module",
  "1",
  "The module implements the main application loop.",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

QMAINLOOP_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

typedef ApModuleSingleton<MainLoopModule> MainLoopModuleInstance;

AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_Win32Loop)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_EndLoop)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_ModuleFinished)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Win32_GetInstance)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Win32_GetMainWindow)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, Win32_WndProcMessage)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, System_ThreadMessage)

//----------------------------------------------------------


QMAINLOOP_API int Load(AP_MODULE_CALL* pModuleData)
{
  int ok = 0;

  AP_UNUSED_ARG(pModuleData);
  MainLoopModuleInstance::Delete();

  { Msg_MainLoop_Win32Loop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_Win32Loop), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_MainLoop_EndLoop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_EndLoop), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_MainLoop_ModuleFinished msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_ModuleFinished), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Win32_GetInstance msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetInstance), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Win32_GetMainWindow msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetMainWindow), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_Win32_WndProcMessage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_WndProcMessage), MainLoopModuleInstance::Get(), ApCallbackPosLate); }
  { Msg_System_ThreadMessage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, System_ThreadMessage), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }

  MainLoopModule* pModule = MainLoopModuleInstance::Get();
  if (pModule != 0) {
    ok = pModule->Init();
  }

  return ok;
}

QMAINLOOP_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { Msg_MainLoop_Win32Loop msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_Win32Loop), MainLoopModuleInstance::Get()); }
  { Msg_MainLoop_EndLoop msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_EndLoop), MainLoopModuleInstance::Get()); }
  { Msg_MainLoop_ModuleFinished msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_ModuleFinished), MainLoopModuleInstance::Get()); }
  { Msg_Win32_GetInstance msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetInstance), MainLoopModuleInstance::Get()); }
  { Msg_Win32_GetMainWindow msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_GetMainWindow), MainLoopModuleInstance::Get()); }
  { Msg_Win32_WndProcMessage msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, Win32_WndProcMessage), MainLoopModuleInstance::Get()); }
  { Msg_System_ThreadMessage msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, System_ThreadMessage), MainLoopModuleInstance::Get()); }

  MainLoopModule* pModule = MainLoopModuleInstance::Get();
  if (pModule != 0) {
    pModule->Exit();
  }

  MainLoopModuleInstance::Delete();

  return 1;
}
