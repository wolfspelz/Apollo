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

  RECT r = { nLeft_, nTop_, nWidth_, nHeight_ };
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

  ::MoveWindow(hWnd_, nLeft_, nTop_ - 10000, nWidth_, nHeight_, TRUE);
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

void View::SetPosition(int nLeft, int nTop, int nWidth, int nHeight)
{
  nLeft_ = nLeft;
  nTop_ = nTop;
  nWidth_ = nWidth;
  nHeight_ = nHeight;

#if defined(WIN32)
  ::MoveWindow(hWnd_, nLeft_, nTop_ - (bVisible_ ? 0 : 10000), nWidth_, nHeight_, FALSE);
#endif // WIN32
}

void View::SetVisibility(int bVisible)
{
  int bChanged = (bVisible_ != bVisible);
  bVisible_ = bVisible;

  if (bChanged) {
#if defined(WIN32)
    ::MoveWindow(hWnd_, nLeft_, nTop_ - (bVisible_ ? 0 : 10000), nWidth_, nHeight_, FALSE);

    //::ShowWindow(hWnd_, bVisible_ ? SW_SHOW : SW_HIDE);
    //if (bVisible_) {
    //  ::UpdateWindow(hWnd_);
    //}
#endif // WIN32
  }
}

void View::SetScriptAccessPolicy(const String& sPolicy)
{
  if (sPolicy == Msg_WebView_SetScriptAccessPolicy_Allowed) {
    bScriptAccessEnabled_ = 1;
  }
}

void View::SetNavigationPolicy(const String& sPolicy)
{
  if (sPolicy == Msg_WebView_SetNavigationPolicy_Denied) {
    bNavigationEnabled_ = 0;
  }
}

void View::MoveBy(int nX, int nY)
{
  nLeft_ += nX;
  nTop_ += nY;

#if defined(WIN32)
  ::MoveWindow(hWnd_, nLeft_, nTop_ - (bVisible_ ? 0 : 10000), nWidth_, nHeight_, FALSE);
#endif // WIN32
}
  
void View::SizeBy(int nX, int nY, int nDirection)
{
  switch (nDirection) {
    case Msg_WebView_SizeBy::DirectionLeft:          nLeft_ -= nX;               nWidth_ += nX;                 break;
    case Msg_WebView_SizeBy::DirectionTop:                         nTop_  -= nY;                nHeight_ += nY; break;
    case Msg_WebView_SizeBy::DirectionRight:                                     nWidth_ += nX;                 break;
    case Msg_WebView_SizeBy::DirectionBottom:                                                   nHeight_ += nY; break;
    case Msg_WebView_SizeBy::DirectionTopLeft:       nLeft_ -= nX;  nTop_ -= nY; nWidth_ += nX; nHeight_ += nY; break;
    case Msg_WebView_SizeBy::DirectionTopRight:                     nTop_ -= nY; nWidth_ += nX; nHeight_ += nY; break;
    case Msg_WebView_SizeBy::DirectionBottomLeft:    nLeft_ -= nX;               nWidth_ += nX; nHeight_ += nY; break;
    case Msg_WebView_SizeBy::DirectionBottomRight:                               nWidth_ += nX; nHeight_ += nY; break;
  }

  //if (nWidth_ < 0) { nWidth_ = 0; }
  //if (nHeight_ < 0) { nHeight_ = 0; }

  // Prevent from hitting https://bugs.webkit.org/show_bug.cgi?id=60618
  if (nWidth_ < 1) { nWidth_ = 1; }
  if (nHeight_ < 1) { nHeight_ = 1; }

#if defined(WIN32)
  ::MoveWindow(hWnd_, nLeft_, nTop_ - (bVisible_ ? 0 : 10000), nWidth_, nHeight_, FALSE);
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

void View::GetPosition(int& nLeft, int& nTop, int& nWidth, int& nHeight)
{
  nLeft = nLeft_;
  nTop = nTop_;
  nWidth = nWidth_;
  nHeight = nHeight_;
}

void View::GetVisibility(int& bVisible)
{
  bVisible = bVisible_;
}

//------------------------------------
// Call JS

String View::CallJsFunction(const String& sFunction, List& lArgs)
{
  String sResult;

  JSGlobalContextRef runCtx = pWebFrame_->globalContext();

  AutoJSStringRef methodName = JSStringCreateWithUTF8CString(sFunction);

  JSObjectRef global = JSContextGetGlobalObject(runCtx);

  JSValueRef* exception = 0;
  JSValueRef sampleFunction = JSObjectGetProperty(runCtx, global, methodName, exception);
  if (exception) { throw ApException("View::CallJsFunction JSObjectGetProperty(methodName) returned exception"); }

  if (!JSValueIsObject(runCtx, sampleFunction)) { throw ApException("View::CallJsFunction no such function: %s", StringType(sFunction)); }

  JSObjectRef function = JSValueToObject(runCtx, sampleFunction, exception);
  if (exception) { throw ApException("View::CallJsFunction JSValueToObject(sampleFunction) returned exception"); }

  JSValueRef result = 0;

  if (lArgs.length() > 0) {
    AutoPtr<JSValueRef> args(new JSValueRef[lArgs.length()]);
    int nCnt = 0;
    for (Elem* e = 0; (e = lArgs.Next(e)) != 0; ) {
      args[nCnt++] = JSValueMakeString (runCtx, JSStringCreateWithCharacters((LPCTSTR) e->getName(), e->getName().chars()));
    }
    result = JSObjectCallAsFunction (runCtx, function, global, lArgs.length(), args.get(), exception);
    if (exception) { throw ApException("View::CallJsFunction JSObjectCallAsFunction() returned exception"); }
  } else {
    result = JSObjectCallAsFunction (runCtx, function, global, 0, 0, exception);
    if (exception) { throw ApException("View::CallJsFunction JSObjectCallAsFunction() returned exception"); }
  }

  // Convert the result into a string.
  if (result) {
    if (JSValueIsString(runCtx, result)) {
      AutoJSStringRef value = JSValueToStringCopy (runCtx, result, exception);
      if (exception) { throw ApException("View::CallJsFunction JSValueToStringCopy() returned exception"); }

      sResult.set((PWSTR) JSStringGetCharactersPtr(value), JSStringGetLength(value));

    } else if (JSValueIsNumber(runCtx, result)) {
      double value = JSValueToNumber(runCtx, result, exception);
      if (exception) { throw ApException("View::CallJsFunction JSValueToNumber() returned exception"); }

      sResult.appendf("%g", value);

    } else if (JSValueIsBoolean(runCtx, result))  {
      bool value = JSValueToBoolean(runCtx, result);
      if (exception) { throw ApException("View::CallJsFunction JSValueToBoolean() returned exception"); }

      sResult = value ? "true" : "false";
    }
  }

  return sResult;
}

void View::CallJsSrpc(const String& sFunction, Apollo::SrpcMessage& srpc, Apollo::SrpcMessage& response)
{
  Msg_WebView_CallScriptFunction msg;
  msg.hView = apHandle();
  msg.sFunction = sFunction;
  msg.lArgs.AddLast(srpc.toString());
  if (!msg.Request()) { throw ApException("View::CallJsSrpc Msg_WebView_CallScriptFunction: %s", StringType(msg.sComment)); }

  response.fromString(msg.sResult);
  if (response.length() > 0) {
    int nStatus = response.getInt("Status");
    if (nStatus != 1) {
      throw ApException("View::CallJsSrpc Status=%d Message=%s", nStatus, StringType(response.getString("Message")));
    }
  }
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
  String sType = srpc.getString("ApType");
  String sResponse;

  if (sType) {
    // Supplied message type -> send SRPC with custom type

    ApSRPCMessage msg(sType);
    srpc >> msg.srpc;

    // Custom message handlers just do the apStatus thing.
    // They rely on someone else (us here) to fill msg.response.
    if (msg.Call()) {
      msg.response.setInt("Status", 1);
    } else {
      msg.response.setInt("Status", 0);
      msg.response.setString("Message", msg.sComment);
    }

    sResponse = msg.response.toString();

  } else {
    // Handle SRPC message via SrpcGate with "SrpcGate" type

    ApSRPCMessage msg("SrpcGate");
    srpc >> msg.srpc;

    // SrpcGate handlers are supposed to do their own Status handling.
    // They already fill msg.response if they like.
    (void) msg.Call();

    sResponse = msg.response.toString();

  }

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

  if (0) {}
  else if (IsEqualIID(riid, IID_IUnknown)) *ppvObject = static_cast<IWebUIDelegate*>(this);
  else if (IsEqualIID(riid, IID_IWebUIDelegate)) *ppvObject = static_cast<IWebUIDelegate*>(this);
  else if (IsEqualIID(riid, IID_IWebUIDelegatePrivate)) *ppvObject = static_cast<IWebUIDelegatePrivate*>(this);
  else if (IsEqualIID(riid, IID_IWebFrameLoadDelegate)) *ppvObject = static_cast<IWebFrameLoadDelegate*>(this);
  else if (IsEqualIID(riid, IID_IWebFrameLoadDelegatePrivate)) *ppvObject = static_cast<IWebFrameLoadDelegatePrivate*>(this);
  else if (IsEqualIID(riid, IID_IWebResourceLoadDelegate)) *ppvObject = static_cast<IWebResourceLoadDelegate*>(this);
  else if (IsEqualIID(riid, IID_IWebPolicyDelegate)) *ppvObject = static_cast<IWebPolicyDelegate*>(this);
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

String View::GetUrlFrom(IWebFrame *frame)
{
  String sUrl;

  AutoComPtr<IWebDataSource> dataSource;
  AutoComPtr<IWebMutableURLRequest> request;

  if (FAILED( frame->dataSource(dataSource) )) goto exit;
  if (FAILED( dataSource->request(request) )) goto exit;

  BSTR bstrUrl = 0;
  if (FAILED( request->URL(&bstrUrl) )) goto exit;

  sUrl = StringFromBSTR(bstrUrl);

exit:
  return sUrl;
}

String View::GetUrlFrom(IWebURLRequest *request)
{
  String sUrl;

  BSTR bstrUrl = 0;
  if (FAILED( request->URL(&bstrUrl) )) goto exit;

  sUrl = StringFromBSTR(bstrUrl);

exit:
  return sUrl;
}

HRESULT View::didFinishDocumentLoadForFrame(IWebView *webView, IWebFrame *frame)
{
  if (pTopLoadingFrame_ == frame) {
    ApAsyncMessage<Msg_WebView_Event_DocumentLoaded> msg;
    msg->hView = apHandle();
    msg.Post();

    try {
      MakeScriptObject();
    } catch (ApException& ex) {
      apLog_Error((LOG_CHANNEL, "View::didFinishDocumentLoadForFrame", "MakeScriptObject() %s", StringType(ex.getText())));
    }

    if (apLog_IsVerbose) {
      apLog_Verbose((LOG_CHANNEL, "View::didFinishDocumentLoadForFrame", "%s", StringType(GetUrlFrom(frame))));
    }
  }

  return S_OK;
}

HRESULT View::didFinishLoadForFrame(IWebView* webView, IWebFrame* frame)
{
  if (pTopLoadingFrame_ == frame) {
    ApAsyncMessage<Msg_WebView_Event_DocumentComplete> msg;
    msg->hView = apHandle();
    msg.Post();

    if (apLog_IsVerbose) {
      apLog_Verbose((LOG_CHANNEL, "View::didFinishLoadForFrame", "%s", StringType(GetUrlFrom(frame))));
    }
  }

  return S_OK;
}

HRESULT View::willSendRequest(IWebView *webView, unsigned long identifier, IWebURLRequest *request, IWebURLResponse *redirectResponse, IWebDataSource *dataSource, IWebURLRequest **newRequest)
{
  int bChanged = 0;

  Msg_WebView_Event_BeforeRequest msg;

  if (pWebView_ == webView) {
    if (apLog_IsVerbose) {
      apLog_Verbose((LOG_CHANNEL, "View::willSendRequest", "%s", StringType(GetUrlFrom(request))));
    }

    BSTR bstrUrl = 0;
    request->URL(&bstrUrl);
    String sUrl = StringFromBSTR(bstrUrl);

    msg.hView = apHandle();
    msg.sUrl = sUrl;
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
  int bNavigate = 1;

  if (pWebView_ == webView) {
    bHandled = 1;

    bNavigate = HasNavigation() || (GetUrlFrom(request) == GetUrlFrom(frame));

    if (bNavigate) {
      Msg_WebView_Event_BeforeNavigate msg;

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

      bNavigate = !msg.bCancel;
      bHandled = 1;
    }
  }

  if (bHandled) {

    if (bNavigate) {
      listener->use();
    } else {
      listener->ignore();
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

HRESULT View::webViewAddMessageToConsole(IWebView *sender, BSTR message, int lineNumber, BSTR url, BOOL isError)
{
  String sUrl = StringFromBSTR(url);
  String sMessage = StringFromBSTR(message);

  if (isError) {
    apLog_Error((LOG_CHANNEL, "JS:", "%s(%d): %s", StringType(sUrl), lineNumber, StringType(sMessage)));
  } else {
    apLog_Warning((LOG_CHANNEL, "JS:", "%s(%d): %s", StringType(sUrl), lineNumber, StringType(sMessage)));
  }

  return S_OK;
}
