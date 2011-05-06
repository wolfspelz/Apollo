// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "TestDialog.h"
#include "MsgMainLoop.h"
#include "MsgFluid.h"

#if defined(WIN32)
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
  AP_MEMORY_CHECK(dwReason);
  AP_UNUSED_ARG(hModule);
  AP_UNUSED_ARG(dwReason);
  AP_UNUSED_ARG(lpReserved);
  return TRUE;
}
#endif // defined(WIN32)

#define LOG_CHANNEL "TestDialog"
#define MODULE_NAME "TestDialog"

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME,
  "TestDialog",
  "TestDialog Module",
  "1",
  "Shows a dialog with HTML controls",
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

EXDISPLAY_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

class TestDialogModule
{
public:
  TestDialogModule()
    :hDisplay_(ApNoHandle)
  {}

  void On_Fluid_Created(Msg_Fluid_Created* pMsg);
  void On_Fluid_Loaded(Msg_Fluid_Loaded* pMsg);
  void On_Fluid_Destroyed(Msg_Fluid_Destroyed* pMsg);
  void On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg);
  void On_TestDialog_Control(ApSRPCMessage* pMsg);
  void On_TestDialog_Command(Msg_Fluid_HostCall* pMsg);

public:
  ApHandle hDisplay_;
};

typedef ApModuleSingleton<TestDialogModule> TestDialogModuleInstance;

void TestDialogModule::On_Fluid_Created(Msg_Fluid_Created* pMsg)
{
  if (pMsg->hDisplay == hDisplay_) {
    {
      ApSRPCMessage msg("TestDialog_Notification");
      msg.srpc.setString("Method", "Opened");
      (void) msg.Call();
    }

    {
      Msg_Fluid_DisplayCall msg;
      msg.hDisplay = hDisplay_;
      msg.srpc.setString("Method", Fluid_SrpcMethod_SetTitle);
      msg.srpc.setString("sTitle", Apollo::translate(MODULE_NAME, "Window", "Title"));
      (void) msg.Call();
    }

    {
      Msg_Fluid_DisplayCall msg;
      msg.hDisplay = hDisplay_;
      msg.srpc.setString("Method", Fluid_SrpcMethod_SetPosition);
      msg.srpc.setInt("nLeft", Apollo::getModuleConfig(MODULE_NAME, "Left", 300));
      msg.srpc.setInt("nTop", Apollo::getModuleConfig(MODULE_NAME, "Top", 200));
      msg.srpc.setInt("nWidth", Apollo::getModuleConfig(MODULE_NAME, "Width", 500));
      msg.srpc.setInt("nHeight", Apollo::getModuleConfig(MODULE_NAME, "Height", 400));
      (void) msg.Call();
    }

    {
      Msg_Fluid_DisplayCall msg;
      msg.hDisplay = hDisplay_;
      msg.srpc.setString("Method", Fluid_SrpcMethod_SetVisibility);
      msg.srpc.setInt("nVisible", 1);
      (void) msg.Call();
    }

  }
}

void TestDialogModule::On_Fluid_Loaded(Msg_Fluid_Loaded* pMsg)
{
  if (pMsg->hDisplay == hDisplay_) {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    msg.srpc.setString("Method", Fluid_SrpcMethod_SetVisibility);
    msg.srpc.setInt("nVisible", 1);
    (void) msg.Call();
  }
}

void TestDialogModule::On_Fluid_Destroyed(Msg_Fluid_Destroyed* pMsg)
{
  if (pMsg->hDisplay == hDisplay_) {
    hDisplay_ = ApNoHandle;

    ApSRPCMessage msg("TestDialog_Notification");
    msg.srpc.setString("Method", "Closed");
    (void) msg.Call();
  }
}

void TestDialogModule::On_MainLoop_EventLoopBeforeEnd(Msg_MainLoop_EventLoopBeforeEnd* pMsg)
{
  if (ApIsHandle(hDisplay_)) {
    ApSRPCMessage msg("TestDialog_Control");
    msg.srpc.setString("Method", "Close");
    (void) msg.Call();
  }
}

void TestDialogModule::On_TestDialog_Control(ApSRPCMessage* pMsg)
{
  String sMethod = pMsg->srpc.getString("Method");
  if (0) {
  } else if (sMethod == "Open") {
    if (!ApIsHandle(hDisplay_)) {
      hDisplay_ = Apollo::newHandle();

      String sContentUrl;
      sContentUrl.appendf("file:%sindex.html", StringType(Apollo::getModuleResourcePath(MODULE_NAME)));
      sContentUrl = Apollo::getModuleConfig(MODULE_NAME, "Html/ContentURL", sContentUrl);

      Msg_Fluid_Create msg;
      msg.hDisplay = hDisplay_;
      msg.sPathname = Apollo::getModuleConfig(MODULE_NAME, "ExePath", "..\\..\\..\\fl\\ApolloLib\\SimpleHTML\\SimpleHTML.exe");
      msg.sArgs.appendf("-url \"%s\"", StringType(sContentUrl));
      msg.bVisible = Apollo::getModuleConfig(MODULE_NAME, "InitiallyVisible", 0);
      int ok = msg.Request();
    }

  } else if (sMethod == "Close") {
    if (ApIsHandle(hDisplay_)) {
      Msg_Fluid_Destroy msg;
      msg.hDisplay = hDisplay_;
      int ok = msg.Request();
    }

  }
}

void TestDialogModule::On_TestDialog_Command(Msg_Fluid_HostCall* pMsg)
{
  String sMethod = pMsg->srpc.getString("Method");
  if (0) {
    
    //----------------------------------------------------------
    // Testdialog specific ops

  } else if (sMethod == "OpenDisplay") {
    String sPath = pMsg->srpc.getString("sPath");
    String sArgs = pMsg->srpc.getString("sArgs");

    ApHandle hNewDisplay = Apollo::newHandle();

    {
      Msg_Fluid_Create msg;
      msg.hDisplay = hNewDisplay;
      msg.sPathname = sPath;
      msg.sArgs = sArgs;
      int ok = msg.Request();
    }

    {
      Msg_Fluid_DisplayCall msg;
      msg.hDisplay = hDisplay_;
      msg.srpc.setString("Method", "OpenDisplayResponse");
      msg.srpc.setString("hDisplay", Apollo::handle2String(hNewDisplay));
      (void) msg.Call();
    }

  } else if (sMethod == "DisplayCall") {
    String sDisplay = pMsg->srpc.getString("hDisplay");
    ApHandle hDisplay = Apollo::string2Handle(sDisplay);
    String sMsg = pMsg->srpc.getString("sMsg");
    List lMsg;
    KeyValueLfBlob2List(sMsg, lMsg);

    {
      Msg_Fluid_DisplayCall msg;
      msg.hDisplay = hDisplay;
      for (Elem* e = 0; (e = lMsg.Next(e)) != 0; ) {
        msg.srpc.AddLast(e->getName(), e->getString());
      }
      (void) msg.Call();
    }

    //----------------------------------------------------------
    // Developer specific ops

    //----------------------------------------------------------
    // Everything else send to the display

  } else {
    Msg_Fluid_DisplayCall msg;
    msg.hDisplay = hDisplay_;
    pMsg->srpc >> msg.srpc;
    Elem* eApType = msg.srpc.FindByName("ApType");
    if (eApType != 0) {
      msg.srpc.Remove(eApType);
      delete eApType;
      eApType = 0;
    }
    (void) msg.Call();

  }
}

//----------------------------------------------------------

AP_REFINSTANCE_MSG_HANDLER(TestDialogModule, Fluid_Created)
AP_REFINSTANCE_MSG_HANDLER(TestDialogModule, Fluid_Loaded)
AP_REFINSTANCE_MSG_HANDLER(TestDialogModule, Fluid_Destroyed)
AP_REFINSTANCE_MSG_HANDLER(TestDialogModule, MainLoop_EventLoopBeforeEnd)
AP_REFINSTANCE_SRPC_HANDLER(TestDialogModule, ApSRPCMessage, TestDialog_Control)
AP_REFINSTANCE_SRPC_HANDLER(TestDialogModule, Msg_Fluid_HostCall, TestDialog_Command)

//----------------------------------------------------------

EXDISPLAY_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { Msg_Fluid_Created msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, Fluid_Created), TestDialogModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_Fluid_Loaded msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, Fluid_Loaded), TestDialogModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_Fluid_Destroyed msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, Fluid_Destroyed), TestDialogModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_MainLoop_EventLoopBeforeEnd msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, MainLoop_EventLoopBeforeEnd), TestDialogModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { ApSRPCMessage msg("TestDialog_Control"); msg.Hook(MODULE_NAME, AP_REFINSTANCE_SRPC_CALLBACK(TestDialogModule, TestDialog_Control), TestDialogModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_Fluid_HostCall msg("TestDialog_Command"); msg.Hook(MODULE_NAME, AP_REFINSTANCE_SRPC_CALLBACK(TestDialogModule, TestDialog_Command), TestDialogModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }

  return TestDialogModuleInstance::Get() != 0;
}

EXDISPLAY_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { Msg_Fluid_Created msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, Fluid_Created), TestDialogModuleInstance::Get()); }
  { Msg_Fluid_Loaded msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, Fluid_Loaded), TestDialogModuleInstance::Get()); }
  { Msg_Fluid_Destroyed msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, Fluid_Destroyed), TestDialogModuleInstance::Get()); }
  { Msg_MainLoop_EventLoopBeforeEnd msg; msg.UnHook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, MainLoop_EventLoopBeforeEnd), TestDialogModuleInstance::Get()); }
  { ApSRPCMessage msg("TestDialog_Control"); msg.UnHook(MODULE_NAME, AP_REFINSTANCE_SRPC_CALLBACK(TestDialogModule, TestDialog_Control), TestDialogModuleInstance::Get()); }
  { Msg_Fluid_HostCall msg("TestDialog_Command"); msg.UnHook(MODULE_NAME, AP_REFINSTANCE_SRPC_CALLBACK(TestDialogModule, TestDialog_Command), TestDialogModuleInstance::Get()); }

  TestDialogModuleInstance::Delete();

  return 1;
}
