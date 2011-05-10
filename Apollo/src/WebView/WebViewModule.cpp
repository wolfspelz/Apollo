// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgConfig.h"
#include "Local.h"
#include "WebViewModule.h"

View* WebViewModule::CreateView(const ApHandle& hView)
{
  View* pView = new View(hView);
  if (pView) {
    int ok = pView->Create();
    if (ok) {
      pView->AddRef();
      views_.Set(hView, pView);
    } else {
      delete pView;
      pView = 0;
      throw ApException("WebViewModule::CreateView " ApHandleFormat " Create() failed", ApHandleType(hView));
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
  if (pView == 0) { throw ApException("WebViewModule::FindView no webview=" ApHandleFormat "", ApHandleType(hView)); }

  return pView;
}

//---------------------------

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Create)
{
  if (views_.Find(pMsg->hView) != 0) { throw ApException("WebViewModule::WebView_Create: webview=" ApHandleFormat " already exists", ApHandleType(pMsg->hView)); }
  View* pView = CreateView(pMsg->hView);
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
  pView->SetPosition(pMsg->nX, pMsg->nY, pMsg->nW, pMsg->nH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Visibility)
{
  View* pView = FindView(pMsg->hView);
  pView->SetVisibility(pMsg->bVisible);
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

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_CallScriptFunction)
{
  View* pView = FindView(pMsg->hView);
  pMsg->sResult = pView->CallJSFunction(pMsg->sMethod, pMsg->lArgs);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_SetScriptAccess)
{
  View* pView = FindView(pMsg->hView);
  pView->SetJSAccess(pMsg->sAccess);
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
  pView->SizeBy(pMsg->nW, pMsg->nH, pMsg->nDirection);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_MouseCapture)
{
  View* pView = FindView(pMsg->hView);
  pView->MouseCapture();
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
  pView->GetPosition(pMsg->nX, pMsg->nY, pMsg->nW, pMsg->nH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_GetVisibility)
{
  View* pView = FindView(pMsg->hView);
  pView->GetVisibility(pMsg->bVisible);
  pMsg->apStatus = ApMessage::Ok;
}

int g_nCnt = 0;
AP_MSG_HANDLER_METHOD(WebViewModule, System_3SecTimer)
{
  ViewListNode *pNode = views_.Next(0);
  if (pNode) {
    ApHandle hView = pNode->Key();
    View* pView = pNode->Value();

    if (pView) {
      //if (g_nCnt++ == 3) {
      //  Msg_WebView_Destroy::_(hView);
      //}

      //pView->SetVisibility(g_nCnt++ % 2 == 0 ? 1 : 0);

      //List lArgs;
      //lArgs.AddLast("abc");
      //lArgs.AddLast("def");
      //String s = pView->CallJSFunction("Concat", lArgs);
    }
  }
}

//----------------------------------------------------------

void SrpcGate_WebView_MoveBy(ApSRPCMessage* pMsg)
{
  Msg_WebView_MoveBy msg;
  msg.hView = Apollo::string2Handle(pMsg->srpc.getString("hView"));
  msg.nX =pMsg->srpc.getInt("nX");
  msg.nY =pMsg->srpc.getInt("nY");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_WebView_SizeBy(ApSRPCMessage* pMsg)
{
  Msg_WebView_SizeBy msg;
  msg.hView = Apollo::string2Handle(pMsg->srpc.getString("hView"));
  msg.nW =pMsg->srpc.getInt("nW");
  msg.nH =pMsg->srpc.getInt("nH");
  msg.nDirection =pMsg->srpc.getInt("nDirection");
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_WebView_MouseCapture(ApSRPCMessage* pMsg)
{
  Msg_WebView_MouseCapture msg;
  msg.hView = Apollo::string2Handle(pMsg->srpc.getString("hView"));
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_WebView_MouseRelease(ApSRPCMessage* pMsg)
{
  Msg_WebView_MouseRelease msg;
  msg.hView = Apollo::string2Handle(pMsg->srpc.getString("hView"));
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
  }
}

void SrpcGate_WebView_GetPosition(ApSRPCMessage* pMsg)
{
  Msg_WebView_GetPosition msg;
  msg.hView = Apollo::string2Handle(pMsg->srpc.getString("hView"));
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
    pMsg->response.setInt("nX", msg.nX);
    pMsg->response.setInt("nY", msg.nY);
    pMsg->response.setInt("nW", msg.nW);
    pMsg->response.setInt("nH", msg.nH);
  }
}

void SrpcGate_WebView_GetVisibility(ApSRPCMessage* pMsg)
{
  Msg_WebView_GetVisibility msg;
  msg.hView = Apollo::string2Handle(pMsg->srpc.getString("hView"));
  if (!msg.Request()) {
    pMsg->response.createError(pMsg->srpc, msg.sComment);
  } else {
    pMsg->response.createResponse(pMsg->srpc);
    pMsg->response.setInt("bVisible", msg.bVisible);
  }
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_LoadHtml, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Load, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_CallScriptFunction, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_SetScriptAccess, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_MoveBy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_SizeBy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_MouseCapture, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_MouseRelease, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_GetPosition, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_GetVisibility, this, ApCallbackPosNormal);

  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, System_3SecTimer, this, ApCallbackPosNormal);

  AP_UNITTEST_HOOK(WebViewModule, this);

  srpcGateRegistry_.add("WebView_MoveBy", SrpcGate_WebView_MoveBy);
  srpcGateRegistry_.add("WebView_SizeBy", SrpcGate_WebView_SizeBy);
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

  shutDownWebKit();

#if defined(WIN32)
  ::OleUninitialize();
#endif
}
