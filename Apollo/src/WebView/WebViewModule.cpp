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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, System_3SecTimer, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(WebViewModule, this);

  return ok;
}

void WebViewModule::Exit()
{
  AP_UNITTEST_UNHOOK(WebViewModule, this);
  AP_MSG_REGISTRY_FINISH;

  shutDownWebKit();

#if defined(WIN32)
  ::OleUninitialize();
#endif
}
