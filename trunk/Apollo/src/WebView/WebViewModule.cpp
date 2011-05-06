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

WebView* WebViewModule::CreateWebView(const ApHandle& hWebView, const String& sHtml, const String& sBase)
{
  WebView* pWebView = new WebView(hWebView);
  if (pWebView) {
    int ok = pWebView->Create(sHtml, sBase);
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
  if (pWebView == 0) { throw ApException("WebViewModule::FindWebView no scene=" ApHandleFormat "", ApHandleType(hWebView)); }

  return pWebView;
}

//---------------------------

AP_MSG_HANDLER_METHOD(WebViewModule, WebView_Create)
{
  if (webviews_.Find(pMsg->hWebView) != 0) { throw ApException("WebViewModule::WebView_Create: scene=" ApHandleFormat " already exists", ApHandleType(pMsg->hWebView)); }
  WebView* pWebView = CreateWebView(pMsg->hWebView, pMsg->sHtml, pMsg->sBase);
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

      List lArgs;
      lArgs.AddLast("abc");
      lArgs.AddLast("def");
      String s = pWebView->CallJSFunction("safsdfad", lArgs);
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

#include <shlwapi.h>
#include <wininet.h>

IWebView* gWebView = 0;
HWND gViewWindow = NULL;

void CreateHUD()
{
  RECT clientRect = { -10000, 100, 800, 800 };

  IWebViewPrivate* webViewPrivate = 0;
  IWebMutableURLRequest* request = 0;
  IWebPreferences* tmpPreferences = 0;
  IWebPreferences* standardPreferences = 0;
  if (FAILED(WebKitCreateInstance(CLSID_WebPreferences, 0, IID_IWebPreferences, reinterpret_cast<void**>(&tmpPreferences))))
    goto exit;

  if (FAILED(tmpPreferences->standardPreferences(&standardPreferences)))
    goto exit;

  standardPreferences->setAcceleratedCompositingEnabled(TRUE);

  HRESULT hr = WebKitCreateInstance(CLSID_WebView, 0, IID_IWebView, reinterpret_cast<void**>(&gWebView));
  if (FAILED(hr))
    goto exit;

  hr = gWebView->QueryInterface(IID_IWebViewPrivate, reinterpret_cast<void**>(&webViewPrivate));
  if (FAILED(hr))
    goto exit;

  hr = gWebView->initWithFrame(clientRect, 0, 0);
  if (FAILED(hr))
    goto exit;

  IWebFrame* frame;
  hr = gWebView->mainFrame(&frame);
  if (FAILED(hr))
    goto exit;

#if 0
  static BSTR defaultHTML = SysAllocString(TEXT("<p style=\"background-color: #00FF00\">Testing</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul><iframe src=\"http://www.wolfspelz.de\" style=\"width:100%;height:300px\" />"));
  frame->loadHTMLString(defaultHTML, 0);
  frame->Release();
#else
  //static BSTR urlBStr = 0;
  static BSTR urlBStr = SysAllocString(_T("http://www.wolfspelz.de"));
  if (!urlBStr) {
    TCHAR szFileName[MAX_PATH];
    ::GetModuleFileName(NULL, szFileName, MAX_PATH);
    ::PathRemoveFileSpec(szFileName);
    ::PathAddBackslash(szFileName);
    ::PathAppend(szFileName, _T("frame.html"));

    TCHAR szFileName2[MAX_PATH];
    _stprintf_s(szFileName2, MAX_PATH, _T("file://%s"), szFileName);
    urlBStr = SysAllocString(szFileName2);
  }

  if ((PathFileExists(urlBStr) || PathIsUNC(urlBStr))) {
    TCHAR fileURL[INTERNET_MAX_URL_LENGTH];
    DWORD fileURLLength = sizeof(fileURL)/sizeof(fileURL[0]);
    if (SUCCEEDED(UrlCreateFromPath(urlBStr, fileURL, &fileURLLength, 0)))
      urlBStr = fileURL;
  }

  if (FAILED(WebKitCreateInstance(CLSID_WebMutableURLRequest, 0, IID_IWebMutableURLRequest, (void**)&request)))
    goto exit;

  hr = request->initWithURL(urlBStr, WebURLRequestUseProtocolCachePolicy, 60);
  if (FAILED(hr))
    goto exit;

  hr = frame->loadRequest(request);
  if (FAILED(hr))
    goto exit;

  if (request)
    request->Release();

  frame->Release();
#endif

  hr = webViewPrivate->setTransparent(TRUE);
  if (FAILED(hr))
    goto exit;

  hr = webViewPrivate->setUsesLayeredWindow(TRUE);
  if (FAILED(hr))
    goto exit;

  hr = webViewPrivate->viewWindow(reinterpret_cast<OLE_HANDLE*>(&gViewWindow));
  if (FAILED(hr) || !gViewWindow)
    goto exit;

  ::ShowWindow(gViewWindow, SW_SHOW);
  ::UpdateWindow(gViewWindow);

  // Put our two partner windows (the dialog and the HUD) in proper Z-order
  //::SetWindowPos(m_dialog, gViewWindow, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

exit:
  if (standardPreferences)
    standardPreferences->Release();

  if (tmpPreferences)
    tmpPreferences->Release();

  if (webViewPrivate)
    webViewPrivate->Release();
}

int WebViewModule::Init()
{
  int ok = 1;

#if defined(WIN32)
  ::OleInitialize(NULL);
#endif

  //CreateHUD();

  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Create, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Destroy, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Position, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, WebView_Visibility, this, ApCallbackPosNormal);
  AP_MSG_REGISTRY_ADD(MODULE_NAME, WebViewModule, System_3SecTimer, this, ApCallbackPosNormal);
  AP_UNITTEST_HOOK(WebViewModule, this);

  return ok;
}

void WebViewModule::Exit()
{
  AP_UNITTEST_UNHOOK(WebViewModule, this);
  AP_MSG_REGISTRY_FINISH;

  //if (gWebView != 0) {
  //  gWebView->Release();
  //}

  if (gWebView)
      gWebView->Release();

  shutDownWebKit();

#if defined(WIN32)
  ::OleUninitialize();
#endif
}
