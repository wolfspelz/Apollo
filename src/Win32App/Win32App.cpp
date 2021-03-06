// Win32App.cpp : Defines the entry point for the application.
//


#include "Main/Main.h"
#include "MsgOS.h"
#include <stdlib.h>
#include <shellapi.h>

//----------------------------------------------------------

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
  AP_MEMORY_CHECK(DLL_PROCESS_ATTACH);

#if defined(_DEBUG) && defined(_MSC_VER)
  ::OutputDebugString(_T("############################## Startup #######################################\r\n"));
#endif

  //apLog_SetMask(apLog_MaskSilent);
  //apLog_SetMask(apLog_MaskNoTrace);
  //apLog_SetMask(apLog_MaskMaxInfo);
#if defined(_DEBUG)
  //apLog_SetMask(apLog_MaskMaxVerbose);
#else
  apLog_SetMask(apLog_MaskMaxUser);
#endif

  String sCmdline = GetCommandLine();
  Apollo::ValueList vlArgs;
  Apollo::splitCommandlineArguments(sCmdline, vlArgs);

  if (!Apollo::Init(vlArgs)) {
    apLog_Alert(("Main", "WinMain", "Apollo::Init() failed"));
  }

  apLog_Info(("Main", "WinMain", ""));

  { Msg_Log_Line msg; msg.Hook("Win32App", (ApCallback) On_Log_Line, 0, ApCallbackPosNormal); }
  { Msg_Log_SetMask msg; msg.Hook("Win32App", (ApCallback) On_Log_SetMask, 0, ApCallbackPosNormal); }
  
  { Msg_MainLoop_ConsoleInput msg; msg.Hook("Win32App", (ApCallback) On_MainLoop_ConsoleInput, 0, ApCallbackPosNormal); }

  { Msg_System_RunLevel msg; msg.sLevel = Msg_System_RunLevel_PreBoot; msg.Send(); }

  apLog_User(("Loading"));

  {
    String sModule = "config";
    int ok = Apollo::loadModule("Config", sModule);
    if (!ok) {
      apLog_Alert(("Main", "WinMain", "Could not load module %s", _sz(sModule)));
    }
  }

  //  Load config
  { Msg_System_RunLevel msg; msg.sLevel = Msg_System_RunLevel_Boot; msg.Send(); }
  { Msg_Config_Clear msg; msg.Request(); }
  {
    Msg_Config_Load msg;
    msg.sDir = Apollo::getAppResourcePath();
    msg.sName = "boot";
    int ok = msg.Request();
    if (!ok) {
      apLog_Alert(("Main", "WinMain", "%s/%s not found. Maybe wrong working directory?", _sz(msg.sDir), _sz(msg.sName)));
    }
  }

#if defined(_DEBUG)
  if (Apollo::getConfig("Debug/ShowConsole", 1)) {
    if (::AllocConsole()) {
      ::freopen("conout$", "w", stdout);
      ::freopen("conout$", "w", stderr);
    }
    ::SetConsoleOutputCP(CP_UTF8);
  }
#endif

  int nLogMask = Apollo::getConfig("LogMask", -1);
  if (nLogMask != -1) { Msg_Log_SetMask msg; msg.nMask = nLogMask; msg.Send(); }

  //Apollo::loadModulesFromDir("./");
  Apollo::loadModulesFromConfig("Core/Modules/");

  {
    Msg_Config_Load msg;
    msg.sDir = Apollo::getAppResourcePath();
    msg.sName = "config";
    int ok = msg.Request();
    if (!ok) {
      apLog_Alert(("Main", "WinMain", "%s/%s not found. Maybe wrong working directory?", _sz(msg.sDir), _sz(msg.sName)));
    }
  }

  { Msg_System_RunMode msg; msg.nMode = Msg_System_RunMode::RunModeGUI; msg.Send(); }
  { Msg_System_AfterLoadModules msg; msg.Send(); }
  { Msg_System_BeforeEventLoop msg; msg.Send(); }

  apLog_User(("Running"));

  int nResult = 0;
  {
    Msg_MainLoop_Win32Loop msg;
    msg.hInstance = hInstance;
    msg.hPrevInstance = hPrevInstance;
    msg.lpCmdLine = lpCmdLine;
    msg.nCmdShow = nCmdShow;
    Apollo::callMsg(&msg, Apollo::CF_UNSYNCHRONIZED); // no sync, because other threads may start inside the main loop
    nResult = msg.wParam;
  }

  apLog_User(("Shutdown"));

  { Msg_System_AfterEventLoop msg; msg.Send(); }
  { Msg_System_BeforeUnloadModules msg; msg.Send(); }

  Apollo::unloadModules();

  { Msg_MainLoop_ConsoleInput msg; msg.Unhook("Win32App", (ApCallback) On_MainLoop_ConsoleInput, 0); }
  { Msg_Log_SetMask msg; msg.Unhook("Win32App", (ApCallback) On_Log_SetMask, 0); }
  { Msg_Log_Line msg; msg.Unhook("Win32App", (ApCallback) On_Log_Line, 0); }

  Apollo::Exit();

#if defined(_DEBUG) && defined(_MSC_VER)
  ::OutputDebugString(_T("############################## Shutdown #######################################\r\n"));
#endif

#if defined(_DEBUG)
#if defined(_WIN32) && defined(_MSC_VER)
  _CrtCheckMemory();
#endif
#endif

  return nResult;
}
