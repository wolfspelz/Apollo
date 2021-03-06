// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "WebViewModule.h"

View* WebViewModule::CreateView(const ApHandle& hView, int nLeft, int nTop, int nWidth, int nHeight)
{
  View* pView = new View(hView);
  if (pView) {
    try {
      pView->Create(nLeft, nTop, nWidth, nHeight);
      pView->AddRef();
      views_.Set(hView, pView);
    } catch (ApException& ex) {
      delete pView;
      pView = 0;
      throw ex;
    }
  }

  return pView;
}

void WebViewModule::DeleteView(const ApHandle& hView)
{
  View* pView = FindView(hView);
  if (pView) {
    pView->Destroy();
    views_.Unset(hView);
    int nRefCnt = pView->Release();
    if (nRefCnt > 0) {
      delete pView;
    }
    pView = 0;
  }
}

View* WebViewModule::FindView(const ApHandle& hView)
{
  View* pView = 0;  

  views_.Get(hView, pView);
  if (pView == 0) {
    throw ApException(LOG_CONTEXT, "no view=" ApHandleFormat "", ApHandlePrintf(hView));
  }

  return pView;
}

//---------------------------

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Create)
{
  if (views_.Find(pMsg->hView) != 0) { throw ApException(LOG_CONTEXT, "view=" ApHandleFormat " already exists", ApHandlePrintf(pMsg->hView)); }
  View* pView = CreateView(pMsg->hView, pMsg->nLeft, pMsg->nTop, pMsg->nWidth, pMsg->nHeight);
  bWebKitUsed_ = 1;
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Destroy)
{
  View* pView = FindView(pMsg->hView);
  DeleteView(pMsg->hView); 
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Position)
{
  View* pView = FindView(pMsg->hView);
  pView->SetPosition(pMsg->nLeft, pMsg->nTop, pMsg->nWidth, pMsg->nHeight);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Visibility)
{
  View* pView = FindView(pMsg->hView);
  pView->SetVisibility(pMsg->bVisible);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_SetWindowFlags)
{
  View* pView = FindView(pMsg->hView);
  pView->SetWindowFlags(pMsg->nFlags);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_LoadHtml)
{
  View* pView = FindView(pMsg->hView);
  pView->LoadHtml(pMsg->sHtml, pMsg->sBase);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Load)
{
  View* pView = FindView(pMsg->hView);
  pView->Load(pMsg->sUrl);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Reload)
{
  View* pView = FindView(pMsg->hView);
  pView->Reload();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_CallScriptFunction)
{
  View* pView = FindView(pMsg->hView);
  pMsg->sResult = pView->CallScriptFunction(pMsg->sFrame, pMsg->sFunction, pMsg->lArgs);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_GetElementValue)
{
  View* pView = FindView(pMsg->hView);
  pMsg->sResult = pView->GetElementValue(pMsg->sFrame, pMsg->sElement, pMsg->sProperty);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_ViewCall)
{
  View* pView = FindView(pMsg->hView);
  Apollo::SrpcMessage response;
  pView->CallJsSrpc(pMsg->sFunction, pMsg->srpc, response);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_SetScriptAccessPolicy)
{
  View* pView = FindView(pMsg->hView);
  pView->SetScriptAccessPolicy(pMsg->sPolicy);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_SetNavigationPolicy)
{
  View* pView = FindView(pMsg->hView);
  pView->SetNavigationPolicy(pMsg->sPolicy);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_MoveBy)
{
  View* pView = FindView(pMsg->hView);
  pView->MoveBy(pMsg->nX, pMsg->nY);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_SizeBy)
{
  View* pView = FindView(pMsg->hView);
  pView->SizeBy(pMsg->nX, pMsg->nY, pMsg->nDirection);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_MouseCapture)
{
  View* pView = FindView(pMsg->hView);
  pView->MouseCapture();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_MakeFrontWindow)
{
  View* pView = FindView(pMsg->hView);
  pView->MakeFrontWindow();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_MouseRelease)
{
  View* pView = FindView(pMsg->hView);
  pView->MouseRelease();
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_GetPosition)
{
  View* pView = FindView(pMsg->hView);
  pView->GetPosition(pMsg->nLeft, pMsg->nTop, pMsg->nWidth, pMsg->nHeight);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_GetVisibility)
{
  View* pView = FindView(pMsg->hView);
  pView->GetVisibility(pMsg->bVisible);
  pMsg->apStatus = ApMessage::Ok;
}

#if defined(WIN32)
AP_MSG_HANDLER_METHOD(WebViewModule, WebView_GetWin32Window)
{
  View* pView = FindView(pMsg->hView);
  pView->GetWin32Window(pMsg->hWnd);
  pMsg->apStatus = ApMessage::Ok;
}
#endif // defined(WIN32)

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Event_DocumentLoaded)
{
  View::LoadDone();
  View::TryLoad();
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Event_LoadError)
{
  View::LoadDone();
  View::TryLoad();
}

AP_MSG_HANDLER_METHOD(WebViewModule, System_3SecTimer)
{
  View::TryLoad();
}


//----------------------------------------------------------

void SrpcGate_WebView_Create(ApSRPCMessage* pMsg)
{
  Msg_WebView_Create msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  msg.nLeft = pMsg->srpc.getInt("nLeft");
  msg.nTop = pMsg->srpc.getInt("nTop");
  msg.nWidth = pMsg->srpc.getInt("nWidth");
  msg.nHeight = pMsg->srpc.getInt("nHeight");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_Destroy(ApSRPCMessage* pMsg)
{
  Msg_WebView_Destroy msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_Position(ApSRPCMessage* pMsg)
{
  Msg_WebView_Position msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  msg.nLeft = pMsg->srpc.getInt("nLeft");
  msg.nTop = pMsg->srpc.getInt("nTop");
  msg.nWidth = pMsg->srpc.getInt("nWidth");
  msg.nHeight = pMsg->srpc.getInt("nHeight");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_Visibility(ApSRPCMessage* pMsg)
{
  Msg_WebView_Visibility msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  msg.bVisible = pMsg->srpc.getInt("bVisible");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_SetWindowFlags(ApSRPCMessage* pMsg)
{
  Msg_WebView_SetWindowFlags msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  msg.nFlags = pMsg->srpc.getInt("nFlags");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_Load(ApSRPCMessage* pMsg)
{
  Msg_WebView_Load msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  msg.sUrl = pMsg->srpc.getString("sUrl");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_LoadHtml(ApSRPCMessage* pMsg)
{
  Msg_WebView_LoadHtml msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  msg.sHtml = pMsg->srpc.getString("sHtml");
  msg.sBase = pMsg->srpc.getString("sBase");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_Reload(ApSRPCMessage* pMsg)
{
  Msg_WebView_Reload msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_MoveBy(ApSRPCMessage* pMsg)
{
  Msg_WebView_MoveBy msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  msg.nX = pMsg->srpc.getInt("nX");
  msg.nY = pMsg->srpc.getInt("nY");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_SizeBy(ApSRPCMessage* pMsg)
{
  Msg_WebView_SizeBy msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  msg.nX = pMsg->srpc.getInt("nX");
  msg.nY = pMsg->srpc.getInt("nY");
  msg.nDirection = pMsg->srpc.getInt("nDirection");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_MouseCapture(ApSRPCMessage* pMsg)
{
  Msg_WebView_MouseCapture msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_MakeFrontWindow(ApSRPCMessage* pMsg)
{
  Msg_WebView_MakeFrontWindow msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_MouseRelease(ApSRPCMessage* pMsg)
{
  Msg_WebView_MouseRelease msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
}

void SrpcGate_WebView_GetPosition(ApSRPCMessage* pMsg)
{
  Msg_WebView_GetPosition msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->response.set("nLeft", msg.nLeft);
  pMsg->response.set("nTop", msg.nTop);
  pMsg->response.set("nWidth", msg.nWidth);
  pMsg->response.set("nHeight", msg.nHeight);
}

void SrpcGate_WebView_GetVisibility(ApSRPCMessage* pMsg)
{
  Msg_WebView_GetVisibility msg;
  msg.hView = pMsg->srpc.getHandle("hView");
  SRPCGATE_HANDLER_NATIVE_REQUEST(pMsg, msg);
  pMsg->response.set("bVisible", msg.bVisible);
}

//----------------------------------------------------------

#if defined(AP_TEST)

#include "WebViewModuleTester.h"

AP_MSG_HANDLER_METHOD(WebViewModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/WebView", 0)) {
    WebViewModuleTester::Begin();
  }
}

AP_MSG_HANDLER_METHOD(WebViewModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/WebView", 0)) {
    WebViewModuleTester::Execute();
  }
}

AP_MSG_HANDLER_METHOD(WebViewModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/WebView", 0)) {
    WebViewModuleTester::End();
  }
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

int WebViewModule::Init()
{
  int ok = 1;

#if defined(WIN32)
  ::OleInitialize(NULL);
#endif

  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Position, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Visibility, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_SetWindowFlags, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_LoadHtml, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Load, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Reload, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_CallScriptFunction, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_GetElementValue, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_ViewCall, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_SetScriptAccessPolicy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_SetNavigationPolicy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_MoveBy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_SizeBy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_MakeFrontWindow, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_MouseCapture, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_MouseRelease, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_GetPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_GetVisibility, this, ApCallbackPosNormal);
  #if defined(WIN32)
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_GetWin32Window, this, ApCallbackPosNormal);
  #endif // defined(WIN32)
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Event_DocumentLoaded, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Event_LoadError, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, System_3SecTimer, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(WebViewModule, this);

  srpcGateRegistry_.add("WebView_Create", SrpcGate_WebView_Create);
  srpcGateRegistry_.add("WebView_Destroy", SrpcGate_WebView_Destroy);
  srpcGateRegistry_.add("WebView_Position", SrpcGate_WebView_Position);
  srpcGateRegistry_.add("WebView_Visibility", SrpcGate_WebView_Visibility);
  srpcGateRegistry_.add("WebView_SetWindowFlags", SrpcGate_WebView_SetWindowFlags);
  srpcGateRegistry_.add("WebView_Load", SrpcGate_WebView_Load);
  srpcGateRegistry_.add("WebView_LoadHtml", SrpcGate_WebView_LoadHtml);
  srpcGateRegistry_.add("WebView_Reload", SrpcGate_WebView_Reload);
  srpcGateRegistry_.add("WebView_MoveBy", SrpcGate_WebView_MoveBy);
  srpcGateRegistry_.add("WebView_SizeBy", SrpcGate_WebView_SizeBy);
  srpcGateRegistry_.add("WebView_MakeFrontWindow", SrpcGate_WebView_MakeFrontWindow);
  srpcGateRegistry_.add("WebView_MouseCapture", SrpcGate_WebView_MouseCapture);
  srpcGateRegistry_.add("WebView_MouseRelease", SrpcGate_WebView_MouseRelease);
  srpcGateRegistry_.add("WebView_GetPosition", SrpcGate_WebView_GetPosition);
  srpcGateRegistry_.add("WebView_GetVisibility", SrpcGate_WebView_GetVisibility);

  return ok;
}

void WebViewModule::Exit()
{
  srpcGateRegistry_.finish();
  AP_UNITTEST_UNHOOK(WebViewModule, this);
  AP_MSG_REGISTRY_FINISH;

  if (bWebKitUsed_) {
    // Otherwise this crashes in iconDatabase() because mainThreadIdentifier was never initialized and hence isMainThread() fails
    shutDownWebKit();
  }

#if defined(WIN32)
  ::OleUninitialize();
#endif
}
