// ============================================================================
//
// Apollo
//
// ============================================================================

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import "Main/Main.h"
#import "MsgOs.h"

int main(int argc, char** argv)
{
  [NSApplication sharedApplication];
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
  
  apLog_SetMask(apLog_MaskMaxVerbose); // apLog_MaskMaxVerbose
  
  if (!Apollo::Init(argc, argv)) {
    apLog_Alert(("Main", "MacApp", "Apollo::Init() failed"));
  }
  
  apLog_Info(("Main", "main", ""));

  { Msg_Log_MaskToString msg; msg.Hook("MacApp", (ApCallback) On_Log_MaskToString, 0, ApCallbackPosNormal); }
  { Msg_Log_DoLog msg; msg.Hook("MacApp", (ApCallback) On_Log_DoLog, 0, ApCallbackPosNormal); }
  { Msg_Log_SetMask msg; msg.Hook("MacApp", (ApCallback) On_Log_SetMask, 0, ApCallbackPosNormal); }
  
  { Msg_Core_ModuleLoaded msg; msg.Hook("MacApp", (ApCallback) On_Core_ModuleLoaded, 0, ApCallbackPosNormal); }
  { Msg_Core_ModuleUnloaded msg; msg.Hook("MacApp", (ApCallback) On_Core_ModuleUnloaded, 0, ApCallbackPosNormal); }
  { Msg_MainLoop_EventLoopBegin msg; msg.Hook("MacApp", (ApCallback) On_MainLoop_EventLoopBegin, 0, ApCallbackPosNormal); }
  { Msg_MainLoop_EventLoopEnd msg; msg.Hook("MacApp", (ApCallback) On_MainLoop_EventLoopEnd, 0, ApCallbackPosNormal); }
  { Msg_MainLoop_ConsoleInput msg; msg.Hook("MacApp", (ApCallback) On_MainLoop_ConsoleInput, 0, ApCallbackPosNormal); }
    
  {
    SString sModule = "config";
    int ok = Apollo::LoadModule("Config", sModule);
    if (!ok) {
      apLog_Alert(("Main", "MacApp", "Could not load module %s", (const char*) sModule));
    }
  }
  
  //  Load config
  { Msg_Config_Clear msg; msg.Request(); }
  {
    Msg_Config_Load msg;
    msg.sFilePathName = Apollo::GetAppResourcePath(); msg.sFilePathName += "config.txt";
    int ok = msg.Request();
    if (!ok) {
      apLog_Alert(("Main", "MacApp", "%s not found. Maybe wrong working directory?", (char*) msg.sFilePathName));
    }
  }

  Apollo::LoadModulesFromConfig("Core/Modules");
  
  { Msg_System_RunMode msg; msg.nMode = Msg_System_RunMode::RunModeGUI; msg.Send(); }
  { Msg_System_AfterLoadModules msg; msg.Send(); }
  { Msg_System_BeforeEventLoop msg; msg.Send(); }
  
  int nResult = 0;
  {
    Msg_MainLoop_Loop msg;
    msg.argc = argc;
    msg.argv = argv;
    Apollo::Call(&msg, Apollo::CF_UNSYNCHRONIZED); // no sync, because other threads may start inside the main loop

    nResult = msg.nReturnCode;
  }

  { Msg_System_AfterEventLoop msg; msg.Send(); }
  { Msg_System_BeforeUnloadModules msg; msg.Send(); }

  Apollo::UnLoadModules();

  { Msg_MainLoop_ConsoleInput msg; msg.UnHook("MacApp", (ApCallback) On_MainLoop_ConsoleInput); }
  { Msg_MainLoop_EventLoopEnd msg; msg.UnHook("MacApp", (ApCallback) On_MainLoop_EventLoopEnd); }
  { Msg_MainLoop_EventLoopBegin msg; msg.UnHook("MacApp", (ApCallback) On_MainLoop_EventLoopBegin); }
  { Msg_Core_ModuleUnloaded msg; msg.UnHook("MacApp", (ApCallback) On_Core_ModuleUnloaded); }
  { Msg_Core_ModuleLoaded msg; msg.UnHook("MacApp", (ApCallback) On_Core_ModuleLoaded); }
  { Msg_Log_SetMask msg; msg.UnHook("MacApp", (ApCallback) On_Log_SetMask); }
  { Msg_Log_DoLog msg; msg.UnHook("MacApp", (ApCallback) On_Log_DoLog); }
  { Msg_Log_MaskToString msg; msg.UnHook("MacApp", (ApCallback) On_Log_MaskToString); }
  
  Apollo::Exit();

  [pPool release];

  return nResult;  
}
