// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApContainer.h"
#include "apLog.h"
#include "TestDialog.h"
#include "MsgDialog.h"
#include "MsgNavigation.h"
#include "MsgVpView.h"
#include "MsgWebView.h"
#include "MsgSystem.h"

#define LOG_CHANNEL MODULE_NAME
#define LOG_CONTEXT apLog_Context

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

  int Init();
  void Exit();
  void Open();

  void On_TestDialog_Srpc(ApSRPCMessage* pMsg);
  void On_WebView_ModuleCall(Msg_WebView_ModuleCall* pMsg);
  void On_System_RunLevel(Msg_System_RunLevel* pMsg);
  void On_Dialog_OnClosed(Msg_Dialog_OnClosed* pMsg);

protected:
  ApHandle hBrowserConnection_;
  Apollo::KeyValueList kvBrowserTabContextList_;
  String sBrowserSelectedTab_;
  ApHandleTree<int> dialogs_;

  AP_MSG_REGISTRY_DECLARE;
};

typedef ApModuleSingleton<TestDialogModule> TestDialogModuleInstance;

//----------------------------------------------------------

AP_MSGCLASS_HANDLER_METHOD(TestDialogModule, TestDialog_Srpc, ApSRPCMessage)
{
  String sMethod = pMsg->srpc.getString(Srpc::Key::Method);
  if (0) {
  } else if (sMethod == "Open") {
    Open();
  }

  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(TestDialogModule, WebView_ModuleCall)
{
  if (dialogs_.Find(pMsg->hView) == 0) { return; }

  String sMethod = pMsg->srpc.getString(Srpc::Key::Method);
  if (0) {

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

    Msg_Navigation_ContextOpen msg;
    msg.hConnection = hBrowserConnection_ = (ApIsHandle(hBrowserConnection_) ? hBrowserConnection_ : Apollo::newHandle());
    msg.hContext = hContext;
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

      Msg_Navigation_ContextNavigate msg;
      msg.hConnection = hBrowserConnection_ = (ApIsHandle(hBrowserConnection_) ? hBrowserConnection_ : Apollo::newHandle());
      msg.hContext = hContext;
      msg.sUrl = pMsg->srpc.getString("sUrl");
      msg.Request();
    }

  } else if (sMethod == "TabClose") {
    String sTab = pMsg->srpc.getString("sTab");
    Apollo::KeyValueElem* pElem = kvBrowserTabContextList_.find(sTab);
    if (pElem) {
      ApHandle hContext = pElem->getHandle();
      kvBrowserTabContextList_.removeElem(pElem);

      Msg_Navigation_ContextClose msg;
      msg.hContext = hContext;
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

  } else if (sMethod == "Cwd") {
    apLog_Debug((LOG_CHANNEL, LOG_CONTEXT, "cwd=%s", _sz(Msg_System_GetCurrentWorkingDirectory::_())));

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

AP_MSG_HANDLER_METHOD(TestDialogModule, Dialog_OnClosed)
{
  if (dialogs_.Find(pMsg->hDialog) != 0) {
    dialogs_.Unset(pMsg->hDialog);
  }
}

//----------------------------------------------------------

int TestDialogModule::Init()
{
  AP_MSG_REGISTRY_ADD(MODULE_NAME, TestDialogModule, TestDialog_Srpc, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, TestDialogModule, WebView_ModuleCall, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, TestDialogModule, System_RunLevel, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, TestDialogModule, Dialog_OnClosed, this, ApCallbackPosNormal);

  return 1;
}

void TestDialogModule::Exit()
{
  AP_MSG_REGISTRY_FINISH;
}

void TestDialogModule::Open()
{
  ApHandle hDialog = Apollo::newHandle();

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = Apollo::getModuleConfig(MODULE_NAME, "Left", 200);
  msg.nTop = Apollo::getModuleConfig(MODULE_NAME, "Top", 200);
  msg.nWidth = Apollo::getModuleConfig(MODULE_NAME, "Width", 500);
  msg.nHeight = Apollo::getModuleConfig(MODULE_NAME, "Height", 300);
  msg.bVisible = 1;
  msg.sCaption = "Test Controls";
  msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "icon.png";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "index.html";
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "Msg_Dialog_Create failed: %s", _sz(msg.sComment)); }

  dialogs_.Set(hDialog, 1);
}

//----------------------------------------------------------

TESTDIALOG_API int Load(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  TestDialogModuleInstance::Delete();
  int ok = TestDialogModuleInstance::Get()->Init();
 
  return ok;
}

TESTDIALOG_API int UnLoad(AP_MODULE_CALL* pModuleData)
{
  AP_UNUSED_ARG(pModuleData);

  TestDialogModuleInstance::Get()->Exit();
  TestDialogModuleInstance::Delete();

  return 1;
}