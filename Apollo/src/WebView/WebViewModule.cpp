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

WebView* WebViewModule::CreateWebView(const ApHandle& hWebView)
{
  WebView* pWebView = new WebView(hWebView);
  if (pWebView) {
    int ok = pWebView->Create();
    if (ok) {
      pWebView->AddRef();
      webviews_.Set(hWebView, pWebView);
    } else {
      delete pWebView;
      pWebView = 0;
      throw ApException("WebViewModule::CreateWebView " ApHandleFormat " Create() failed", ApHandleType(hWebView));
    }
  }

  return pWebView;
}

void WebViewModule::DeleteWebView(const ApHandle& hWebView)
{
  WebView* pWebView = FindWebView(hWebView);
  if (pWebView) {
    pWebView->Destroy();
    webviews_.Unset(hWebView);
    int nRefCnt = pWebView->Release();
    if (nRefCnt > 0) {
      delete pWebView;
    }
    pWebView = 0;
  }
}

WebView* WebViewModule::FindWebView(const ApHandle& hWebView)
{
  WebView* pWebView = 0;  

  webviews_.Get(hWebView, pWebView);
  if (pWebView == 0) { throw ApException("WebViewModule::FindWebView no webview=" ApHandleFormat "", ApHandleType(hWebView)); }

  return pWebView;
}

//---------------------------

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Create)
{
  if (webviews_.Find(pMsg->hWebView) != 0) { throw ApException("WebViewModule::WebView_Create: webview=" ApHandleFormat " already exists", ApHandleType(pMsg->hWebView)); }
  WebView* pWebView = CreateWebView(pMsg->hWebView);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Destroy)
{
  WebView* pWebView = FindWebView(pMsg->hWebView);
  DeleteWebView(pMsg->hWebView); 
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Position)
{
  WebView* pWebView = FindWebView(pMsg->hWebView);
  pWebView->SetPosition(pMsg->nX, pMsg->nY, pMsg->nW, pMsg->nH);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Visibility)
{
  WebView* pWebView = FindWebView(pMsg->hWebView);
  pWebView->SetVisibility(pMsg->bVisible);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_LoadHtml)
{
  WebView* pWebView = FindWebView(pMsg->hWebView);
  pWebView->LoadHtml(pMsg->sHtml, pMsg->sBase);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Load)
{
  WebView* pWebView = FindWebView(pMsg->hWebView);
  pWebView->Load(pMsg->sUrl);
  pMsg->apStatus = ApMessage::Ok;
}

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_CallJavaScriptFunction)
{
  WebView* pWebView = FindWebView(pMsg->hWebView);
  pMsg->sResult = pWebView->CallJSFunction(pMsg->sMethod, pMsg->lArgs);
  pMsg->apStatus = ApMessage::Ok;
}

int g_nCnt = 0;
AP_MSG_HANDLER_METHOD(WebViewModule, System_3SecTimer)
{
  WebViewListNode *pNode = webviews_.Next(0);
  if (pNode) {
    ApHandle hWebView = pNode->Key();
    WebView* pWebView = pNode->Value();

    if (pWebView) {
      //if (g_nCnt++ == 3) {
      //  Msg_WebView_Destroy::_(hWebView);
      //}

      //pWebView->SetVisibility(g_nCnt++ % 2 == 0 ? 1 : 0);

      //List lArgs;
      //lArgs.AddLast("abc");
      //lArgs.AddLast("def");
      //String s = pWebView->CallJSFunction("safsdfad", lArgs);
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
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_CallJavaScriptFunction, this, ApCallbackPosNormal);
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
