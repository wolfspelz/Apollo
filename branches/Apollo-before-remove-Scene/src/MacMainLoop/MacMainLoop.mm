// ============================================================================
//
// Apollo
//
// ============================================================================

#import "Apollo.h"
#import "MacMainLoop.h"
#import "ApOS.h"
#import "MsgConfig.h"
//#include "MsgUnitTest.h"
//#include "MsgTimer.h"
#import "MsgMainLoop.h"
#import "MsgSystem.h"
#import "MsgGUI.h"

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import "AppController.h"


static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "MainLoop",
  "MacOS MainLoop Module",
  1,
  "The module implements native application loop for Mac",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_UrlHomepage
};

MACMAINLOOP_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

// .-----------------------------------------

class MainLoopModule
{
public:
  MainLoopModule() {}

  void On_MainLoop_Loop(Msg_MainLoop_Loop* pMsg);

  void On_MainLoop_ModuleFinished(Msg_MainLoop_ModuleFinished* pMsg);
  
  void On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg);
  void On_MainLoop_EndLoop(Msg_MainLoop_EndLoop* pMsg);
};

typedef ApModuleSingleton<MainLoopModule> MainLoopModuleInstance;

// .-----------------------------------------

void MainLoopModule::On_MainLoop_Loop(Msg_MainLoop_Loop* pMsg)
{
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
  
  ApNotificationQueue* pNotificationQueue = [[[ApNotificationQueue alloc] init] autorelease];
  [pNotificationQueue registerForNotifications:APTHREADMSG_NOTIFICATIONID];
  
  AppController* pDelegate = [[[AppController alloc] init] autorelease];
  [NSApp setDelegate:pDelegate];
  
  //[pWindow makeKeyWindow];
  int ok = 1;
  
#if 0
  ApHandle hAppWindow = 0;
  {
    Msg_GUI_WebView_Create msg_create; msg_create.rView = ApRect(100,100,500,400); 
    msg_create.minWidth = 100; msg_create.minHeight = 100; msg_create.Request();
    
    if (msg_create.apStatus != ApMessage::Ok) {
      apLog_Error((LOG_CHANNEL, "MainLoopModule::On_MainLoop_Loop", "GUI_WebView_Create failed: %s",
                  (const char*)msg_create.sComment));
    } else {
      ok = 1;
      hAppWindow = msg_create.hWebView;
      apLog_Debug((LOG_CHANNEL, "MainLoopModule::On_MainLoop_Loop", "AppWindow created with handle[" ApHandleFormat "]", msg_create.hWebView));
    }
    if (ok) {
      Msg_GUI_WebView_LoadContent msg; msg.sContentName = "file:///Users/ruzarowski/breakpointEditor.html";
      msg.hWebView = msg_create.hWebView; msg.Request();
      if (msg.apStatus != ApMessage::Ok) {
        ok = 0;
        Msg_GUI_WebView_Destroy msg_destroy; msg_destroy.hWebView = msg_create.hWebView; msg_destroy.Request();
        apLog_Error((LOG_CHANNEL, "MainLoopModule::On_MainLoop_Loop", "GUI_WebView_LoadContent failed: %s",
                    (const char*)msg.sComment));
      } else {
        apLog_Debug((LOG_CHANNEL, "MainLoopModule::On_MainLoop_Loop", "AppWindow loaded content [%s]", 
                    (const char*)msg.sContentName));
      }
    }
    if (ok) {
      Msg_GUI_WebView_Show msg; msg.bDoShow = 1;
      msg.hWebView = msg_create.hWebView; msg.Request();
      if (msg.apStatus != ApMessage::Ok) {
        ok = 0;
        Msg_GUI_WebView_Destroy msg_destroy; msg_destroy.hWebView = msg_create.hWebView; msg_destroy.Request();
        apLog_Error((LOG_CHANNEL, "MainLoopModule::On_MainLoop_Loop", "GUI_WebView_Show failed: %s",
            (const char*)msg.sComment));
      } else {
        apLog_Debug((LOG_CHANNEL, "MainLoopModule::On_MainLoop_Loop", "AppWindow shown"));
      }
    }
  }
#endif
  
  if (ok) {
    apLog_Debug((LOG_CHANNEL, "MainLoopModule::On_MainLoop_Loop", "Starting NSApplication's main loop"));
    [NSApp run];

#if 0
    Msg_GUI_WebView_Destroy msg_destroy; msg_destroy.hWebView = hAppWindow; msg_destroy.Request();
#endif

  }
  
  [NSApp setDelegate:nil]; // to avoid messages sent to a freed object 
                          // (pDelegate gets freed with the autorelease pool)
  [pPool release];
  
  pMsg->apStatus = (ok ? ApMessage::Ok : ApMessage::Error);
  pMsg->Stop();
}

void MainLoopModule::On_MainLoop_ModuleFinished(Msg_MainLoop_ModuleFinished* pMsg)
{
  AP_UNUSED_ARG(pMsg);

  AppController* pController = [NSApp delegate];
  if (pController) {
    [pController moduleFinished];
  }
}

void MainLoopModule::On_System_ThreadMessage(Msg_System_ThreadMessage* pMsg)
{
  pMsg->apStatus = ApMessage::Ok;
  
  NSNotificationCenter* pCenter = [NSNotificationCenter defaultCenter];
  if (pCenter != NULL) {
    NSApMessage* pNSApMsg = [[NSApMessage alloc] initWithApMessage:(ApMessage*)(pMsg->pMsg)];
    if (pNSApMsg != NULL) {  
      [pCenter postNotificationName:APTHREADMSG_NOTIFICATIONID object:pNSApMsg]; 
    }
  }
  pMsg->Stop();
}

void MainLoopModule::On_MainLoop_EndLoop(Msg_MainLoop_EndLoop* pMsg)
{
  [NSApp terminate:nil];
  pMsg->apStatus = ApMessage::Ok;
  pMsg->Stop();
}

// .-----------------------------------------

#if defined(WIN32)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_Win32Loop);
#elif defined(LINUX) || defined(MAC)
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_Loop);
#else
#error mainloop handler not implemented for this os
#endif

AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_ModuleFinished)

AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, System_ThreadMessage);
AP_REFINSTANCE_MSG_HANDLER(MainLoopModule, MainLoop_EndLoop);

// ------------------------------------------

MACMAINLOOP_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
  MainLoopModuleInstance::Delete();

#if defined(WIN32)  
  { Msg_MainLoop_Win32Loop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_Win32Loop), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
#elif defined(LINUX) || defined(MAC)
  { Msg_MainLoop_Loop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_Loop), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
#else
#error mainloop handler not implemented for this os
#endif
  { Msg_MainLoop_ModuleFinished msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_ModuleFinished), MainLoopModuleInstance::Get(), ApCallbackPosNormal); }
  { Msg_System_ThreadMessage msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, System_ThreadMessage), MainLoopModuleInstance::Get(), ApCallbackPosEarly); }
  { Msg_MainLoop_EndLoop msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_EndLoop), MainLoopModuleInstance::Get(), ApCallbackPosEarly); }
#if defined(APOLLOTEST)
#endif

//Msg_

  return MainLoopModuleInstance::Get() != NULL;
}

MACMAINLOOP_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  
#if defined(WIN32) 
  { Msg_MainLoop_Win32Loop msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_Win32Loop)); }
#elif defined(LINUX) || defined(MAC)
  { Msg_MainLoop_Loop msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_Loop)); }
#else
#error mainloop handler not implemented for this os
#endif
  { Msg_MainLoop_ModuleFinished msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_ModuleFinished)); }
  { Msg_System_ThreadMessage msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, System_ThreadMessage)); }
  { Msg_MainLoop_EndLoop msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(MainLoopModule, MainLoop_EndLoop)); }
#if defined(APOLLOTEST)
#endif

  MainLoopModuleInstance::Delete();

  return 1;
}








