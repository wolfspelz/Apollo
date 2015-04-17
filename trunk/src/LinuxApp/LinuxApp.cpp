// Win32App.cpp : Defines the entry point for the application.
//

#include "Main/Main.h"
#include "MsgOs.h"

//----------------------------------------------------------

int main(int argc, char** argv)
{
  apLog_SetMask(apLog_MaskMaxVerbose); // apLog_MaskMaxVerbose
  Apollo::Init(argc, argv);
  {
    SString sOsModule; if (argc) { sOsModule += SString::FilenameBasePath(argv[0]); }
    sOsModule +="os"; // Filename is being normalized anyway, so we can use path + name core as fullpath
    int ok = Apollo::LoadModule((const char*)sOsModule, Apollo::MODNAME_FULLPATH); // Critical: the first module loaded
    if (!ok) {
      apLog_Alert(("Main", "main", "Could not load module %s", (const char*) sOsModule));
    }
  }

  apLog_Info(("Main", "main", ""));

  { Msg_Log_DoLog msg; msg.Hook("LinuxApp", (ApCallback) On_Log_DoLog, 0, ApCallbackPosNormal); }
  { Msg_Log_SetMask msg; msg.Hook("LinuxApp", (ApCallback) On_Log_SetMask, 0, ApCallbackPosNormal); }

  { Msg_System_ModuleLoaded msg; msg.Hook("LinuxApp", (ApCallback) On_System_ModuleLoaded, 0, ApCallbackPosNormal); }
  { Msg_System_ModuleUnLoaded msg; msg.Hook("LinuxApp", (ApCallback) On_System_ModuleUnLoaded, 0, ApCallbackPosNormal); }
  { Msg_MainLoop_EventLoopBegin msg; msg.Hook("LinuxApp", (ApCallback) On_MainLoop_EventLoopBegin, 0, ApCallbackPosNormal); }
  { Msg_MainLoop_EventLoopEnd msg; msg.Hook("LinuxApp", (ApCallback) On_MainLoop_EventLoopEnd, 0, ApCallbackPosNormal); }
  { Msg_MainLoop_ConsoleInput msg; msg.Hook("LinuxApp", (ApCallback) On_MainLoop_ConsoleInput, 0, ApCallbackPosNormal); }

  Apollo::LoadModule("config");

  //  Load config
  { Msg_Config_Clear msg; msg.Request(); }
  {
    Msg_Config_Load msg;
    msg.sFilePathName = Apollo::GetConfig("Core/Runtime/Paths/AppBase", ""); msg.sFilePathName += "config.txt";
    int ok = msg.Request();
    if (!ok) {
      apLog_Alert(("Main", "main", "%s not found. Maybe wrong working directory?", (const char*) msg.sFilePathName));
    }
  }

  //Apollo::LoadModulesFromDir("./");
  Apollo::LoadModulesFromConfig("Core/Modules");

  { Msg_System_RunMode msg; msg.nMode = Msg_System_RunMode::RunModeGUI; msg.Send(); }
  { Msg_System_AfterLoadModules msg; msg.Send(); }
  { Msg_System_BeforeEventLoop msg; msg.Send(); }

  int nResult = 0;
  {
    Msg_MainLoop_Loop msg;
    msg.argc = argc;
    msg.argv = argv;
    Apollo::CallUnsynchronized(&msg); // no sync, because other threads may start inside the main loop
    nResult = msg.nReturnCode;
  }

  { Msg_System_AfterEventLoop msg; msg.Send(); }
  { Msg_System_BeforeUnloadModules msg; msg.Send(); }

  Apollo::UnLoadModules();

  { Msg_MainLoop_ConsoleInput msg; msg.UnHook("LinuxApp", (ApCallback) On_MainLoop_ConsoleInput); }
  { Msg_MainLoop_EventLoopEnd msg; msg.UnHook("LinuxApp", (ApCallback) On_MainLoop_EventLoopEnd); }
  { Msg_MainLoop_EventLoopBegin msg; msg.UnHook("LinuxApp", (ApCallback) On_MainLoop_EventLoopBegin); }
  { Msg_System_ModuleUnLoaded msg; msg.UnHook("LinuxApp", (ApCallback) On_System_ModuleUnLoaded); }
  { Msg_System_ModuleLoaded msg; msg.UnHook("LinuxApp", (ApCallback) On_System_ModuleLoaded); }
  { Msg_Log_SetMask msg; msg.UnHook("LinuxApp", (ApCallback) On_Log_SetMask); }
  { Msg_Log_DoLog msg; msg.UnHook("LinuxApp", (ApCallback) On_Log_DoLog); }

  Apollo::Exit();

  return nResult;
}
