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
    pDialog->GetView();
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

AP_MSG_HANDLER_METHOD(DialogModule, Dialog_ContentLoaded)
{
  Dialog* pDialog = FindDialogByView(pMsg->hView);
  if (pDialog) {
    pDialog->OnContentLoaded();
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
  pMsg->srpc.getString("hView") = msg.hView.toString();
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

void SrpcGate_Dialog_ContentLoaded(ApSRPCMessage* pMsg)
{
  Msg_Dialog_ContentLoaded msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  msg.Send();
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_SetCaption, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_SetIcon, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_CallScriptFunction, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_ContentCall, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, WebView_Event_DocumentLoaded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, WebView_Event_ReceivedFocus, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, WebView_Event_LostFocus, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, WebView_Event_Closing, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, DialogModule, Dialog_ContentLoaded, this, ApCallbackPosNormal);

  srpcGateRegistry_.add("Dialog_Create", SrpcGate_Dialog_Create);
  srpcGateRegistry_.add("Dialog_Destroy", SrpcGate_Dialog_Destroy);
  srpcGateRegistry_.add("Dialog_GetView", SrpcGate_Dialog_GetView);
  srpcGateRegistry_.add("Dialog_SetCaption", SrpcGate_Dialog_SetCaption);
  srpcGateRegistry_.add("Dialog_SetIcon", SrpcGate_Dialog_SetIcon);
  srpcGateRegistry_.add("Dialog_ContentLoaded", SrpcGate_Dialog_ContentLoaded);

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
  //AP_UNITTEST_REGISTER(DialogModuleTester::CreateWaitCloseByContent);
  //AP_UNITTEST_REGISTER(DialogModuleTester::ContentLoadedFromHtml);
  //AP_UNITTEST_REGISTER(DialogModuleTester::CallContentScript);
  //AP_UNITTEST_REGISTER(DialogModuleTester::CallContentSrpc);
  //AP_UNITTEST_REGISTER(DialogModuleTester::SetCaption);

  if (Apollo::isLoadedModule("Net") && Msg_Net_IsOnline::_()) {
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrl);
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrlLoaded);
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrlEmbeddedDocumentLoaded);
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrlEmbeddedDocumentComplete);
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrlDomElementProperty);
    AP_UNITTEST_REGISTER(DialogModuleTester::ExternalUrlClosing);
  }
}

void DialogModuleTester::Execute()
{
  //DialogModuleTester::CreateWaitCloseByContent();
  //DialogModuleTester::ContentLoadedFromHtml();
  //DialogModuleTester::CallContentScript();
  //DialogModuleTester::CallContentSrpc();
  //DialogModuleTester::SetCaption();

  if (Apollo::isLoadedModule("Net") && Msg_Net_IsOnline::_()) {
    AP_UNITTEST_EXECUTE(DialogModuleTester::ExternalUrl);
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

static ApHandle DialogModuleTester_ContentLoadedFromHtml_hDialog;

void DialogModuleTester_ContentLoadedFromHtml_Dialog_ContentLoaded(Msg_Dialog_ContentLoaded* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ContentLoadedFromHtml_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::ContentLoadedFromHtml);
  { Msg_Dialog_ContentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ContentLoadedFromHtml_Dialog_ContentLoaded, 0); }
  
  // Not a good idea to destroy the view while in body-onload
  { ApAsyncMessage<Msg_Dialog_Destroy> msg; msg->hDialog = DialogModuleTester_ContentLoadedFromHtml_hDialog; msg.Post(); }
}

String DialogModuleTester::ContentLoadedFromHtml()
{
  String s;

  { Msg_Dialog_ContentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ContentLoadedFromHtml_Dialog_ContentLoaded, 0, ApCallbackPosNormal); }

  ApHandle hDialog = Apollo::newHandle();
  DialogModuleTester_ContentLoadedFromHtml_hDialog = hDialog;

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 500;
  msg.nTop = 100;
  msg.nWidth = 300;
  msg.nHeight = 200;
  msg.bVisible = 1;
  msg.sCaption = "ContentLoadedFromHtml Caption";
  msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/ContentLoadedFromHtml.html";
  if (!s) { if (!msg.Request()) { s = "Msg_Dialog_Create failed"; }}

  return s;
}

//----------------------------

static ApHandle DialogModuleTester_CallContentScript_hDialog;

void DialogModuleTester_CallContentScript_Dialog_ContentLoaded(Msg_Dialog_ContentLoaded* pMsg)
{
  if (pMsg->hView != DialogModuleTester_CallContentScript_hDialog) { return; }

  String s;

  if (!s) {
    // document.getElementById('iApContent').contentWindow.eval("SetText('zz')")
    Msg_Dialog_CallScriptFunction msg;
    msg.hDialog = pMsg->hView;
    msg.sFunction = "SetText";
    msg.lArgs.AddLast("CallContentScript Text1");
    msg.lArgs.AddLast("42");
    if (!msg.Request()) {
      s = "Msg_Dialog_CallScriptFunction failed";
    }
  }

  if (!s) {
    String sExpected = "1=CallContentScript Text1 2=42";
    String sResult = Msg_WebView_CallScriptFunction::_(pMsg->hView, "iApContent", "ApEval", "document.getElementById('iText').innerHTML");
    if (sResult != sExpected) {
      s = "Wrong text expected=" + sExpected + " got=" + sResult;
    }
  }

  AP_UNITTEST_RESULT(DialogModuleTester::CallContentScript, s.empty(), s);
  { Msg_Dialog_ContentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_CallContentScript_Dialog_ContentLoaded, 0); }
  { ApAsyncMessage<Msg_Dialog_Destroy> msg; msg->hDialog = pMsg->hView; msg.Post(); }
}

String DialogModuleTester::CallContentScript()
{
  String s;

  { Msg_Dialog_ContentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_CallContentScript_Dialog_ContentLoaded, 0, ApCallbackPosNormal); }

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

void DialogModuleTester_CallContentSrpc_Dialog_ContentLoaded(Msg_Dialog_ContentLoaded* pMsg)
{
  if (pMsg->hView != DialogModuleTester_CallContentSrpc_hDialog) { return; }

  String s;

  if (!s) {
    // document.getElementById('iApContent').contentWindow.eval("SetText('zz')")
    Msg_Dialog_ContentCall msg;
    msg.hDialog = pMsg->hView;
    msg.sFunction = "SetText";
    msg.srpc.set("String", "41");
    msg.srpc.set("Int", 42);
    if (!msg.Request()) {
      s = "Msg_Dialog_ContentCall failed";
    }
  }

  if (!s) {
    String sExpected = "String=41\nInt=42\n";
    String sResult = Msg_WebView_CallScriptFunction::_(pMsg->hView, "iApContent", "ApEval", "document.getElementById('iText').innerHTML");
    if (sResult != sExpected) {
      s = "Wrong text expected=" + sExpected + " got=" + sResult;
    }
  }

  AP_UNITTEST_RESULT(DialogModuleTester::CallContentSrpc, s.empty(), s);

  { Msg_Dialog_ContentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_CallContentSrpc_Dialog_ContentLoaded, 0); }

  { ApAsyncMessage<Msg_Dialog_Destroy> msg; msg->hDialog = DialogModuleTester_CallContentSrpc_hDialog; msg.Post(); }
}

String DialogModuleTester::CallContentSrpc()
{
  String s;

  { Msg_Dialog_ContentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_CallContentSrpc_Dialog_ContentLoaded, 0, ApCallbackPosNormal); }

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

static ApHandle DialogModuleTester_ExternalUrl_hDialog;
static String DialogModuleTester_ExternalUrl_sUrl;

void DialogModuleTester_ExternalUrl_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrl_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrlLoaded);
  { Msg_WebView_Event_DocumentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_DocumentLoaded, 0); }
}

void DialogModuleTester_ExternalUrl_WebView_Event_EmbeddedDocumentLoaded(Msg_WebView_Event_EmbeddedDocumentLoaded* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrl_hDialog) { return; }

  String s;
  if (pMsg->sUrl != DialogModuleTester_ExternalUrl_sUrl) {
    s.appendf("got=%s expected=%s", _sz(pMsg->sUrl), _sz(DialogModuleTester_ExternalUrl_sUrl));
  }

  AP_UNITTEST_RESULT(DialogModuleTester::ExternalUrlEmbeddedDocumentLoaded, s.empty(), s);
  { Msg_WebView_Event_EmbeddedDocumentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_EmbeddedDocumentLoaded, 0); }
}

void DialogModuleTester_ExternalUrl_WebView_Event_EmbeddedDocumentComplete(Msg_WebView_Event_EmbeddedDocumentComplete* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrl_hDialog) { return; }

  if (pMsg->sUrl == DialogModuleTester_ExternalUrl_sUrl) {
    String s;
    String sExpectedSubstring = "background-image";
    String sResult = Msg_WebView_GetElementValue::_(DialogModuleTester_ExternalUrl_hDialog, "iApContent", "#p-logo", "innerHTML");
    if (!sResult.contains(sExpectedSubstring)) {
      s.appendf("got=%s expected=%s", _sz(sResult), _sz(sExpectedSubstring));
    }
    AP_UNITTEST_RESULT(DialogModuleTester::ExternalUrlDomElementProperty, s.empty(), s);
  }

  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrlEmbeddedDocumentComplete);
  { Msg_WebView_Event_EmbeddedDocumentComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_EmbeddedDocumentComplete, 0); }

  { ApAsyncMessage<Msg_Dialog_Destroy> msg; msg->hDialog = pMsg->hView; msg.Post(); }
}

void DialogModuleTester_ExternalUrl_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrl_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrlDomElementProperty);
  { Msg_WebView_Event_DocumentComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_DocumentComplete, 0); }
}

void DialogModuleTester_ExternalUrl_WebView_Event_Closing(Msg_WebView_Event_Closing* pMsg)
{
  if (pMsg->hView != DialogModuleTester_ExternalUrl_hDialog) { return; }
  AP_UNITTEST_SUCCESS(DialogModuleTester::ExternalUrlClosing);
  { Msg_WebView_Event_Closing msg; msg.Unhook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_Closing, 0); }
}

String DialogModuleTester::ExternalUrl()
{
  String s;

  { Msg_WebView_Event_DocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_DocumentLoaded, 0, ApCallbackPosNormal); }
  { Msg_WebView_Event_DocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_DocumentComplete, 0, ApCallbackPosNormal); }
  { Msg_WebView_Event_EmbeddedDocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_EmbeddedDocumentLoaded, 0, ApCallbackPosNormal); }
  { Msg_WebView_Event_EmbeddedDocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_EmbeddedDocumentComplete, 0, ApCallbackPosNormal); }
  { Msg_WebView_Event_Closing msg; msg.Hook(MODULE_NAME, (ApCallback) DialogModuleTester_ExternalUrl_WebView_Event_Closing, 0, ApCallbackPosNormal); }

  ApHandle hDialog = Apollo::newHandle();
  DialogModuleTester_ExternalUrl_hDialog = hDialog;
  DialogModuleTester_ExternalUrl_sUrl = "http://en.wikipedia.org/wiki/Main_Page";

  Msg_Dialog_Create msg;
  msg.hDialog = hDialog;
  msg.nLeft = 220;
  msg.nTop = 300;
  msg.nWidth = 800;
  msg.nHeight = 500;
  msg.bVisible = 1;
  msg.sCaption = "ExternalUrl";
  msg.sContentUrl = DialogModuleTester_ExternalUrl_sUrl;
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
