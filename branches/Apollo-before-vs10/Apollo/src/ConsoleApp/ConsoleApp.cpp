// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Main/Main.h"

//----------------------------------------------------------

int main(int argc, char* argv[])
{
  Apollo::Init(argc, argv);

  { Msg_Log_Line msg; msg.Hook("ConsoleApp", (ApCallback) On_Log_Line, 0, ApCallbackPosNormal); }

  apLog_SetMask(apLog_MaskMaxInfo);
  apLog_Info(("main", "ConsoleApp", ""));

  { Msg_MainLoop_ConsoleInput msg; msg.Hook("ConsoleApp", (ApCallback) On_MainLoop_ConsoleInput, 0, ApCallbackPosNormal); }

  Apollo::loadModule("Config", "config.dll");
  Apollo::loadModulesFromConfig("Core/Modules");

  { Msg_System_RunMode msg; msg.nMode = Msg_System_RunMode::RunModeConsole; msg.Send(); }
  { Msg_System_AfterLoadModules msg; msg.Send(); }

  { Msg_Config_Clear msg; msg.Request(); }
  {
    Msg_Config_Load msg;
    msg.sDir = Apollo::getAppResourcePath();
    msg.sName = "config";
    int ok = msg.Request();
    if (!ok) {
      TCHAR pBuf[MAX_PATH+1];
      String sCwd = ::_tgetcwd(pBuf, MAX_PATH);
      apLog_Error(("main", "ConsoleApp", "Config file '/%s' not found. Maybe wrong working directory: %s", StringType(msg.sDir), StringType(msg.sName), StringType(sCwd)));
    }
  }

  { 
    Msg_MainLoop_Loop msg;
    Apollo::callMsg(&msg, Apollo::CF_UNSYNCHRONIZED); // no sync, because other threads may start inside the main loop
  }

  { Msg_System_BeforeUnloadModules msg; msg.Send(); }
  Apollo::unloadModules();

  { Msg_MainLoop_ConsoleInput msg; msg.UnHook("ConsoleApp", (ApCallback) On_MainLoop_ConsoleInput, 0); }
  { Msg_Log_Line msg; msg.UnHook("ConsoleApp", (ApCallback) On_Log_Line, 0); }

  Apollo::Exit();

#if defined(WIN32)
  getchar();
#endif
  return 0;
}
