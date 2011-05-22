// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"

Dialog* DialogModule::NewDialog(const ApHandle& hDialog, int nLeft, int nTop, int nWidth, int nHeight, int bVisible, const String& sCaption, const String& sIconUrl, const String& sContentUrl)
{
  Dialog* pDialog = new Dialog(hDialog);
  if (pDialog) {
    try {
      pDialog->Create(nLeft, nTop, nWidth, nHeight, bVisible, sCaption, sIconUrl, sContentUrl);
      dialogs_.Set(hDialog, pDialog);
    } catch (ApException& ex) {
      delete pDialog;
      pDialog = 0;
      throw ex;
    }
  }

  return pDialog;
}

void DialogModule::DeleteDialog(const ApHandle& hDialog)
{
  Dialog* pDialog = GetDialog(hDialog);
  if (pDialog) {
    pDialog->Destroy();
    dialogs_.Unset(hDialog);
    delete pDialog;
    pDialog = 0;
  }
}

Dialog* DialogModule::FindDialog(const ApHandle& hDialog)
{
  Dialog* pDialog = 0;
  dialogs_.Get(hDialog, pDialog);
  return pDialog;
}

Dialog* DialogModule::GetDialog(const ApHandle& hDialog)
{
  Dialog* pDialog = FindDialog(hDialog);  
  if (pDialog == 0) { throw ApException("DialogModule::FindDialog no Dialog=" ApHandleFormat "", ApHandleType(hDialog)); }
  return pDialog;
}

Dialog* DialogModule::FindDialogByView(const ApHandle& hView)
{
  Dialog* pDialog = 0;
  // hView == hDialog
  dialogs_.Get(hView, pDialog);
  return pDialog;
}

//----------------------------

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_Create)
{
  if (dialogs_.Find(pMsg->hDialog) != 0) { throw ApException("DialogModule::Dialog_Create: Dialog=" ApHandleFormat " already exists", ApHandleType(pMsg->hDialog)); }
  Dialog* pDialog = NewDialog(pMsg->hDialog, pMsg->nLeft, pMsg->nTop, pMsg->nWidth, pMsg->nHeight, pMsg->bVisible, pMsg->sCaption, pMsg->sIconUrl, pMsg->sContentUrl);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_Destroy)
{
  Dialog* pDialog = GetDialog(pMsg->hDialog);
  DeleteDialog(pMsg->hDialog);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_GetView)
{
  Dialog* pDialog = GetDialog(pMsg->hDialog);
  pMsg->hView = pDialog->GetView();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(DialogModule, WebView_Event_DocumentLoaded)
{
  Dialog* pDialog = FindDialogByView(pMsg->hView);
  if (pDialog) {
    pDialog->OnDocumentLoaded();
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, WebView_Event_ReceivedFocus)
{
  Dialog* pDialog = FindDialogByView(pMsg->hView);
  if (pDialog) {
    pDialog->OnReceivedFocus();
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, WebView_Event_LostFocus)
{
  Dialog* pDialog = FindDialogByView(pMsg->hView);
  if (pDialog) {
    pDialog->OnLostFocus();
  }
}

//----------------------------

void SrpcGate_Dialog_Create(ApSRPCMessage* pMsg)
{
  Msg_Dialog_Create msg;
  msg.hDialog = Apollo::string2Handle(pMsg->srpc.getString("hDialog"));
  msg.nLeft = pMsg->srpc.getInt("nLeft");
  msg.nTop = pMsg->srpc.getInt("nTop");
  msg.nWidth = pMsg->srpc.getInt("nWidth");
  msg.nHeight = pMsg->srpc.getInt("nHeight");
  msg.bVisible = pMsg->srpc.getInt("bVisible");
  msg.sCaption = pMsg->srpc.getString("sCaption");
  msg.sIconUrl = pMsg->srpc.getString("sIconUrl");
  msg.sContentUrl = pMsg->srpc.getString("sContentUrl");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Dialog_Destroy(ApSRPCMessage* pMsg)
{
  Msg_Dialog_Destroy msg;
  msg.hDialog = Apollo::string2Handle(pMsg->srpc.getString("hDialog"));
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Dialog_GetView(ApSRPCMessage* pMsg)
{
  Msg_Dialog_GetView msg;
  msg.hDialog = Apollo::string2Handle(pMsg->srpc.getString("hDialog"));
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->srpc.getString("hView") = msg.hView.toString();
}

//----------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(DialogModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Dialog", 0)) {
    DialogModuleTester::Begin();
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Dialog", 0)) {
    DialogModuleTester::Execute();
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/Dialog", 0)) {
    DialogModuleTester::End();
  }
}

#endif // #if defined(AP_TEST)

//----------------------------

int DialogModule::Init()
{
  int ok = 1;

  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_GetView, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, WebView_Event_DocumentLoaded, this, ApCallbackPosNormal);

  srpcGateRegistry_.add("Dialog_Create", SrpcGate_Dialog_Create);
  srpcGateRegistry_.add("Dialog_Destroy", SrpcGate_Dialog_Destroy);
  srpcGateRegistry_.add("Dialog_GetView", SrpcGate_Dialog_GetView);

  AP_UNITTEST_HOOK(DialogModule, this);

  return ok;
}

void DialogModule::Exit()
{
  srpcGateRegistry_.finish();
  AP_UNITTEST_UNHOOK(DialogModule, this);
  AP_MSG_REGISTRY_FINISH;
}

//----------------------------------------------------------

#if defined(AP_TEST)

void DialogModuleTester::Begin()
{
  AP_UNITTEST_REGISTER(DialogModuleTester::CreateWaitCloseByContent);
  AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrl);
}

void DialogModuleTester::Execute()
{
  DialogModuleTester::CreateWaitCloseByContent();
  DialogModuleTester::ExternalUrl();
}

void DialogModuleTester::End()
{
}

//----------------------------

static ApHandle DialogModuleTester_CreateWaitCloseByContent_hDialog;

void DialogModuleTester_CreateWaitCloseByContent_WebView_Event_DocumentUnload(Msg_WebView_Event_DocumentUnload* pMsg)
{
  if (pMsg->hView != DialogModuleTester_CreateWaitCloseByContent_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::CreateWaitCloseByContent);
  { Msg_WebView_Event_DocumentUnload msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_CreateWaitCloseByContent_WebView_Event_DocumentUnload, 0); }
}

String DialogModuleTester::CreateWaitCloseByContent()
{
  String s;

  { Msg_WebView_Event_DocumentUnload msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_CreateWaitCloseByContent_WebView_Event_DocumentUnload, 0, ApCallbackPosNormal); }

  ApHandle hDialog = Apollo::newHandle();
  DialogModuleTester_CreateWaitCloseByContent_hDialog = hDialog;

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 500;
  msg.nTop = 100;
  msg.nWidth = 300;
  msg.nHeight = 200;
  msg.bVisible = 1;
  msg.sCaption = "Very Long Window Caption";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/Content.html";
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}

  return s;
}

//----------------------------

static ApHandle DialogModuleTester_ExternalUrl_hDialog;

void DialogModuleTester_ExternalUrl_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrl_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrl);
  { Msg_WebView_Event_DocumentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_DocumentLoaded, 0); }
}

void DialogModuleTester_ExternalUrl_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrl_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrl);
  { Msg_WebView_Event_DocumentComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_DocumentComplete, 0); }
}

void DialogModuleTester_ExternalUrl_WebView_Event_DocumentUnload(Msg_WebView_Event_DocumentUnload* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrl_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrl);
  { Msg_WebView_Event_DocumentUnload msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_DocumentUnload, 0); }
}

String DialogModuleTester::ExternalUrl()
{
  String s;

  { Msg_WebView_Event_DocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_DocumentLoaded, 0, ApCallbackPosNormal); }
  { Msg_WebView_Event_DocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_DocumentComplete, 0, ApCallbackPosNormal); }
  { Msg_WebView_Event_DocumentUnload msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_DocumentUnload, 0, ApCallbackPosNormal); }

  ApHandle hDialog = Apollo::newHandle();
  DialogModuleTester_ExternalUrl_hDialog = hDialog;

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 220;
  msg.nTop = 300;
  msg.nWidth = 800;
  msg.nHeight = 500;
  msg.bVisible = 1;
  msg.sCaption = "Wikipedia";
  msg.sContentUrl = "http://en.wikipedia.org/wiki/Main_Page";
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}

  return s;
}

//----------------------------

#endif // #if defined(AP_TEST)
