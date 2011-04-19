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

// Replace regex: 
// \(Msg_[a-zA-Z]+_[^ *]+\* pMsg\)

//AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Get)
//{
//  String sSomeConfigValue;
//  Msg_Config_GetValue msg;
//  msg.sPath = "path";
//  msg.Request();
//  sSomeConfigValue = msg.sValue;
//
//  sSomeConfigValue = Apollo::getConfig("path", "default");
//
//  pMsg->nValue = nTheAnswer_;
//  pMsg->apStatus = ApMessage::Ok;
//}

//----------------------------------------------------------

#if defined(AP_TEST)

AP_MSG_HANDLER_METHOD(WebViewModule, UnitTest_Begin)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/WebView", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(WebViewModule, UnitTest_Execute)
{
  AP_UNUSED_ARG(pMsg);
  if (Apollo::getConfig("Test/WebView", 0)) {
  }
}

AP_MSG_HANDLER_METHOD(WebViewModule, UnitTest_End)
{
  AP_UNUSED_ARG(pMsg);
}

#endif // #if defined(AP_TEST)

//----------------------------------------------------------

#include <WebKit/WebKit.h>
#include <WebKit/WebKitCOMAPI.h>

IWebView* gWebView = 0;
HWND gViewWindow = NULL;

static void loadURL(BSTR urlBStr)
{
    IWebFrame* frame = 0;
    IWebMutableURLRequest* request = 0;

    static BSTR methodBStr = SysAllocString(TEXT("GET"));

    HRESULT hr = gWebView->mainFrame(&frame);
    if (FAILED(hr))
        goto exit;

    hr = WebKitCreateInstance(CLSID_WebMutableURLRequest, 0, IID_IWebMutableURLRequest, (void**)&request);
    if (FAILED(hr))
        goto exit;

    hr = request->initWithURL(urlBStr, WebURLRequestUseProtocolCachePolicy, 60);
    if (FAILED(hr))
        goto exit;

    hr = request->setHTTPMethod(methodBStr);
    if (FAILED(hr))
        goto exit;

    hr = frame->loadRequest(request);
    if (FAILED(hr))
        goto exit;

    SetFocus(gViewWindow);

exit:
    if (frame)
        frame->Release();
    if (request)
        request->Release();
}

int WebViewModule::init()
{
  int ok = 1;

#if defined(WIN32)
  ::OleInitialize(NULL);
#endif

  if (ok) {
    if (FAILED( WebKitCreateInstance(CLSID_WebView, 0, IID_IWebView, (void**)&gWebView) )) { ok = 0; }
  }

  if (ok) {
    RECT clientRect = { 200, 200, 800, 600 };
    if (FAILED( gWebView->initTransparentViewWithFrame(clientRect, 0, 0) )) { ok = 0; }
  }

  IWebFrame* frame = 0;
  if (ok) {
    if (FAILED( gWebView->mainFrame(&frame) )) { ok = 0; }
  }

  if (ok) {
    static BSTR defaultHTML = SysAllocString(TEXT("<p style=\"background-color: #00FF00\">Testing</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul>"));
    frame->loadHTMLString(defaultHTML, 0);
    frame->Release();
  }

  IWebViewPrivate* webViewPrivate = 0;
  if (ok) {
    if (FAILED( gWebView->QueryInterface(IID_IWebViewPrivate, (void**)&webViewPrivate) )) { ok = 0; }
  }

  if (ok) {
    if (FAILED( webViewPrivate->setTransparent(TRUE ))) { ok = 0; }
  }

  if (ok) {
    if (FAILED( webViewPrivate->viewWindow((OLE_HANDLE*) &gViewWindow) )) { ok = 0; }
    webViewPrivate->Release();
  }

  if (ok) {
#define URL "http://wolfspelz.de"
    BSTR bstr = SysAllocStringLen(_T(URL), _tcslen(_T(URL)));
    loadURL(bstr);
    SysFreeString(bstr);
  }

  //AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Get, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(WebViewModule, this);

  return ok;
}

void WebViewModule::exit()
{
  AP_UNITTEST_UNHOOK(WebViewModule, this);
  AP_MSG_REGISTRY_FINISH;

  if (gWebView != 0) {
    gWebView->Release();
  }

  shutDownWebKit();

#if defined(WIN32)
  ::OleUninitialize();
#endif
}
