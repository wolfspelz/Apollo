// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "DialogModule.h"

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
  if (pDialog == 0) { throw ApException(LOG_CONTEXT, "no Dialog=" ApHandleFormat "", ApHandlePrintf(hDialog)); }
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
  if (dialogs_.Find(pMsg->hDialog) != 0) { throw ApException(LOG_CONTEXT, "Dialog=" ApHandleFormat " already exists", ApHandlePrintf(pMsg->hDialog)); }
  Dialog* pDialog = NewDialog(pMsg->hDialog, pMsg->nLeft, pMsg->nTop, pMsg->nWidth, pMsg->nHeight, pMsg->bVisible, pMsg->sCaption, pMsg->sIconUrl, pMsg->sContentUrl);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_Destroy)
{
  Dialog* pDialog = FindDialog(pMsg->hDialog);
  if (pDialog) {
    DeleteDialog(pMsg->hDialog);
    pMsg->apStatus = ApMessage::Ok;
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_GetView)
{
  Dialog* pDialog = FindDialog(pMsg->hDialog);
  if (pDialog) {
    pMsg->hView = pDialog->GetView();
    pMsg->apStatus = ApMessage::Ok;
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_GetContentRect)
{
  Dialog* pDialog = FindDialog(pMsg->hDialog);
  if (pDialog) {
    pDialog->GetContentRect(pMsg->nLeft, pMsg->nTop, pMsg->nWidth, pMsg->nHeight);
    pMsg->apStatus = ApMessage::Ok;
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_SetCaption)
{
  Dialog* pDialog = FindDialog(pMsg->hDialog);
  if (pDialog) {
    pDialog->SetCaption(pMsg->sCaption);
    pMsg->apStatus = ApMessage::Ok;
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_SetIcon)
{
  Dialog* pDialog = FindDialog(pMsg->hDialog);
  if (pDialog) {
    pDialog->SetIcon(pMsg->sIconUrl);
    pMsg->apStatus = ApMessage::Ok;
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_CallScriptFunction)
{
  Dialog* pDialog = FindDialog(pMsg->hDialog);
  if (pDialog) {
    pMsg->sResult = pDialog->CallScriptFunction(pMsg->sFunction, pMsg->lArgs);
    pMsg->apStatus = ApMessage::Ok;
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_ContentCall)
{
  Dialog* pDialog = FindDialog(pMsg->hDialog);
  if (pDialog) {
    pDialog->ContentCall(pMsg->sFunction, pMsg->srpc, pMsg->response);
    pMsg->apStatus = ApMessage::Ok;
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, WebView_Event_DocumentLoaded)
{
  Dialog* pDialog = FindDialogByView(pMsg->hView);
  if (pDialog) {
    pDialog->OnDocumentLoaded();
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, WebView_Event_LoadError)
{
  Dialog* pDialog = FindDialogByView(pMsg->hView);
  if (pDialog) {
    pDialog->OnLoadError(pMsg->sUrl, pMsg->sComment);
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

AP_MSG_HANDLER_METHOD(DialogModule, WebView_Event_Closing)
{
  Dialog* pDialog = FindDialogByView(pMsg->hView);
  if (pDialog) {
    pDialog->OnUnload();
  }
}

AP_MSG_HANDLER_METHOD(DialogModule, WebView_Event_EmbeddedDocumentLoaded)
{
  Dialog* pDialog = FindDialogByView(pMsg->hView);
  if (pDialog) {
    pDialog->OnContentLoaded(pMsg->sUrl);
  }
}

//----------------------------

void SrpcGate_Dialog_Create(ApSRPCMessage* pMsg)
{
  Msg_Dialog_Create msg;
  msg.hDialog = pMsg->srpc.getHandle("hDialog");
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
  msg.hDialog = pMsg->srpc.getHandle("hDialog");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Dialog_GetView(ApSRPCMessage* pMsg)
{
  Msg_Dialog_GetView msg;
  msg.hDialog = pMsg->srpc.getHandle("hDialog");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->srpc.set("hView", msg.hView.toString());
}

void SrpcGate_Dialog_GetContentRect(ApSRPCMessage* pMsg)
{
  Msg_Dialog_GetContentRect msg;
  msg.hDialog = pMsg->srpc.getHandle("hDialog");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->srpc.set("nLeft", msg.nLeft);
  pMsg->srpc.set("nTop", msg.nTop);
  pMsg->srpc.set("nWidth", msg.nWidth);
  pMsg->srpc.set("nHeight", msg.nHeight);
}

void SrpcGate_Dialog_SetCaption(ApSRPCMessage* pMsg)
{
  Msg_Dialog_SetCaption msg;
  msg.hDialog = pMsg->srpc.getHandle("hDialog");
  msg.sCaption = pMsg->srpc.getString("sCaption");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_Dialog_SetIcon(ApSRPCMessage* pMsg)
{
  Msg_Dialog_SetIcon msg;
  msg.hDialog = pMsg->srpc.getHandle("hDialog");
  msg.sIconUrl = pMsg->srpc.getString("sIconUrl");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_GetContentRect, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_SetCaption, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_SetIcon, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_CallScriptFunction, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_ContentCall, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, WebView_Event_DocumentLoaded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, WebView_Event_LoadError, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, WebView_Event_ReceivedFocus, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, WebView_Event_LostFocus, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, WebView_Event_Closing, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, WebView_Event_EmbeddedDocumentLoaded, this, ApCallbackPosNormal);

  srpcGateRegistry_.add("Dialog_Create", SrpcGate_Dialog_Create);
  srpcGateRegistry_.add("Dialog_Destroy", SrpcGate_Dialog_Destroy);
  srpcGateRegistry_.add("Dialog_GetView", SrpcGate_Dialog_GetView);
  srpcGateRegistry_.add("Dialog_GetContentRect", SrpcGate_Dialog_GetContentRect);
  srpcGateRegistry_.add("Dialog_SetCaption", SrpcGate_Dialog_SetCaption);
  srpcGateRegistry_.add("Dialog_SetIcon", SrpcGate_Dialog_SetIcon);

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
  AP_UNITTEST_REGISTER(DialogModuleTester::GenerateOpenedEvent);
  AP_UNITTEST_REGISTER(DialogModuleTester::CallContentScript);
  AP_UNITTEST_REGISTER(DialogModuleTester::CallContentSrpc);
  AP_UNITTEST_REGISTER(DialogModuleTester::SetCaption);

  if (Apollo::isLoadedModule("Net") && Msg_Net_IsOnline::_()) {
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrlOpenCloseLoaded);
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrlOpenCloseComplete);
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrlOpenCloseClosing);
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrlDomElementPropertyAccess);
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrlEmbeddedDocumentEventsLoaded);
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrlEmbeddedDocumentEventsComplete);
  }
}

void DialogModuleTester::Execute()
{
  DialogModuleTester::CreateWaitCloseByContent();
  DialogModuleTester::GenerateOpenedEvent();
  DialogModuleTester::CallContentScript();
  DialogModuleTester::CallContentSrpc();
  DialogModuleTester::SetCaption();

  if (Apollo::isLoadedModule("Net") && Msg_Net_IsOnline::_()) {
    DialogModuleTester::ExternalUrlOpenClose();
    DialogModuleTester::ExternalUrlDomElementProperty();
    DialogModuleTester::ExternalUrlEmbeddedDocumentEvents();
  }
}

void DialogModuleTester::End()
{
}

//----------------------------

static ApHandle DialogModuleTester_CreateWaitCloseByContent_hDialog;

void DialogModuleTester_CreateWaitCloseByContent_WebView_Event_Closing(Msg_WebView_Event_Closing* pMsg)
{
  if (pMsg->hView != DialogModuleTester_CreateWaitCloseByContent_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::CreateWaitCloseByContent);
  { Msg_WebView_Event_Closing msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_CreateWaitCloseByContent_WebView_Event_Closing, 0); }
}

String DialogModuleTester::CreateWaitCloseByContent()
{
  String s;

  { Msg_WebView_Event_Closing msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_CreateWaitCloseByContent_WebView_Event_Closing, 0, ApCallbackPosNormal); }

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
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/CreateWaitCloseByContent.html";
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}

  return s;
}

//----------------------------

static ApHandle DialogModuleTester_GenerateOpenedEvent_hDialog;

void DialogModuleTester_GenerateOpenedEvent_Dialog_OnOpened(Msg_Dialog_OnOpened* pMsg)
{
  if (pMsg->hDialog != DialogModuleTester_GenerateOpenedEvent_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::GenerateOpenedEvent);
  { Msg_Dialog_OnOpened msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_GenerateOpenedEvent_Dialog_OnOpened, 0); }
  
  // Not a good idea to destroy the view while in body-onload -> destroy async
  { ApAsyncMessage<Msg_Dialog_Destroy> msg; msg->hDialog = DialogModuleTester_GenerateOpenedEvent_hDialog; msg.Post(); }
}

String DialogModuleTester::GenerateOpenedEvent()
{
  String s;

  { Msg_Dialog_OnOpened msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_GenerateOpenedEvent_Dialog_OnOpened, 0, ApCallbackPosNormal); }

  ApHandle hDialog = Apollo::newHandle();
  DialogModuleTester_GenerateOpenedEvent_hDialog = hDialog;

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 500;
  msg.nTop = 100;
  msg.nWidth = 300;
  msg.nHeight = 200;
  msg.bVisible = 1;
  msg.sCaption = "GenerateOpenedEvent Caption";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/GenerateOpenedEvent.html";
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}

  return s;
}

//----------------------------

static ApHandle DialogModuleTester_CallContentScript_hDialog;

void DialogModuleTester_CallContentScript_Dialog_OnOpened(Msg_Dialog_OnOpened* pMsg)
{
  if (pMsg->hDialog != DialogModuleTester_CallContentScript_hDialog) { return; }

  String s;

  if (!s) {
    // document.getElementById('iApContent').contentWindow.eval("SetText('zz')")
    Msg_Dialog_CallScriptFunction msg;
    msg.hDialog = pMsg->hDialog;
    msg.sFunction = "SetText";
    msg.lArgs.AddLast("CallContentScript Text1");
    msg.lArgs.AddLast("42");
    if (!msg.Request()) {
      s = "Msg_Dialog_CallScriptFunction failed";
    }
  }

  if (!s) {
    String sExpected = "1=CallContentScript Text1 2=42";
    String sResult = Msg_WebView_CallScriptFunction::_(pMsg->hDialog, "iApContent", "ApEval", "document.getElementById('iText').innerHTML");
    if (sResult != sExpected) {
      s = "Wrong text expected=" + sExpected + " got=" + sResult;
    }
  }

  AP_UNITTEST_RESULT(DialogModuleTester::CallContentScript, s.empty(), s);
  { Msg_Dialog_OnOpened msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_CallContentScript_Dialog_OnOpened, 0); }
  { ApAsyncMessage<Msg_Dialog_Destroy> msg; msg->hDialog = pMsg->hDialog; msg.Post(); }
}

String DialogModuleTester::CallContentScript()
{
  String s;

  { Msg_Dialog_OnOpened msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_CallContentScript_Dialog_OnOpened, 0, ApCallbackPosNormal); }

  ApHandle hDialog = Apollo::newHandle();
  DialogModuleTester_CallContentScript_hDialog = hDialog;

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 500;
  msg.nTop = 100;
  msg.nWidth = 300;
  msg.nHeight = 200;
  msg.bVisible = 1;
  msg.sCaption = "CallContentScript Caption";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/CallContentScript.html";
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}

  return s;
}

//----------------------------

static ApHandle DialogModuleTester_CallContentSrpc_hDialog;

void DialogModuleTester_CallContentSrpc_Dialog_OnOpened(Msg_Dialog_OnOpened* pMsg)
{
  if (pMsg->hDialog != DialogModuleTester_CallContentSrpc_hDialog) { return; }

  String s;

  if (!s) {
    // document.getElementById('iApContent').contentWindow.eval("SetText('zz')")
    Msg_Dialog_ContentCall msg;
    msg.hDialog = pMsg->hDialog;
    msg.sFunction = "SetText";
    msg.srpc.set("String", "41");
    msg.srpc.set("Int", 42);
    if (!msg.Request()) {
      s = "Msg_Dialog_ContentCall failed";
    }
  }

  if (!s) {
    String sExpected = "String=41\nInt=42\n";
    String sResult = Msg_WebView_CallScriptFunction::_(pMsg->hDialog, "iApContent", "ApEval", "document.getElementById('iText').innerHTML");
    if (sResult != sExpected) {
      s = "Wrong text expected=" + sExpected + " got=" + sResult;
    }
  }

  AP_UNITTEST_RESULT(DialogModuleTester::CallContentSrpc, s.empty(), s);

  { Msg_Dialog_OnOpened msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_CallContentSrpc_Dialog_OnOpened, 0); }

  { ApAsyncMessage<Msg_Dialog_Destroy> msg; msg->hDialog = DialogModuleTester_CallContentSrpc_hDialog; msg.Post(); }
}

String DialogModuleTester::CallContentSrpc()
{
  String s;

  { Msg_Dialog_OnOpened msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_CallContentSrpc_Dialog_OnOpened, 0, ApCallbackPosNormal); }

  ApHandle hDialog = Apollo::newHandle();
  DialogModuleTester_CallContentSrpc_hDialog = hDialog;

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 500;
  msg.nTop = 100;
  msg.nWidth = 300;
  msg.nHeight = 200;
  msg.bVisible = 1;
  msg.sCaption = "CallContentSrpc Caption";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/CallContentSrpc.html";
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}

  return s;
}

//----------------------------
// ExternalUrlOpenClose

static ApHandle DialogModuleTester_ExternalUrlOpenClose_hDialog;
static String DialogModuleTester_ExternalUrlOpenClose_sUrl;

void DialogModuleTester_ExternalUrlOpenClose_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrlOpenClose_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrlOpenCloseLoaded);
  { Msg_WebView_Event_DocumentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlOpenClose_WebView_Event_DocumentLoaded, 0); }
}

void DialogModuleTester_ExternalUrlOpenClose_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrlOpenClose_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrlOpenCloseComplete);
  { Msg_WebView_Event_DocumentComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlOpenClose_WebView_Event_DocumentComplete, 0); }

  { ApAsyncMessage<Msg_Dialog_Destroy> msg; msg->hDialog = pMsg->hView; msg.Post(); }
}

void DialogModuleTester_ExternalUrlOpenClose_WebView_Event_Closing(Msg_WebView_Event_Closing* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrlOpenClose_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrlOpenCloseClosing);
  { Msg_WebView_Event_Closing msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlOpenClose_WebView_Event_Closing, 0); }
}

String DialogModuleTester::ExternalUrlOpenClose()
{
  String s;

  { Msg_WebView_Event_DocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlOpenClose_WebView_Event_DocumentLoaded, 0, ApCallbackPosNormal); }
  { Msg_WebView_Event_DocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlOpenClose_WebView_Event_DocumentComplete, 0, ApCallbackPosNormal); }
  { Msg_WebView_Event_Closing msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlOpenClose_WebView_Event_Closing, 0, ApCallbackPosNormal); }

  ApHandle hDialog = Apollo::newHandle();
  DialogModuleTester_ExternalUrlOpenClose_hDialog = hDialog;
  DialogModuleTester_ExternalUrlOpenClose_sUrl = "http://en.wikipedia.org/wiki/Main_Page";

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 220;
  msg.nTop = 300;
  msg.nWidth = 800;
  msg.nHeight = 500;
  msg.bVisible = 1;
  msg.sCaption = "ExternalUrlOpenClose";
  msg.sContentUrl = DialogModuleTester_ExternalUrlOpenClose_sUrl;
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}

  return s;
}

//----------------------------
// ExternalUrlEmbeddedDocumentEvents

static ApHandle DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_hDialog;
static String DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_sUrl;

void DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_WebView_Event_EmbeddedDocumentLoaded(Msg_WebView_Event_EmbeddedDocumentLoaded* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_hDialog) { return; }
  if (pMsg->sUrl != DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_sUrl) { return; }

  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrlEmbeddedDocumentEventsLoaded);
  { Msg_WebView_Event_EmbeddedDocumentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_WebView_Event_EmbeddedDocumentLoaded, 0); }
}

void DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_WebView_Event_EmbeddedDocumentComplete(Msg_WebView_Event_EmbeddedDocumentComplete* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_hDialog) { return; }
  if (pMsg->sUrl != DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_sUrl) { return; }

  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrlEmbeddedDocumentEventsComplete);
  { Msg_WebView_Event_EmbeddedDocumentComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_WebView_Event_EmbeddedDocumentComplete, 0); }

  { ApAsyncMessage<Msg_Dialog_Destroy> msg; msg->hDialog = pMsg->hView; msg.Post(); }
}

String DialogModuleTester::ExternalUrlEmbeddedDocumentEvents()
{
  String s;

  { Msg_WebView_Event_EmbeddedDocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_WebView_Event_EmbeddedDocumentLoaded, 0, ApCallbackPosNormal); }
  { Msg_WebView_Event_EmbeddedDocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_WebView_Event_EmbeddedDocumentComplete, 0, ApCallbackPosNormal); }

  ApHandle hDialog = Apollo::newHandle();
  DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_hDialog = hDialog;
  DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_sUrl = "http://en.wikipedia.org/wiki/Main_Page";

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 240;
  msg.nTop = 320;
  msg.nWidth = 800;
  msg.nHeight = 500;
  msg.bVisible = 1;
  msg.sCaption = "ExternalUrlEmbeddedDocumentEvents";
  msg.sContentUrl = DialogModuleTester_ExternalUrlEmbeddedDocumentEvents_sUrl;
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}

  return s;
}

//----------------------------
// ExternalUrlDomElementProperty

static ApHandle DialogModuleTester_ExternalUrlDomElementProperty_hDialog;
static String DialogModuleTester_ExternalUrlDomElementProperty_sUrl;

void DialogModuleTester_ExternalUrlDomElementProperty_WebView_Event_EmbeddedDocumentComplete(Msg_WebView_Event_EmbeddedDocumentComplete* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrlDomElementProperty_hDialog) { return; }
  if (pMsg->sUrl != DialogModuleTester_ExternalUrlDomElementProperty_sUrl) { return; }

  String s;
  String sExpectedSubstring = "background-image";
  String sResult = Msg_WebView_GetElementValue::_(DialogModuleTester_ExternalUrlDomElementProperty_hDialog, "iApContent", "#p-logo", "innerHTML");
  if (!sResult.contains(sExpectedSubstring)) {
    s.appendf("got=%s expected=%s", _sz(sResult), _sz(sExpectedSubstring));
  }
  AP_UNITTEST_RESULT(DialogModuleTester::ExternalUrlDomElementPropertyAccess, s.empty(), s);

  { Msg_WebView_Event_EmbeddedDocumentComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlDomElementProperty_WebView_Event_EmbeddedDocumentComplete, 0); }

  { ApAsyncMessage<Msg_Dialog_Destroy> msg; msg->hDialog = pMsg->hView; msg.Post(); }
}

String DialogModuleTester::ExternalUrlDomElementProperty()
{
  String s;

  { Msg_WebView_Event_EmbeddedDocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrlDomElementProperty_WebView_Event_EmbeddedDocumentComplete, 0, ApCallbackPosNormal); }

  ApHandle hDialog = Apollo::newHandle();
  DialogModuleTester_ExternalUrlDomElementProperty_hDialog = hDialog;
  DialogModuleTester_ExternalUrlDomElementProperty_sUrl = "http://en.wikipedia.org/wiki/Main_Page";

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 260;
  msg.nTop = 340;
  msg.nWidth = 800;
  msg.nHeight = 500;
  msg.bVisible = 1;
  msg.sCaption = "ExternalUrlDomElementProperty";
  msg.sContentUrl = DialogModuleTester_ExternalUrlDomElementProperty_sUrl;
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}

  return s;
}

//----------------------------

static ApHandle DialogModuleTester_SetCaption_hDialog;
static String DialogModuleTester_SetCaption_sTheme;

void DialogModuleTester_SetCaption_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg)
{
  if (pMsg->hView != DialogModuleTester_SetCaption_hDialog) { return; }
  
  String s;

  if (!s) {
    Msg_Dialog_SetCaption msg;
    msg.hDialog = pMsg->hView ;
    msg.sCaption = "Final Window Caption";
    if (!msg.Request()) { s = "Msg_Dialog_SetCaption failed"; }
  }

  if (!s) {
    Msg_Dialog_SetIcon msg;
    msg.hDialog = pMsg->hView ;
    msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/SetCaptionFinalIcon.png";
    if (!msg.Request()) { s = "Msg_Dialog_SetCaption failed"; }
  }

  if (!s) {
    String sResult = Msg_WebView_CallScriptFunction::_(pMsg->hView, "", "ApEval", "$('#iApCaption').text()");
    if (sResult != "Final Window Caption") {
      s = "Caption wrong";
    }
  }

  if (!s) {
    String sResult = Msg_WebView_CallScriptFunction::_(pMsg->hView, "", "ApEval", "$('#iApIcon').attr('src')");
    if (sResult != "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/SetCaptionFinalIcon.png") {
      s = "IconUrl wrong";
    }
  }

  AP_UNITTEST_RESULT(DialogModuleTester::SetCaption, s.empty(), s);
  { Msg_WebView_Event_DocumentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_SetCaption_WebView_Event_DocumentLoaded, 0); }
  { ApAsyncMessage<Msg_Dialog_Destroy> msg; msg->hDialog = pMsg->hView; msg.Post(); }
}

String DialogModuleTester::SetCaption()
{
  String s;

  { Msg_WebView_Event_DocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_SetCaption_WebView_Event_DocumentLoaded, 0, ApCallbackPosNormal); }

  ApHandle hDialog = Apollo::newHandle();
  DialogModuleTester_SetCaption_hDialog = hDialog;
  DialogModuleTester_SetCaption_sTheme = Apollo::getModuleConfig(MODULE_NAME, "Theme", "");
  Apollo::setModuleConfig(MODULE_NAME, "Theme", "WhiteWin");

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 500;
  msg.nTop = 100;
  msg.nWidth = 300;
  msg.nHeight = 200;
  msg.bVisible = 1;
  msg.sCaption = "Initial Window Caption";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/Content.html";
  msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/SetCaptionInitialIcon.png";
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}

  if (DialogModuleTester_SetCaption_sTheme) {
    Apollo::setModuleConfig(MODULE_NAME, "Theme", DialogModuleTester_SetCaption_sTheme);
  }

  return s;
}

//----------------------------

#endif // #if defined(AP_TEST)
