// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "TestDialog.h"
#include "MsgDialog.h"
#include "MsgNavigation.h"
#include "MsgVpView.h"
#include "MsgSystem.h"

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

#define MODULE_NAME "TestDialog"

static AP_MODULE_INFO g_info = {
  sizeof(AP_MODULE_INFO),
  AP_MODULE_INTERFACE_FLAGS_CPP | AP_MODULE_INTERFACE_FLAGS_UTF8
  ,
  MODULE_NAME, // szName
  "", // szServices
  "TestDialog Module", // szLongName
  "1", // szVersion
  "Shows a dialog with test controls", // szDescription
  PROJECT_Author,
  PROJECT_Email,
  PROJECT_Copyright,
  PROJECT_HomepageUrl
};

TESTDIALOG_API AP_MODULE_INFO* Info(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);
  return &g_info;
}

//----------------------------------------------------------

class TestDialogModule
{
public:
  TestDialogModule() {}
  virtual ~TestDialogModule() {}

  void Init();
  void Exit();
  void Open();

  void On_TestDialog_Control(ApSRPCMessage* pMsg);
  void On_System_RunLevel(Msg_System_RunLevel* pMsg);

protected:
  AP_MSG_REGISTRY_DECLARE;

  ApHandle hBrowserConnection_;
  Apollo::KeyValueList kvBrowserTabContextList_;
  String sBrowserSelectedTab_;
};

typedef ApModuleSingleton<TestDialogModule> TestDialogModuleInstance;

//----------------------------------------------------------

void TestDialogModule::Init()
{
}

void TestDialogModule::Exit()
{
}

void TestDialogModule::Open()
{
  Msg_Dialog_Create msg;
  msg.hDialog = Apollo::newHandle();
  msg.nLeft = Apollo::getModuleConfig(MODULE_NAME, "Left", 200);
  msg.nTop = Apollo::getModuleConfig(MODULE_NAME, "Top", 200);
  msg.nWidth = Apollo::getModuleConfig(MODULE_NAME, "Width", 500);
  msg.nHeight = Apollo::getModuleConfig(MODULE_NAME, "Height", 300);
  msg.bVisible = 1;
  msg.sCaption = "Test Controls";
  msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "icon.png";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "index.html";
  if (!msg.Request()) { throw ApException("Msg_Dialog_Create failed: %s", StringType(msg.sComment)); }
}

//----------------------------------------------------------

AP_TYPEDMSG_HANDLER_METHOD(TestDialogModule, TestDialog_Control, ApSRPCMessage)
{
  String sMethod = pMsg->srpc.getString("Method");
  if (0) {
  } else if (sMethod == "Open") {
    Open();

  } else if (sMethod == "TabPosition") {
    String sTab = pMsg->srpc.getString("sTab");
    Apollo::KeyValueElem* pElem = kvBrowserTabContextList_.find(sTab);
    ApHandle hContext;
    if (pElem) {
      hContext = pElem->getHandle();
    } else {
      hContext = Apollo::newHandle();
      kvBrowserTabContextList_.add(sTab, hContext);
    }

    Msg_Navigation_Receive msg;
    ApIN ApHandle hConnection = hBrowserConnection_ = (ApIsHandle(hBrowserConnection_) ? hBrowserConnection_ : Apollo::newHandle());
    msg.srpc.set("Method", "Context.Open");
    msg.srpc.set("hContext", hContext);
    msg.Request();

    int nLeft = pMsg->srpc.getInt("nLeft");
    int nBottom = pMsg->srpc.getInt("nBottom");
    int nWidth = pMsg->srpc.getInt("nWidth");
    int nHeight = pMsg->srpc.getInt("nHeight");
    {
      Msg_VpView_ContextPosition msg;
      msg.hContext = hContext;
      msg.nLeft = nLeft;
      msg.nBottom = nBottom;
      msg.Send();
    }
    {
      Msg_VpView_ContextSize msg;
      msg.hContext = hContext;
      msg.nWidth = nWidth;
      msg.nHeight = nHeight;
      msg.Send();
    }
    {
      Msg_VpView_ContextVisibility msg;
      msg.hContext = hContext;
      msg.bVisible = 1;
      msg.Send();
    }

  } else if (sMethod == "TabNavigate") {
    String sTab = pMsg->srpc.getString("sTab");
    Apollo::KeyValueElem* pElem = kvBrowserTabContextList_.find(sTab);
    if (pElem) {
      ApHandle hContext = pElem->getHandle();
      Msg_Navigation_Receive msg;
      ApIN ApHandle hConnection = hBrowserConnection_ = (ApIsHandle(hBrowserConnection_) ? hBrowserConnection_ : Apollo::newHandle());
      msg.srpc.set("Method", "Context.Navigate");
      msg.srpc.set("hContext", hContext);
      msg.srpc.set("sUrl", pMsg->srpc.getString("sUrl"));
      msg.Request();
    }

  } else if (sMethod == "TabClose") {
    String sTab = pMsg->srpc.getString("sTab");
    Apollo::KeyValueElem* pElem = kvBrowserTabContextList_.find(sTab);
    if (pElem) {
      ApHandle hContext = pElem->getHandle();
      kvBrowserTabContextList_.removeElem(pElem);
      Msg_Navigation_Receive msg;
      ApIN ApHandle hConnection = hBrowserConnection_ = (ApIsHandle(hBrowserConnection_) ? hBrowserConnection_ : Apollo::newHandle());
      msg.srpc.set("Method", "Context.Close");
      msg.srpc.set("hContext", hContext);
      msg.Request();
      delete pElem; pElem = 0;
    }

  } else if (sMethod == "TabSelect") {
    String sTab = pMsg->srpc.getString("sTab");
    if (sBrowserSelectedTab_ != sTab) {
      if (sBrowserSelectedTab_) {
        Apollo::KeyValueElem* p = kvBrowserTabContextList_.find(sBrowserSelectedTab_);
        if (p) {
          Msg_VpView_ContextVisibility msg;
          msg.hContext = p->getHandle();
          msg.bVisible = 0;
          msg.Send();
        }
      }
      if (sTab) {
        Apollo::KeyValueElem* p = kvBrowserTabContextList_.find(sTab);
        if (p) {
          Msg_VpView_ContextVisibility msg;
          msg.hContext = p->getHandle();
          msg.bVisible = 1;
          msg.Send();
        }
      }
      sBrowserSelectedTab_ = sTab;
    }

  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(TestDialogModule, System_RunLevel)
{
  if (0) {
  } else if (pMsg->sLevel == Msg_System_RunLevel_Normal) {
    if (Apollo::getModuleConfig(MODULE_NAME, "OpenOnStartup", 0)) {
      Open();
    }

  }
}

//----------------------------------------------------------

TESTDIALOG_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  TestDialogModuleInstance::Delete();
  TestDialogModuleInstance::Get()->Init();
 
  { ApSRPCMessage msg("TestDialog_Control"); msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, TestDialog_Control), TestDialogModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }
  { Msg_System_RunLevel msg; msg.Hook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, System_RunLevel), TestDialogModuleInstance::Get(), Apollo::modulePos(ApCallbackPosEarly, MODULE_NAME)); }

  return 1;
}

TESTDIALOG_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  { ApSRPCMessage msg("TestDialog_Control"); msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, TestDialog_Control), TestDialogModuleInstance::Get()); }
  { Msg_System_RunLevel msg; msg.Unhook(MODULE_NAME, AP_REFINSTANCE_MSG_CALLBACK(TestDialogModule, System_RunLevel), TestDialogModuleInstance::Get()); }

  TestDialogModuleInstance::Get()->Exit();
  TestDialogModuleInstance::Delete();

  return 1;
}