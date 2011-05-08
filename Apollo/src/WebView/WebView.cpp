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

WebView::~WebView()
{
}

int WebView::Create()
{
  int ok = 0;

#if defined(WIN32)
  HRESULT hr = S_OK;

  IWebPreferences* tmpPreferences = 0;
  IWebPreferences* standardPreferences = 0;
  if (FAILED(WebKitCreateInstance(CLSID_WebPreferences, 0, IID_IWebPreferences, reinterpret_cast<void**>(&tmpPreferences)))) goto exit;

  if (FAILED(tmpPreferences->standardPreferences(&standardPreferences))) goto exit;

  standardPreferences->setAcceleratedCompositingEnabled(TRUE);

  hr = WebKitCreateInstance(CLSID_WebView, 0, IID_IWebView, reinterpret_cast<void**>(&pWebView_));
  if (FAILED(hr)) goto exit;

  hr = pWebView_->setUIDelegate(this);
  if (FAILED(hr)) goto exit;

  hr = pWebView_->setFrameLoadDelegate(this);
  if (FAILED(hr)) goto exit;

  hr = pWebView_->QueryInterface(IID_IWebViewPrivate, reinterpret_cast<void**>(&pWebViewPrivate_));
  if (FAILED(hr)) goto exit;

  {
    RECT r = { nX_, nY_, nW_, nH_ };
    hr = pWebView_->initWithFrame(r, 0, 0);
    if (FAILED(hr)) goto exit;
  }

  // -----------------------------

  hr = pWebViewPrivate_->setFrameLoadDelegatePrivate(this);
  if (FAILED(hr)) goto exit;

  hr = pWebViewPrivate_->setTransparent(TRUE);
  if (FAILED(hr)) goto exit;

  hr = pWebViewPrivate_->setUsesLayeredWindow(TRUE);
  if (FAILED(hr)) goto exit;

  hr = pWebViewPrivate_->viewWindow(reinterpret_cast<OLE_HANDLE*>(&hWnd_));
  if (FAILED(hr) || !hWnd_) goto exit;

  ::ShowWindow(hWnd_, SW_SHOW);
  ::UpdateWindow(hWnd_);

  // -----------------------------

  hr = pWebView_->mainFrame(&pWebFrame_);
  if (FAILED(hr)) goto exit;

  ::MoveWindow(hWnd_, nX_, nY_ - 10000, nW_, nH_, TRUE);
  ok = 1;

exit:
  if (standardPreferences) standardPreferences->Release();
  if (tmpPreferences) tmpPreferences->Release();

#endif // WIN32

  return ok;
}

void WebView::Destroy()
{
#if defined(WIN32)
  if (pWebViewPrivate_) {
    pWebViewPrivate_->Release();
    pWebViewPrivate_ = 0;
  }

  if (pWebFrame_) {
    pWebFrame_->Release();
    pWebFrame_ = 0;
  }

  if (pWebView_) {
    pWebView_->Release();
    pWebView_ = 0;
  }

  if (hWnd_ != NULL) {
    ::DestroyWindow(hWnd_);
    hWnd_ = NULL;
  }
#endif // WIN32
}

int WebView::LoadHtml(const String& sHtml, const String& sBase)
{
  int ok = 0;

#if defined(WIN32)
  IWebMutableURLRequest* request = 0;

#if 1
  pWebFrame_->loadHTMLString(::SysAllocString(sHtml), ::SysAllocString(sBase));
#else
  BSTR bstrHTML = SysAllocString(TEXT("<p style=\"background-color: #00FF00\">Testing</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul><iframe src=\"http://www.wolfspelz.de\" style=\"width:100%;height:300px\" />"));
  pWebFrame_->loadHTMLString(bstrHTML, 0);
#endif

  ok = 1;

exit:;
#endif // WIN32

  return ok;
}

#include <shlwapi.h>
#include <wininet.h>

int WebView::Load(const String& sUrl)
{
  int ok = 0;

#if defined(WIN32)
  HRESULT hr = S_OK;

  IWebMutableURLRequest* request = 0;
  hr = WebKitCreateInstance(CLSID_WebMutableURLRequest, 0, IID_IWebMutableURLRequest, (void**) &request);
  if (FAILED(hr)) goto exit;

#if 1
  hr = request->initWithURL(::SysAllocString(sUrl), WebURLRequestUseProtocolCachePolicy, 60);
  if (FAILED(hr)) goto exit;
#else
  BSTR bstrUrl = 0;
  //BSTR bstrUrl = SysAllocString(_T("http://www.wolfspelz.de"));
  if (!bstrUrl) {
    TCHAR szFileName[MAX_PATH];
    ::GetModuleFileName(NULL, szFileName, MAX_PATH);
    ::PathRemoveFileSpec(szFileName);
    ::PathAddBackslash(szFileName);
    //::PathAppend(szFileName, _T("frame.html"));
    ::PathAppend(szFileName, _T("overlay.html"));

    TCHAR szFileName2[MAX_PATH];
    _stprintf_s(szFileName2, MAX_PATH, _T("file://%s"), szFileName);
    bstrUrl = SysAllocString(szFileName2);
  }

  if ((PathFileExists(bstrUrl) || PathIsUNC(bstrUrl))) {
    TCHAR fileURL[INTERNET_MAX_URL_LENGTH];
    DWORD fileURLLength = sizeof(fileURL)/sizeof(fileURL[0]);
    if (SUCCEEDED(UrlCreateFromPath(bstrUrl, fileURL, &fileURLLength, 0))) {
      bstrUrl = fileURL;
    }
  }

  hr = request->initWithURL(bstrUrl, WebURLRequestUseProtocolCachePolicy, 60);
  if (FAILED(hr)) goto exit;
#endif

  //hr = pWebFrame_->setAllowsScrolling(FALSE);
  //if (FAILED(hr)) goto exit;

  hr = pWebFrame_->loadRequest(request);
  if (FAILED(hr)) goto exit;

  ok = 1;

exit:
  if (request) request->Release();
#endif // WIN32

  return ok;
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
// Call JS

String WebView::CallJSFunction(const String& sFunction, List& lArgs)
{
  String sResult;

  JSGlobalContextRef runCtx = pWebFrame_->globalContext();

  static JSStringRef methodName = 0;
  if (!methodName) methodName = JSStringCreateWithUTF8CString(sFunction);

  JSObjectRef global = JSContextGetGlobalObject(runCtx);

  JSValueRef* exception = 0;
  JSValueRef sampleFunction = JSObjectGetProperty(runCtx, global, methodName, exception);
  if (exception) goto exit;

  if (!JSValueIsObject(runCtx, sampleFunction)) goto exit;

  JSObjectRef function = JSValueToObject(runCtx, sampleFunction, exception);
  if (exception) goto exit;

  JSValueRef result = 0;

  if (lArgs.length() > 0) {
    AutoPtr<JSValueRef> pJSArgs(new JSValueRef[lArgs.length()]);
    int nCnt = 0;
    for (Elem* e = 0; (e = lArgs.Next(e)) != 0; ) {
      pJSArgs[nCnt++] = JSValueMakeString (runCtx, JSStringCreateWithCharacters((LPCTSTR) e->getName(), e->getName().chars()));
    }
    result = JSObjectCallAsFunction (runCtx, function, global, lArgs.length(), pJSArgs.get(), exception);
    if (exception) goto exit;
  } else {
    result = JSObjectCallAsFunction (runCtx, function, global, 0, 0, exception);
    if (exception) goto exit;
  }

  // Convert the result into a string.
  if (result) {
    if (JSValueIsString(runCtx, result)) {
      JSStringRef temp = JSValueToStringCopy (runCtx, result, exception);
      if (exception) goto exit;

      sResult.set((PWSTR) JSStringGetCharactersPtr(temp), JSStringGetLength(temp));
      JSStringRelease(temp);

    } else if (JSValueIsNumber(runCtx, result)) {
      double temp = JSValueToNumber(runCtx, result, exception);
      if (exception) goto exit;

      sResult.appendf("%g", temp);

    } else if (JSValueIsBoolean(runCtx, result))  {
      bool temp = JSValueToBoolean(runCtx, result);
      if (exception) goto exit;

      sResult = temp ? "true" : "false";
    }
  }

exit:
  return sResult;
}

//------------------------------------
// Called by JS

static JSValueRef JS_apollo_echo(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception);

JSClassRef JS_apollo_class()
{
  static JSStaticFunction JS_apollo_functions[] = {
    { "echo", JS_apollo_echo, kJSPropertyAttributeNone },
    { 0, 0, 0 }
  };

  JSClassDefinition JS_apollo_class = {
    0, kJSClassAttributeNone, "apollo", 0, 0, JS_apollo_functions,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  static JSClassRef pApolloClass = JSClassCreate(&JS_apollo_class);
  return pApolloClass;
}

static JSValueRef JS_apollo_echo(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception)
{
  if (!JSValueIsObjectOfClass(ctx, thisObject, JS_apollo_class())) return JSValueMakeUndefined(ctx);

  if (argumentCount < 1) return JSValueMakeUndefined(ctx);

  // Convert the result into a string for display.
  if (!JSValueIsString(ctx, arguments[0])) return JSValueMakeUndefined(ctx);

  JSStringRef pArg = JSValueToStringCopy (ctx, arguments[0], exception);
  if (exception && *exception) return JSValueMakeUndefined(ctx);

  String sText;
  sText.set((PWSTR) JSStringGetCharactersPtr(pArg), JSStringGetLength(pArg));
  sText += " (JS_apollo_echo)";

  JSStringRelease(pArg);

  JSStringRef pText = JSStringCreateWithUTF8CString(sText);
  JSValueRef pValue = JSValueMakeString(ctx, pText);
  JSStringRelease(pText);

  return pValue;
}

void WebView::MakeScriptObject()
{
  if (!pWebFrame_) return;

  JSGlobalContextRef pRunCtx = pWebFrame_->globalContext();
  if (pRunCtx == 0) return;

  JSObjectRef pGlobal = JSContextGetGlobalObject(pRunCtx);
  if (pGlobal == 0) return;

  JSClassRef pApolloClass = JS_apollo_class();
  if (pApolloClass == 0) return;

  pScriptObject_ = JSObjectMake(pRunCtx, pApolloClass, reinterpret_cast<void*>(this));
  if (pScriptObject_ == 0) return;

  JSStringRef pApolloName = JSStringCreateWithUTF8CString("apollo");
  if (pApolloName == 0) return;

  JSObjectSetProperty(pRunCtx, pGlobal, pApolloName, pScriptObject_, kJSPropertyAttributeNone, 0);
  JSStringRelease(pApolloName);
}

//------------------------------------
// IUnknown

HRESULT WebView::QueryInterface(REFIID riid, void** ppvObject)
{
    *ppvObject = 0;
    if (IsEqualIID(riid, IID_IUnknown))  *ppvObject = static_cast<IWebUIDelegate*>(this);
    else if (IsEqualIID(riid, IID_IWebUIDelegate)) *ppvObject = static_cast<IWebUIDelegate*>(this);
    else return E_NOINTERFACE;

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
    if (!newRef) delete this;

    return newRef;
}

//------------------------------------

String WebView::StringFromBSTR(BSTR bStr)
{
  return String(bStr, ::SysStringLen(bStr));
}

HRESULT WebView::didStartProvisionalLoadForFrame(IWebView* webView, IWebFrame* frame)
{
  if (pTopLoadingFrame_ == 0) {
    pTopLoadingFrame_ = frame;

    MakeScriptObject();

    ApAsyncMessage<Msg_WebView_Event_DocumentLoaded> msg;
    msg->hWebView = apHandle();
    msg.Post();
  }

 return S_OK;
}

HRESULT WebView::didFinishDocumentLoadForFrame(IWebView *sender, IWebFrame *frame)
{
  if (pTopLoadingFrame_ == frame) {
    bDocumentLoaded_ = 1;
  }

#if 0
  HRESULT hr = S_OK;

  IWebDataSource* dataSource = 0;
  hr = frame->dataSource(&dataSource);
  if (FAILED(hr)) goto exit;

  IWebMutableURLRequest* request = 0;
  hr = dataSource->request(&request);
  if (FAILED(hr)) goto exit;

  BSTR bstrUrl = 0;
  hr = request->URL(&bstrUrl);
  if (FAILED(hr)) goto exit;

  apLog_Debug((LOG_CHANNEL, "WebView::OnDidFinishDocumentLoadForFrame", "%s", StringType(StringFromBSTR(bstrUrl))));
exit:
#endif

 return S_OK;
}

HRESULT WebView::didFinishLoadForFrame(IWebView* webView, IWebFrame* frame)
{
  if (pTopLoadingFrame_ == frame) {
    ApAsyncMessage<Msg_WebView_Event_DocumentComplete> msg;
    msg->hWebView = apHandle();
    msg.Post();
  }

 return S_OK;
}

