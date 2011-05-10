// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgMainLoop.h"
#include "MsgSrpcGate.h"
#include "Local.h"
#include "View.h"
#include "SrpcMessage.h"
#include "SAutoPtr.h"

View::~View()
{
}

void View::Create()
{
  AutoComPtr<IWebPreferences> tmpPreferences;
  AutoComPtr<IWebPreferences> standardPreferences;
  if (FAILED( WebKitCreateInstance(CLSID_WebPreferences, 0, IID_IWebPreferences, tmpPreferences) )) { throw ApException("View::Create WebKitCreateInstance(CLSID_WebPreferences) failed"); }
  if (FAILED( tmpPreferences->standardPreferences(standardPreferences) )) { throw ApException("View::Create tmpPreferences->standardPreferences() failed"); }
  if (FAILED( standardPreferences->setAcceleratedCompositingEnabled(TRUE) )) { throw ApException("View::Create standardPreferences->setAcceleratedCompositingEnabled() failed"); }

  // -----------------------------

  if (FAILED( WebKitCreateInstance(CLSID_WebView, 0, IID_IWebView, reinterpret_cast<void**>(&pWebView_)) )) { throw ApException("View::Create WebKitCreateInstance(CLSID_WebView) failed"); }
  if (FAILED( pWebView_->setUIDelegate(this) )) { throw ApException("View::Create pWebView_->setUIDelegate() failed"); }
  if (FAILED( pWebView_->setFrameLoadDelegate(this) )) { throw ApException("View::Create pWebView_->setFrameLoadDelegate() failed"); }
  if (FAILED( pWebView_->setResourceLoadDelegate(this) )) { throw ApException("View::Create pWebView_->setResourceLoadDelegate() failed"); }
  if (FAILED( pWebView_->setPolicyDelegate(this) )) { throw ApException("View::Create pWebView_->setPolicyDelegate() failed"); }
  if (FAILED( pWebView_->QueryInterface(IID_IWebViewPrivate, reinterpret_cast<void**>(&pWebViewPrivate_)) )) { throw ApException("View::Create QueryInterface(IID_IWebViewPrivate) failed"); }

  RECT r = { nX_, nY_, nW_, nH_ };
  if (FAILED( pWebView_->initWithFrame(r, 0, 0) )) { throw ApException("View::Create pWebView_->initWithFrame() failed"); }

  // -----------------------------

  if (FAILED( pWebViewPrivate_->setFrameLoadDelegatePrivate(this) )) { throw ApException("View::Create pWebViewPrivate_->setFrameLoadDelegatePrivate() failed"); }
  if (FAILED( pWebViewPrivate_->setTransparent(TRUE) )) { throw ApException("View::Create pWebViewPrivate_->setTransparent() failed"); }
  if (FAILED( pWebViewPrivate_->setUsesLayeredWindow(TRUE) )) { throw ApException("View::Create pWebViewPrivate_->setUsesLayeredWindow() failed"); }
  if (FAILED( pWebViewPrivate_->viewWindow(reinterpret_cast<OLE_HANDLE*>(&hWnd_))) ) { throw ApException("View::Create pWebViewPrivate_->viewWindow() failed"); }
  if (hWnd_ == NULL) { throw ApException("View::Create pWebViewPrivate_->viewWindow() did not return hWnd"); }

  ::ShowWindow(hWnd_, SW_SHOW);
  ::UpdateWindow(hWnd_);

  // -----------------------------

  if (FAILED( pWebView_->mainFrame(&pWebFrame_) )) { throw ApException("View::Create pWebView_->mainFrame() failed"); }

  ::MoveWindow(hWnd_, nX_, nY_ - 10000, nW_, nH_, TRUE);
}

void View::Destroy()
{
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
}

void View::LoadHtml(const String& sHtml, const String& sBase)
{
  if (!pWebFrame_) { throw ApException("View::LoadHtml pWebFrame_== 0"); }

  if (1){
    if (FAILED( pWebFrame_->loadHTMLString(::SysAllocString(sHtml), ::SysAllocString(sBase)) )) { throw ApException("View::Load pWebFrame_->loadRequest() failed"); }
  } else {
    BSTR bstrHTML = SysAllocString(TEXT("<p style=\"background-color: #00FF00\">Testing</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul><iframe src=\"http://www.wolfspelz.de\" style=\"width:100%;height:300px\" />"));
    pWebFrame_->loadHTMLString(bstrHTML, 0);
  }

  MakeScriptObject();
}

#include <shlwapi.h>
#include <wininet.h>

void View::Load(const String& sUrl)
{
  sUrl_ = sUrl;

  AutoComPtr<IWebMutableURLRequest> request;
  if (FAILED( WebKitCreateInstance(CLSID_WebMutableURLRequest, 0, IID_IWebMutableURLRequest, request) )) { throw ApException("View::Load WebKitCreateInstance(CLSID_WebMutableURLRequest) failed"); }

  if (1) {
    if (FAILED( request->initWithURL(::SysAllocString(sUrl), WebURLRequestUseProtocolCachePolicy, 60) )) { throw ApException("View::Load request->initWithURL() failed"); }
  } else {
    BSTR bstrUrl = 0;
    //BSTR bstrUrl = SysAllocString(_T("http://www.wolfspelz.de"));
    if (!bstrUrl) {
      TCHAR szFileName[MAX_PATH];
      ::GetModuleFileName(NULL, szFileName, MAX_PATH);
      ::PathRemoveFileSpec(szFileName);
      ::PathAddBackslash(szFileName);
      //::PathAppend(szFileName, Apollo::getModuleResourcePath(MODULE_NAME) + "/html/test/frame.html");
      ::PathAppend(szFileName, Apollo::getModuleResourcePath(MODULE_NAME) + "/html/test/overlay.html");

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

    if (FAILED( request->initWithURL(bstrUrl, WebURLRequestUseProtocolCachePolicy, 60) )) { throw ApException("View::Load request->initWithURL() failed"); }
  }

  //if (FAILED( pWebFrame_->setAllowsScrolling(FALSE) )) { throw ApException("View::Load pWebFrame_->setAllowsScrolling() failed"); }

  if (FAILED( pWebFrame_->loadRequest(request) )) { throw ApException("View::Load pWebFrame_->loadRequest() failed"); }

  MakeScriptObject();
}

void View::Reload()
{
  if (FAILED( pWebFrame_->reloadFromOrigin() )) { throw ApException("View::Reload pWebFrame_->reload() failed"); }

  MakeScriptObject();
}

//------------------------------------

void View::SetPosition(int nX, int nY, int nW, int nH)
{
  nX_ = nX;
  nY_ = nY;
  nW_ = nW;
  nH_ = nH;

#if defined(WIN32)
  ::MoveWindow(hWnd_, nX_, nY_ - (bVisible_ ? 0 : 10000), nW_, nH_, FALSE);
#endif // WIN32
}

void View::SetVisibility(int bVisible)
{
  int bChanged = (bVisible_ != bVisible);
  bVisible_ = bVisible;

  if (bChanged) {
#if defined(WIN32)
    ::MoveWindow(hWnd_, nX_, nY_ - (bVisible_ ? 0 : 10000), nW_, nH_, FALSE);

    //::ShowWindow(hWnd_, bVisible_ ? SW_SHOW : SW_HIDE);
    //if (bVisible_) {
    //  ::UpdateWindow(hWnd_);
    //}
#endif // WIN32
  }
}

void View::SetJSAccess(const String& sAccess)
{
  if (sAccess == Msg_WebView_SetScriptAccess_Allowed) {
    bScriptAccess_ = 1;
  }
}

void View::MoveBy(int nX, int nY)
{
  nX_ += nX;
  nY_ += nY;

#if defined(WIN32)
  ::MoveWindow(hWnd_, nX_, nY_ - (bVisible_ ? 0 : 10000), nW_, nH_, FALSE);
#endif // WIN32
}
  
void View::SizeBy(int nW, int nH, int nDirection)
{
  switch (nDirection) {
    case Msg_WebView_SizeBy::DirectionLeft:        nX_ -= nW;            nW_ += nW;            break;
    case Msg_WebView_SizeBy::DirectionTop:                    nY_ -= nH;            nH_ += nH; break;
    case Msg_WebView_SizeBy::DirectionRight:                             nW_ += nW;            break;
    case Msg_WebView_SizeBy::DirectionBottom:                                       nH_ += nH; break;
    case Msg_WebView_SizeBy::DirectionTopLeft:     nX_ -= nW; nY_ -= nH; nW_ += nW; nH_ += nH; break;
    case Msg_WebView_SizeBy::DirectionTopRight:               nY_ -= nH; nW_ += nW; nH_ += nH; break;
    case Msg_WebView_SizeBy::DirectionBottomLeft:  nX_ -= nW;            nW_ += nW; nH_ += nH; break;
    case Msg_WebView_SizeBy::DirectionBottomRight:                       nW_ += nW; nH_ += nH; break;
  }

#if defined(WIN32)
  ::MoveWindow(hWnd_, nX_, nY_ - (bVisible_ ? 0 : 10000), nW_, nH_, FALSE);
#endif // WIN32
}

void View::MouseCapture()
{
#if defined(WIN32)
  ::SetCapture(hWnd_);
#endif // WIN32
}

void View::MouseRelease()
{
#if defined(WIN32)
  ::ReleaseCapture();
#endif // WIN32
}

void View::GetPosition(int& nX, int& nY, int& nW, int& nH)
{
  nX = nX_;
  nY = nY_;
  nW = nW_;
  nH = nH_;
}

void View::GetVisibility(int& bVisible)
{
  bVisible = bVisible_;
}

//------------------------------------
// Call JS

String View::CallJSFunction(const String& sFunction, List& lArgs)
{
  String sResult;

  JSGlobalContextRef runCtx = pWebFrame_->globalContext();

  AutoJSStringRef methodName = JSStringCreateWithUTF8CString(sFunction);

  JSObjectRef global = JSContextGetGlobalObject(runCtx);

  JSValueRef* exception = 0;
  JSValueRef sampleFunction = JSObjectGetProperty(runCtx, global, methodName, exception);
  if (exception) goto exit;

  if (!JSValueIsObject(runCtx, sampleFunction)) goto exit;

  JSObjectRef function = JSValueToObject(runCtx, sampleFunction, exception);
  if (exception) goto exit;

  JSValueRef result = 0;

  if (lArgs.length() > 0) {
    AutoPtr<JSValueRef> args(new JSValueRef[lArgs.length()]);
    int nCnt = 0;
    for (Elem* e = 0; (e = lArgs.Next(e)) != 0; ) {
      args[nCnt++] = JSValueMakeString (runCtx, JSStringCreateWithCharacters((LPCTSTR) e->getName(), e->getName().chars()));
    }
    result = JSObjectCallAsFunction (runCtx, function, global, lArgs.length(), args.get(), exception);
    if (exception) goto exit;
  } else {
    result = JSObjectCallAsFunction (runCtx, function, global, 0, 0, exception);
    if (exception) goto exit;
  }

  // Convert the result into a string.
  if (result) {
    if (JSValueIsString(runCtx, result)) {
      AutoJSStringRef value = JSValueToStringCopy (runCtx, result, exception);
      if (exception) goto exit;

      sResult.set((PWSTR) JSStringGetCharactersPtr(value), JSStringGetLength(value));

    } else if (JSValueIsNumber(runCtx, result)) {
      double value = JSValueToNumber(runCtx, result, exception);
      if (exception) goto exit;

      sResult.appendf("%g", value);

    } else if (JSValueIsBoolean(runCtx, result))  {
      bool value = JSValueToBoolean(runCtx, result);
      if (exception) goto exit;

      sResult = value ? "true" : "false";
    }
  }

exit:
  return sResult;
}

//------------------------------------
// Called by JS

JSClassRef JS_Apollo_class()
{
  static JSStaticValue JS_Apollo_values[] = {
    { "viewHandle", View::JS_Apollo_getSharedValue, 0, kJSPropertyAttributeNone },
    { 0, 0, 0, 0 }
  };

  static JSStaticFunction JS_Apollo_functions[] = {
    { "echoString", View::JS_Apollo_echoString, kJSPropertyAttributeNone },
    { "sendMessage", View::JS_Apollo_sendMessage, kJSPropertyAttributeNone },
    { 0, 0, 0 }
  };

  JSClassDefinition JS_Apollo_class = {
    0,                     // int version // current (and only) version is 0
    kJSClassAttributeNone, // JSClassAttributes attributes
    "apollo",              // const char* className
    0,                     // JSClassRef parentClass
    JS_Apollo_values,      // const JSStaticValue* staticValues
    JS_Apollo_functions,   // const JSStaticFunction* staticFunctions
    0,                     // JSObjectInitializeCallback initialize
    0,                     // JSObjectFinalizeCallback finalize
    0,                     // JSObjectHasPropertyCallback hasProperty
    0,                     // JSObjectGetPropertyCallback getProperty
    0,                     // JSObjectSetPropertyCallback setProperty
    0,                     // JSObjectDeletePropertyCallback deleteProperty
    0,                     // JSObjectGetPropertyNamesCallback getPropertyNames
    0,                     // JSObjectCallAsFunctionCallback callAsFunction
    0,                     // JSObjectCallAsConstructorCallback callAsConstructor
    0,                     // JSObjectHasInstanceCallback hasInstance
    0                      // JSObjectConvertToTypeCallback convertToType
  };

  static JSClassRef apolloClass = JSClassCreate(&JS_Apollo_class);
  return apolloClass;
}

JSValueRef View::JS_Apollo_getSharedValue(JSContextRef ctx, JSObjectRef thisObject, JSStringRef propertyName, JSValueRef* exception)
{
  if (!JSValueIsObjectOfClass(ctx, thisObject, JS_Apollo_class())) return JSValueMakeUndefined(ctx);

  View* pView = static_cast<View*>(JSObjectGetPrivate(thisObject));
  if (pView == 0 || !pView->HasScriptAccess()) return JSValueMakeUndefined(ctx);

  String sName;
  String sValue;
  sName.set((PWSTR) JSStringGetCharactersPtr(propertyName), JSStringGetLength(propertyName));
  if (0) {
  } else if (sName == "viewHandle") {
    sValue = pView->apHandle().toString();
  } else {
    return JSValueMakeUndefined(ctx);
  }

  AutoJSStringRef text = JSStringCreateWithUTF8CString(sValue);
  JSValueRef value = JSValueMakeString(ctx, text);

  return value;
}

JSValueRef View::JS_Apollo_echoString(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception)
{
  if (!JSValueIsObjectOfClass(ctx, thisObject, JS_Apollo_class())) return JSValueMakeUndefined(ctx);

  View* pView = static_cast<View*>(JSObjectGetPrivate(thisObject));
  if (pView == 0 || !pView->HasScriptAccess()) return JSValueMakeUndefined(ctx);

  if (argumentCount < 1) return JSValueMakeUndefined(ctx);

  // Convert the result into a string for display.
  if (!JSValueIsString(ctx, arguments[0])) return JSValueMakeUndefined(ctx);

  AutoJSStringRef arg0 = JSValueToStringCopy(ctx, arguments[0], exception);
  if (exception && *exception) return JSValueMakeUndefined(ctx);

  String sText;
  sText.set((PWSTR) JSStringGetCharactersPtr(arg0), JSStringGetLength(arg0));

  sText += " (JS_Apollo_echoString)";

  AutoJSStringRef text = JSStringCreateWithUTF8CString(sText);
  JSValueRef value = JSValueMakeString(ctx, text);

  return value;
}

JSValueRef View::JS_Apollo_sendMessage(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception)
{
  if (!JSValueIsObjectOfClass(ctx, thisObject, JS_Apollo_class())) return JSValueMakeUndefined(ctx);

  View* pView = static_cast<View*>(JSObjectGetPrivate(thisObject));
  if (pView == 0 || !pView->HasScriptAccess()) return JSValueMakeUndefined(ctx);

  if (argumentCount < 1) return JSValueMakeUndefined(ctx);

  // Convert the result into a string for display.
  if (!JSValueIsString(ctx, arguments[0])) return JSValueMakeUndefined(ctx);

  AutoJSStringRef arg0 = JSValueToStringCopy (ctx, arguments[0], exception);
  if (exception && *exception) return JSValueMakeUndefined(ctx);

  String sText;
  sText.set((PWSTR) JSStringGetCharactersPtr(arg0), JSStringGetLength(arg0));

  Apollo::SrpcMessage srpc;
  srpc.fromString(sText);
  ApSRPCMessage msg("SrpcGate");
  srpc >> msg.srpc;
  (void) msg.Call();
  String sResponse = msg.response.toString();

  AutoJSStringRef text = JSStringCreateWithUTF8CString(sResponse);
  JSValueRef value = JSValueMakeString(ctx, text);

  return value;
}

void View::MakeScriptObject()
{
  if (!pWebFrame_) { throw ApException("View::MakeScriptObject pWebFrame_== 0"); }

  JSGlobalContextRef runCtx = pWebFrame_->globalContext();
  if (runCtx == 0) { throw ApException("View::MakeScriptObject pWebFrame_->globalContext() failed"); }

  JSObjectRef pGlobal = JSContextGetGlobalObject(runCtx);
  if (pGlobal == 0) { throw ApException("View::MakeScriptObject JSContextGetGlobalObject() failed"); }

  JSClassRef apolloClass = JS_Apollo_class();
  if (apolloClass == 0) { throw ApException("View::MakeScriptObject JS_Apollo_class() failed"); }

  pScriptObject_ = JSObjectMake(runCtx, apolloClass, reinterpret_cast<void*>(this));
  if (pScriptObject_ == 0) { throw ApException("View::MakeScriptObject JSObjectMake() failed"); }

  AutoJSStringRef apolloName = JSStringCreateWithUTF8CString("apollo");
  if (!apolloName) { throw ApException("View::MakeScriptObject JSStringCreateWithUTF8CString() failed"); }

  JSObjectSetProperty(runCtx, pGlobal, apolloName, pScriptObject_, kJSPropertyAttributeNone, 0);
}

//------------------------------------
// IUnknown

HRESULT View::QueryInterface(REFIID riid, void** ppvObject)
{
    *ppvObject = 0;
    if (IsEqualIID(riid, IID_IUnknown))  *ppvObject = static_cast<IWebUIDelegate*>(this);
    else if (IsEqualIID(riid, IID_IWebUIDelegate)) *ppvObject = static_cast<IWebUIDelegate*>(this);
    else return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

ULONG View::AddRef(void)
{
    return ++nRefCount_;
}

ULONG View::Release(void)
{
    ULONG newRef = --nRefCount_;
    if (!newRef) delete this;

    return newRef;
}

//------------------------------------

String View::StringFromBSTR(BSTR bStr)
{
  return String(bStr, ::SysStringLen(bStr));
}

HRESULT View::didStartProvisionalLoadForFrame(IWebView* webView, IWebFrame* frame)
{
  if (pTopLoadingFrame_ == 0) {
    pTopLoadingFrame_ = frame;
  }

  return S_OK;
}

HRESULT View::didFinishDocumentLoadForFrame(IWebView *sender, IWebFrame *frame)
{
  if (pTopLoadingFrame_ == frame) {
    ApAsyncMessage<Msg_WebView_Event_DocumentLoaded> msg;
    msg->hView = apHandle();
    msg.Post();
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

  apLog_Debug((LOG_CHANNEL, "View::OnDidFinishDocumentLoadForFrame", "%s", StringType(StringFromBSTR(bstrUrl))));
exit:
#endif

  return S_OK;
}

HRESULT View::didFinishLoadForFrame(IWebView* webView, IWebFrame* frame)
{
  if (pTopLoadingFrame_ == frame) {
    ApAsyncMessage<Msg_WebView_Event_DocumentComplete> msg;
    msg->hView = apHandle();
    msg.Post();
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

  apLog_Debug((LOG_CHANNEL, "View::didFinishLoadForFrame", "%s", StringType(StringFromBSTR(bstrUrl))));
exit:
#endif
  return S_OK;
}

HRESULT View::willSendRequest(IWebView *webView, unsigned long identifier, IWebURLRequest *request, IWebURLResponse *redirectResponse, IWebDataSource *dataSource, IWebURLRequest **newRequest)
{
  int bChanged = 0;

  Msg_WebView_Event_BeforeRequest msg;

  if (pWebView_ == webView) {
    BSTR bstrUrl = 0;
    request->URL(&bstrUrl);
    String sUrl = StringFromBSTR(bstrUrl);

    msg.hView = apHandle();
    msg.sUrl = StringFromBSTR(bstrUrl);
    msg.Filter();

    //if (msg.sUrl == "http://webkit.org/images/icon-gold.png") {
    //  msg.sUrl = "http://www.wolfspelz.de/img/wolf2-3d.gif";
    //}

    if (msg.sUrl != sUrl) {
      bChanged = 1;
    }
  }

  if (bChanged) {
    IWebMutableURLRequest* requestCopy = 0;
    request->mutableCopy(&requestCopy);

    requestCopy->setURL(::SysAllocString(msg.sUrl));

    *newRequest = requestCopy;
    return S_OK;
  } else {
    return E_NOTIMPL;
  }
}

HRESULT View::decidePolicyForNavigationAction(IWebView *webView, IPropertyBag *actionInformation, IWebURLRequest *request, IWebFrame *frame, IWebPolicyDecisionListener *listener)
{
  int bHandled = 0;

  Msg_WebView_Event_BeforeNavigate msg;

  if (pWebView_ == webView) {
    BSTR bstrUrl = 0;
    request->URL(&bstrUrl);
    String sUrl = StringFromBSTR(bstrUrl);

    msg.hView = apHandle();
    msg.sUrl = StringFromBSTR(bstrUrl);
    msg.Filter();

    //if (msg.sUrl == "http://blog.wolfspelz.de/") {
    //  msg.bCancel = 1;
    //  ApAsyncMessage<Msg_WebView_Load> loadMsg;
    //  loadMsg->hView = apHandle();
    //  loadMsg->sUrl = "http://www.google.com/";
    //  loadMsg.Post();
    //}

    bHandled = 1;
  }

  if (bHandled) {

    if (msg.bCancel) {
      listener->ignore();
    } else {
      listener->use();
    }

    return S_OK;
  } else {
    return E_NOTIMPL;
  }
}

HRESULT View::willPerformDragSourceAction(IWebView*, WebDragSourceAction, LPPOINT, IDataObject*, IDataObject**)
{
  // Disable drag
  return S_OK;
}
