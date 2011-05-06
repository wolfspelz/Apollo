// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#if defined(WIN32)
  #include "MsgMainLoop.h"
#endif // WIN32
#include "Local.h"
#include "WebView.h"
#include "SAutoPtr.h"
#include <JavaScriptCore/JavaScriptCore.h>

WebView::WebView(const ApHandle& hWebView)
:hAp_(hWebView)
,bVisible_(0)
,nX_(100)
,nY_(100)
,nW_(600)
,nH_(400)
,pWebKit_(0)
,pWebFrame_(0)
#if defined(WIN32)
,hWnd_(NULL)
#endif // WIN32
,nRefCount_(0)
{
}

WebView::~WebView()
{
}

#include <shlwapi.h>
#include <wininet.h>

int WebView::Create(const String& sHtml, const String& sBase)
{
  int ok = 0;

#if defined(WIN32)
  IWebViewPrivate* webViewPrivate = 0;
  IWebMutableURLRequest* request = 0;
  IWebPreferences* tmpPreferences = 0;
  IWebPreferences* standardPreferences = 0;
  if (FAILED(WebKitCreateInstance(CLSID_WebPreferences, 0, IID_IWebPreferences, reinterpret_cast<void**>(&tmpPreferences))))
    goto exit;

  if (FAILED(tmpPreferences->standardPreferences(&standardPreferences)))
    goto exit;

  standardPreferences->setAcceleratedCompositingEnabled(TRUE);

  HRESULT hr = WebKitCreateInstance(CLSID_WebView, 0, IID_IWebView, reinterpret_cast<void**>(&pWebKit_));
  if (FAILED(hr))
    goto exit;

  hr = pWebKit_->setUIDelegate(this);
  if (FAILED(hr))
    goto exit;

  hr = pWebKit_->setFrameLoadDelegate(this);
  if (FAILED(hr))
    goto exit;

  hr = pWebKit_->QueryInterface(IID_IWebViewPrivate, reinterpret_cast<void**>(&webViewPrivate));
  if (FAILED(hr))
    goto exit;

  {
    RECT r = { nX_, nY_, nW_, nH_ };
    hr = pWebKit_->initWithFrame(r, 0, 0);
    if (FAILED(hr))
      goto exit;
  }

  // -----------------------------

  hr = webViewPrivate->setTransparent(TRUE);
  if (FAILED(hr))
    goto exit;

  hr = webViewPrivate->setUsesLayeredWindow(TRUE);
  if (FAILED(hr))
    goto exit;

  hr = webViewPrivate->viewWindow(reinterpret_cast<OLE_HANDLE*>(&hWnd_));
  if (FAILED(hr) || !hWnd_)
    goto exit;

  ::ShowWindow(hWnd_, SW_SHOW);
  ::UpdateWindow(hWnd_);

  // Put our two partner windows (the dialog and the HUD) in proper Z-order
  //::SetWindowPos(m_dialog, hWnd_, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

  // -----------------------------

  hr = pWebKit_->mainFrame(&pWebFrame_);
  if (FAILED(hr))
    goto exit;

#if 1
  static BSTR defaultHTML = SysAllocString(sHtml);
  pWebFrame_->loadHTMLString(defaultHTML, SysAllocString(sBase));
#else
#if 0
  static BSTR defaultHTML = SysAllocString(TEXT("<p style=\"background-color: #00FF00\">Testing</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul><iframe src=\"http://www.wolfspelz.de\" style=\"width:100%;height:300px\" />"));
  pWebFrame_->loadHTMLString(defaultHTML, 0);
#else
  static BSTR urlBStr = 0;
  //static BSTR urlBStr = SysAllocString(_T("http://www.wolfspelz.de"));
  if (!urlBStr) {
    TCHAR szFileName[MAX_PATH];
    ::GetModuleFileName(NULL, szFileName, MAX_PATH);
    ::PathRemoveFileSpec(szFileName);
    ::PathAddBackslash(szFileName);
    //::PathAppend(szFileName, _T("frame.html"));
    ::PathAppend(szFileName, _T("overlay.html"));

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

  hr = pWebFrame_->loadRequest(request);
  if (FAILED(hr))
    goto exit;

  if (request)
    request->Release();
#endif
#endif

  ok = 1;
  ::MoveWindow(hWnd_, nX_, nY_ - 10000, nW_, nH_, TRUE);

exit:
  if (standardPreferences)
    standardPreferences->Release();

  if (tmpPreferences)
    tmpPreferences->Release();

  if (webViewPrivate)
    webViewPrivate->Release();
#endif // WIN32

  return ok;
}

void WebView::Destroy()
{
#if defined(WIN32)
  if (pWebFrame_) {
    pWebFrame_->Release();
    pWebFrame_ = 0;
  }
  if (pWebKit_) {
    pWebKit_->Release();
    pWebKit_ = 0;
  }
  if (hWnd_ != NULL) {
    ::DestroyWindow(hWnd_);
    hWnd_ = NULL;
  }
#endif // WIN32
}

//------------------------------------

void WebView::SetPosition(int nX, int nY, int nW, int nH)
{
  nX_ = nX;
  nY_ = nY;
  nW_ = nW;
  nH_ = nH;

#if defined(WIN32)
  ::MoveWindow(hWnd_, nX_, nY_ - (bVisible_ ? 0 : 10000), nW_, nH_, TRUE);
#endif // WIN32
}

void WebView::SetVisibility(int bVisible)
{
  int bChanged = (bVisible_ != bVisible);
  bVisible_ = bVisible;

  if (bChanged) {
#if defined(WIN32)
    ::MoveWindow(hWnd_, nX_, nY_ - (bVisible_ ? 0 : 10000), nW_, nH_, TRUE);

    //::ShowWindow(hWnd_, bVisible_ ? SW_SHOW : SW_HIDE);
    //if (bVisible_) {
    //  ::UpdateWindow(hWnd_);
    //}
#endif // WIN32
  }
}

//------------------------------------

String WebView::CallJSFunction(const String& sFunction, List& lArgs)
{
  String sResult;

  JSGlobalContextRef runCtx = pWebFrame_->globalContext();

  static JSStringRef methodName = 0;
  if (!methodName)
    methodName = JSStringCreateWithUTF8CString(sFunction);

  JSObjectRef global = JSContextGetGlobalObject(runCtx);

  JSValueRef* exception = 0;
  JSValueRef sampleFunction = JSObjectGetProperty(runCtx, global, methodName, exception);
  if (exception)
    goto exit;

  if (!JSValueIsObject(runCtx, sampleFunction))
    goto exit;

  JSObjectRef function = JSValueToObject(runCtx, sampleFunction, exception);
  if (exception)
    goto exit;

  JSValueRef result = 0;

  if (lArgs.length() > 0) {
    AutoPtr<JSValueRef> pJSArgs(new JSValueRef[lArgs.length()]);
    int nCnt = 0;
    for (Elem* e = 0; (e = lArgs.Next(e)) != 0; ) {
      pJSArgs[nCnt++] = JSValueMakeString (runCtx, JSStringCreateWithCharacters((LPCTSTR) e->getName(), e->getName().chars()));
    }
    result = JSObjectCallAsFunction (runCtx, function, global, lArgs.length(), pJSArgs.get(), exception);
  } else {
    result = JSObjectCallAsFunction (runCtx, function, global, 0, 0, exception);
  }
  if (exception)
    goto exit;

  // Convert the result into a string.
  if (JSValueIsString(runCtx, result))
  {
    JSStringRef temp = JSValueToStringCopy (runCtx, result, exception);
    if (exception)
      goto exit;

    sResult.set((PWSTR) JSStringGetCharactersPtr(temp), JSStringGetLength(temp));
    JSStringRelease(temp);
  }
  else if (JSValueIsNumber(runCtx, result))
  {
    double temp = JSValueToNumber(runCtx, result, exception);
    if (exception)
      goto exit;

    sResult.appendf("%g", temp);
  }
  else if (JSValueIsBoolean(runCtx, result))
  {
    bool temp = JSValueToBoolean(runCtx, result);
    if (exception)
      goto exit;

    sResult = temp ? "true" : "false";
  }

exit:
  return sResult;
}

//------------------------------------

HRESULT WebView::QueryInterface(REFIID riid, void** ppvObject)
{
    *ppvObject = 0;
    if (IsEqualIID(riid, IID_IUnknown))
        *ppvObject = static_cast<IWebUIDelegate*>(this);
    else if (IsEqualIID(riid, IID_IWebUIDelegate))
        *ppvObject = static_cast<IWebUIDelegate*>(this);
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

ULONG WebView::AddRef(void)
{
    return ++nRefCount_;
}

ULONG WebView::Release(void)
{
    ULONG newRef = --nRefCount_;
    if (!newRef)
        delete this;

    return newRef;
}

