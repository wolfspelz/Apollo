// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "View.h"
#include "SrpcMessage.h"
#include "SAutoPtr.h"

void SerializedLoadTask::Execute()
{
  if (pView_) {
    //apLog_Debug((LOG_CHANNEL, LOG_CONTEXT, "%s", _sz(sUrl_)));
    pView_->SerializedLoad(sUrl_);
  }
}

void SerializedLoadHtmlTask::Execute()
{
  if (pView_) {
    //apLog_Debug((LOG_CHANNEL, LOG_CONTEXT, "%s", _sz(sBase_)));
    pView_->SerializedLoadHtml(sHtml_, sBase_);
  }
}

//---------------------------

void SerializedLoadTaskList::LoadHtml(View* pView, const String& sHtml, const String& sBase)
{
  //apLog_Debug((LOG_CHANNEL, LOG_CONTEXT, "%s", _sz(sBase)));
  SerializedTask* pTask = new SerializedLoadHtmlTask(pView, sHtml, sBase);
  if (pTask) {
    AddLast(pTask);
  }
  TryLoad();
}

void SerializedLoadTaskList::Load(View* pView, const String& sUrl)
{
  //apLog_Debug((LOG_CHANNEL, LOG_CONTEXT, "%s", _sz(sUrl)));
  SerializedTask* pTask = new SerializedLoadTask(pView, sUrl);
  if (pTask) {
    AddLast(pTask);
  }
  TryLoad();
}

void SerializedLoadTaskList::TryLoad()
{
  if (length() > 0) {

    int bExecute = 0;

    if (bLocked_) {
      String sUrls;
      if (apLog_IsVerbose) {
        for (SerializedTask* pTask = 0; (pTask = Next(pTask)) != 0; ) {
          sUrls += " " + pTask->Description();
        }
      }
      apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "Backlog: %d [current=%s] %s", length(), _sz(sCurrent_), _sz(sUrls)));

      int nTimeout = Apollo::getModuleConfig(MODULE_NAME, "SerializedLoadTimeout", 20);
      if (Apollo::getNow() - tvLocked_ > Apollo::TimeValue(nTimeout, 0)) {
        apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "Unlocking: after %d sec", nTimeout));
        LoadDone();
        bExecute = 1;
      }
    } else {
      bExecute = 1;
    }
    
    if (bExecute) {
      SerializedTask* pTask = First();
      if (pTask) {
        Remove(pTask);
        bLocked_ = 1;
        sCurrent_ = pTask->Description();
        pTask->Execute();
        delete pTask;
        pTask = 0;
        tvLocked_ = Apollo::getNow();
      }
    }

  }
}

void SerializedLoadTaskList::LoadDone()
{
  bLocked_ = 0;
  sCurrent_ = "";
}

void SerializedLoadTaskList::ViewDeleted(View* pView)
{
  int bDone = 0;
  while (!bDone) {
    bDone = 1;
    SerializedTask* pDeleteTask = 0;
    for (SerializedTask* pTask = 0; (pTask = Next(pTask)) != 0; ) {
      if (pTask->pView_ == pView) {
        pDeleteTask = pTask;
        bDone = 0;
        break;
      }
    }
    if (pDeleteTask) {
      Remove(pDeleteTask);
      delete pDeleteTask;
      pDeleteTask = 0;
    }
  }
}

//------------------------------------------------------

SerializedLoadTaskList View::serializedLoads_;

View::View(const ApHandle& hView)
:hAp_(hView)
,bLoaded_(0)
,bVisible_(0)
,nLeft_(100)
,nTop_(100)
,nWidth_(200)
,nHeight_(100)
,nFlags_(0)
,bScriptAccessEnabled_(0)
,bNavigationEnabled_(1)
,pWebView_(0)
,pWebFrame_(0)
,pWebViewPrivate_(0)
,hWnd_(NULL)
,nRefCount_(0)
,pTopLoadingFrame_(0)
,pScriptObject_(0)
{}

View::~View()
{
  serializedLoads_.ViewDeleted(this);
}

void View::Create(int nLeft, int nTop, int nWidth, int nHeight)
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " %d,%d %dx%d", ApHandlePrintf(hAp_), nLeft, nTop, nWidth, nHeight));

  nLeft_ = nLeft;
  nTop_ = nTop;
  nWidth_ = nWidth;
  nHeight_ = nHeight;

  AutoComPtr<IWebPreferences> tmpPreferences;
  if (FAILED( WebKitCreateInstance(CLSID_WebPreferences, 0, IID_IWebPreferences, tmpPreferences) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " WebKitCreateInstance(CLSID_WebPreferences) failed", ApHandlePrintf(hAp_)); }
  AutoComPtr<IWebPreferences> standardPreferences;
  if (FAILED( tmpPreferences->standardPreferences(standardPreferences) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " tmpPreferences->standardPreferences() failed", ApHandlePrintf(hAp_)); }
  if (FAILED( standardPreferences->setAcceleratedCompositingEnabled(TRUE) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " standardPreferences->setAcceleratedCompositingEnabled() failed", ApHandlePrintf(hAp_)); }

  //AutoComPtr<IWebPreferencesPrivate> privatePreferences;
  //// How to get privatePreferences?
  //if (FAILED( privatePreferences->setDeveloperExtrasEnabled(FALSE) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " privatePreferences->setDeveloperExtrasEnabled() failed", ApHandlePrintf(hAp_)); }

  // -----------------------------

  if (FAILED( WebKitCreateInstance(CLSID_WebView, 0, IID_IWebView, reinterpret_cast<void**>(&pWebView_)) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " WebKitCreateInstance(CLSID_WebView) failed", ApHandlePrintf(hAp_)); }
  if (FAILED( pWebView_->setUIDelegate(this) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebView_->setUIDelegate() failed", ApHandlePrintf(hAp_)); }
  if (FAILED( pWebView_->setFrameLoadDelegate(this) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebView_->setFrameLoadDelegate() failed", ApHandlePrintf(hAp_)); }
  if (FAILED( pWebView_->setResourceLoadDelegate(this) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebView_->setResourceLoadDelegate() failed", ApHandlePrintf(hAp_)); }
  if (FAILED( pWebView_->setPolicyDelegate(this) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebView_->setPolicyDelegate() failed", ApHandlePrintf(hAp_)); }
  if (FAILED( pWebView_->QueryInterface(IID_IWebViewPrivate, reinterpret_cast<void**>(&pWebViewPrivate_)) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " QueryInterface(IID_IWebViewPrivate) failed", ApHandlePrintf(hAp_)); }

  RECT r = { nLeft_, nTop_, nWidth_ + nLeft_, nHeight_ + nTop_ };
  if (FAILED( pWebView_->initWithFrame(r, 0, 0) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebView_->initWithFrame() failed", ApHandlePrintf(hAp_)); }

  // -----------------------------

  if (FAILED( pWebViewPrivate_->setFrameLoadDelegatePrivate(this) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebViewPrivate_->setFrameLoadDelegatePrivate() failed", ApHandlePrintf(hAp_)); }
  if (FAILED( pWebViewPrivate_->setTransparent(TRUE) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebViewPrivate_->setTransparent() failed", ApHandlePrintf(hAp_)); }
  if (FAILED( pWebViewPrivate_->setUsesLayeredWindow(TRUE) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebViewPrivate_->setUsesLayeredWindow() failed", ApHandlePrintf(hAp_)); }
  if (FAILED( pWebViewPrivate_->viewWindow(reinterpret_cast<OLE_HANDLE*>(&hWnd_))) ) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebViewPrivate_->viewWindow() failed", ApHandlePrintf(hAp_)); }
  if (hWnd_ == NULL) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebViewPrivate_->viewWindow() did not return hWnd", ApHandlePrintf(hAp_)); }

  // -----------------------------

  if (FAILED( pWebView_->mainFrame(&pWebFrame_) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebView_->mainFrame() failed", ApHandlePrintf(hAp_)); }

  ::MoveWindow(hWnd_, nLeft_, nTop_ - 10000, nWidth_, nHeight_, TRUE);
  ::ShowWindow(hWnd_, SW_SHOW);
  ::UpdateWindow(hWnd_);
}

void View::Destroy()  
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat "", ApHandlePrintf(hAp_)));

  //{
  //  Msg_WebView_Event_Closing msg;
  //  msg.hView = apHandle();
  //  msg.Send();
  //}

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
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " base=%s", ApHandlePrintf(hAp_), _sz(sBase)));
  serializedLoads_.LoadHtml(this, sHtml, sBase);
}

void View::Load(const String& sUrl)
{
  if (sUrl.contains("arena")) {
    int x = 1;
  }
  apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " url=%s", ApHandlePrintf(hAp_), _sz(sUrl)));
  serializedLoads_.Load(this, sUrl);
}

// static
void View::LoadDone()
{
  serializedLoads_.LoadDone();
}

// static
void View::TryLoad()
{
  serializedLoads_.TryLoad();
}

void View::SerializedLoadHtml(const String& sHtml, const String& sBase)
{
  if (!pWebFrame_) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebFrame_== 0", ApHandlePrintf(hAp_)); }

  sUrl_ = sBase; 

  if (1) {
    if (FAILED( pWebFrame_->loadHTMLString(::SysAllocString(sHtml), ::SysAllocString(sBase)) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebFrame_->loadRequest() failed", ApHandlePrintf(hAp_)); }
  }
  
  if (0) {
    BSTR bstrHTML = SysAllocString(TEXT("<p style=\"background-color: #00FF00\">Testing</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul><iframe src=\"http://www.wolfspelz.de\" style=\"width:100%;height:300px\" />"));
    pWebFrame_->loadHTMLString(bstrHTML, 0);
  }
}

#include <shlwapi.h>
#include <wininet.h>

void View::SerializedLoad(const String& sUrl)
{
  sUrl_ = sUrl;

  AutoComPtr<IWebMutableURLRequest> request;
  if (FAILED( WebKitCreateInstance(CLSID_WebMutableURLRequest, 0, IID_IWebMutableURLRequest, request) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " WebKitCreateInstance(CLSID_WebMutableURLRequest) failed", ApHandlePrintf(hAp_)); }

  if (1) {
    if (FAILED( request->initWithURL(::SysAllocString(sUrl), WebURLRequestUseProtocolCachePolicy, 60) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " request->initWithURL() failed", ApHandlePrintf(hAp_)); }
  }
  
  if (0) {
    BSTR bstrUrl = 0;
    //BSTR bstrUrl = SysAllocString(_T("http://www.wolfspelz.de"));
    if (!bstrUrl) {
      TCHAR szFileName[MAX_PATH];
      ::GetModuleFileName(NULL, szFileName, MAX_PATH);
      ::PathRemoveFileSpec(szFileName);
      ::PathAddBackslash(szFileName);
      //::PathAppend(szFileName, Apollo::getModuleResourcePath(MODULE_NAME) + "/test/frame.html");
      ::PathAppend(szFileName, Apollo::getModuleResourcePath(MODULE_NAME) + "/test/overlay.html");

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

    if (FAILED( request->initWithURL(bstrUrl, WebURLRequestUseProtocolCachePolicy, 60) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " request->initWithURL() failed", ApHandlePrintf(hAp_)); }
  }

  //if (FAILED( pWebFrame_->setAllowsScrolling(FALSE) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebFrame_->setAllowsScrolling() failed", ApHandlePrintf(hAp_)); }

  if (FAILED( pWebFrame_->loadRequest(request) )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebFrame_->loadRequest() failed", ApHandlePrintf(hAp_)); }
}

void View::Reload()
{
  if (FAILED( pWebFrame_->reloadFromOrigin() )) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pWebFrame_->reload() failed", ApHandlePrintf(hAp_)); }
}

//------------------------------------

void View::MoveWindowToCurrentRect()
{
#if defined(WIN32)
  //apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "############### %d,%d %dx%d", nLeft_, nTop_, nWidth_, nHeight_));
  ::MoveWindow(hWnd_, nLeft_ - (bVisible_ ? 0 : 0), nTop_ - (bVisible_ ? 0 : 10000), nWidth_, nHeight_, FALSE);
#endif // WIN32
}

void View::SetPosition(int nLeft, int nTop, int nWidth, int nHeight)
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " %d,%d %dx%d", ApHandlePrintf(hAp_), nLeft, nTop, nWidth, nHeight));

  nLeft_ = nLeft;
  nTop_ = nTop;
  nWidth_ = nWidth;
  nHeight_ = nHeight;

  MoveWindowToCurrentRect();
}

void View::SetVisibility(int bVisible)
{
  apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " %d", ApHandlePrintf(hAp_), bVisible));

  int bChanged = (bVisible_ != bVisible);
  bVisible_ = bVisible;

  if (bChanged) {
    MoveWindowToCurrentRect();

#if defined(WIN32)
    if (bVisible_) {
      // Force webkit repaint, which is suppressed while invisible
      RECT r = { 0, 0, nWidth_, nHeight_ };
      ::InvalidateRect(hWnd_, &r, FALSE);
    }
#endif // WIN32
  }
}

void View::SetWindowFlags(int nFlags)
{
  int bChanged = (nFlags_ != nFlags);
  nFlags_ = nFlags;

  if (bChanged) {
#if defined(WIN32)
    if (nFlags_ & Msg_WebView_SetWindowFlags::ToolWindow != 0) {
      // Remove from taskbar
      ::SetWindowLong(hWnd_, GWL_EXSTYLE, ::GetWindowLong(hWnd_, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
      //::ShowWindow(hWnd_, SW_SHOW);
      //::UpdateWindow(hWnd_);
            
      // Tried to set Parent (= Owner for toplevel windows) in order to remove it from taskbar, but failed
      // Even worse: Win32FirefoxBrowser::AdjustStackingOrder always finds the app main window 
      // on top and View::hWnd_(s) switch order all the time. This probably could be fixed, but 
      // if it is true, that owned windows always stack above the owner, then the owner feature
      // is useless for us, because we need View::hWnd_(s) to be in different positions in the stacking 
      // order, depending on their respective browser window.
      //::SetWindowLong(hWnd_, GWL_HWNDPARENT, (LONG) Msg_Win32_GetMainWindow::_());
      //HWND hWndOwner = ::GetWindow(hWnd_, GW_OWNER);
    }
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

  MoveWindowToCurrentRect();
}
  
void View::SizeBy(int nX, int nY, int nDirection)
{
  switch (nDirection) {
    case Msg_WebView_SizeBy::DirectionLeft:          nLeft_ += nX;               nWidth_ -= nX;                 break;
    case Msg_WebView_SizeBy::DirectionTop:                         nTop_ += nY;                 nHeight_ -= nY; break;
    case Msg_WebView_SizeBy::DirectionRight:                                     nWidth_ += nX;                 break;
    case Msg_WebView_SizeBy::DirectionBottom:                                                   nHeight_ += nY; break;
    case Msg_WebView_SizeBy::DirectionTopLeft:       nLeft_ += nX; nTop_ += nY;  nWidth_ -= nX; nHeight_ -= nY; break;
    case Msg_WebView_SizeBy::DirectionTopRight:                    nTop_ += nY;  nWidth_ += nX; nHeight_ -= nY; break;
    case Msg_WebView_SizeBy::DirectionBottomLeft:    nLeft_ += nX;               nWidth_ -= nX; nHeight_ += nY; break;
    case Msg_WebView_SizeBy::DirectionBottomRight:                               nWidth_ += nX; nHeight_ += nY; break;
  }

  //if (nWidth_ < 0) { nWidth_ = 0; }
  //if (nHeight_ < 0) { nHeight_ = 0; }

  // Prevent from hitting https://bugs.webkit.org/show_bug.cgi?id=60618
  if (nWidth_ < 1) { nWidth_ = 1; }
  if (nHeight_ < 1) { nHeight_ = 1; }

  MoveWindowToCurrentRect();
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

void View::GetWin32Window(HWND& hWnd)
{
  hWnd = hWnd_;
}

//------------------------------------
// Call JS

IWebFrame* View::GetFrameByPath(const String& sFramePath)
{
  if (pWebFrame_ == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " no main frame", ApHandlePrintf(hAp_)); }

  IWebFrame* pResult = 0;

  String sPath = sFramePath;
  sPath.trim("/");

  if (sPath.empty()) {
    pResult = pWebFrame_;
  } else {

    AutoComPtr<IWebFrame> frame = pWebFrame_;
    String sError;

    String sFrameId;
    while (sPath.nextToken("/", sFrameId) && sError.empty()) {
      AutoComPtr<IDOMDocument> doc;
      frame->DOMDocument(doc);
      if (doc.get() == 0) {
        sError = "No document";
      } else {
        BSTR bstrId = ::SysAllocString(sFrameId);
        AutoComPtr<IDOMElement> elem;
        doc->getElementById(bstrId, elem);
        if (elem.get() == 0) {
          sError.appendf("getElementById(%s) failed", _sz(sFrameId));
        } else {
          AutoComPtr<IDOMHTMLIFrameElement> frameElem;
          elem->QueryInterface(IID_IDOMHTMLIFrameElement, frameElem);
          if (frameElem.get() == 0) {
            sError.appendf("element '%s' is not an IFRAME", _sz(sFrameId));
          } else {
            AutoComPtr<IWebFrame> nextFrame;
            frameElem->contentFrame(nextFrame);
            frame = nextFrame;
          }
        }
        ::SysFreeString(bstrId);
      }
    }

    if (sError) {
      throw ApException(LOG_CONTEXT, "" ApHandleFormat " %s: %s", ApHandlePrintf(hAp_), _sz(sFramePath), _sz(sError));
    } else {
      pResult = frame.get();
    }
  }

  return pResult;
}

String View::GetElementValue(const String& sFramePath, const String& sElement, const String& sProperty)
{
  if (!bLoaded_) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " ('%s', %s.%s): document not yet loaded", ApHandlePrintf(hAp_), _sz(sFramePath), _sz(sElement), _sz(sProperty)); }
  String sResult;

  IWebFrame* pFrame = GetFrameByPath(sFramePath);
  if (pFrame != 0) {
    sResult = GetElementValue(pFrame, sElement, sProperty);
  }

  return sResult;
}

String View::CallScriptFunction(const String& sFramePath, const String& sFunction, List& lArgs)
{
  if (!bLoaded_) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " ('%s', %s):  document not yet loaded", ApHandlePrintf(hAp_), _sz(sFramePath), _sz(sFunction)); }
  String sResult;

  IWebFrame* pFrame = GetFrameByPath(sFramePath);
  if (pFrame != 0) {
    sResult = CallScriptFunction(pFrame, sFunction, lArgs);
  }

  return sResult;
}

String View::GetElementValue(IWebFrame* pFrame, const String& sElement, const String& sProperty)
{
  String sResult;

  JSGlobalContextRef runCtx = pFrame->globalContext();
  if (runCtx == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pFrame->globalContext() returned 0", ApHandlePrintf(hAp_)); }

  JSValueRef* exception = 0;

  JSObjectRef global = JSContextGetGlobalObject(runCtx);
  if (global == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSContextGetGlobalObject(runCtx) returned 0", ApHandlePrintf(hAp_)); }

  // Support only element id like '#ID'
  if (!sElement.startsWith("#")) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " Only CSS selectors with element Ids are supported, e.g. #id", ApHandlePrintf(hAp_)); }

  String sName = sElement;
  sName.trim("#");

  AutoJSStringRef elementName = JSStringCreateWithUTF8CString(sName);

  JSValueRef elementValue = JSObjectGetProperty(runCtx, global, elementName, exception);
  if (exception) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSObjectGetProperty('%s') returned exception", ApHandlePrintf(hAp_), _sz(sName)); }
  if (elementValue == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSObjectGetProperty('%s') returned 0", ApHandlePrintf(hAp_), _sz(sName)); }

  JSObjectRef element = JSValueToObject(runCtx, elementValue, exception);
  if (exception) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSValueToObject('%s') returned exception", ApHandlePrintf(hAp_), _sz(sName)); }
  if (element == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSValueToObject('%s') returned 0", ApHandlePrintf(hAp_), _sz(sName)); }

  AutoJSStringRef propertyName = JSStringCreateWithUTF8CString(sProperty);

  JSValueRef propertyValue = JSObjectGetProperty(runCtx, element, propertyName, exception);
  if (exception) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSObjectGetProperty('%s') returned exception", ApHandlePrintf(hAp_), _sz(sName)); }
  if (propertyValue == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSObjectGetProperty('%s') returned 0", ApHandlePrintf(hAp_), _sz(sName)); }

  // Convert the value into a string
  if (propertyValue) {
    AutoJSStringRef value = JSValueToStringCopy (runCtx, propertyValue, exception);
    if (exception) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSValueToStringCopy() returned exception", ApHandlePrintf(hAp_)); }
    if (((JSStringRef) value) == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSValueToStringCopy() returned 0", ApHandlePrintf(hAp_)); }

    sResult.set((PWSTR) JSStringGetCharactersPtr(value), JSStringGetLength(value));
  }

  return sResult;
}

String View::CallScriptFunction(IWebFrame* pFrame, const String& sFunction, List& lArgs)
{
  String sResult;

  AutoJSStringRef methodName = JSStringCreateWithUTF8CString(sFunction);

  JSGlobalContextRef runCtx = pFrame->globalContext();
  if (runCtx == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pFrame->globalContext() returned 0", ApHandlePrintf(hAp_)); }

  JSObjectRef global = JSContextGetGlobalObject(runCtx);
  if (global == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSContextGetGlobalObject(runCtx) returned 0", ApHandlePrintf(hAp_)); }

  JSValueRef* exception = 0;
  JSValueRef functionProperty = JSObjectGetProperty(runCtx, global, methodName, exception);
  if (exception) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSObjectGetProperty(methodName) returned exception", ApHandlePrintf(hAp_)); }
  if (functionProperty == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSObjectGetProperty(methodName) returned 0", ApHandlePrintf(hAp_)); }

  if (!JSValueIsObject(runCtx, functionProperty)) {
    throw ApException(LOG_CONTEXT, "" ApHandleFormat " no such callableFunction: '%s' url=%s", ApHandlePrintf(hAp_), _sz(sFunction), _sz(sUrl_));
  }

  JSObjectRef callableFunction = JSValueToObject(runCtx, functionProperty, exception);
  if (exception) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSValueToObject(functionProperty) returned exception", ApHandlePrintf(hAp_)); }
  if (callableFunction == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSValueToObject(functionProperty) returned 0", ApHandlePrintf(hAp_)); }

  JSValueRef result = 0;

  String sLog = sFunction;

  if (lArgs.length() > 0) {

    AutoPtr<JSValueRef> args(new JSValueRef[lArgs.length()]);
    int nCnt = 0;
    for (Elem* e = 0; (e = lArgs.Next(e)) != 0; ) {
      if (apLog_IsVerbose) { sLog += " " + e->getName(); }
      args[nCnt++] = JSValueMakeString(runCtx, JSStringCreateWithCharacters((LPCTSTR) e->getName(), e->getName().chars()));
    }

    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "%s", _sz(sLog)));
    result = JSObjectCallAsFunction(runCtx, callableFunction, global, lArgs.length(), args.get(), exception);
    if (exception) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSObjectCallAsFunction() returned exception", ApHandlePrintf(hAp_)); }

  } else {

    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "%s", _sz(sLog)));
    result = JSObjectCallAsFunction (runCtx, callableFunction, global, 0, 0, exception);
    if (exception) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSObjectCallAsFunction() returned exception", ApHandlePrintf(hAp_)); }

  }

  // Convert the result into a string
  if (result) {
    if (JSValueIsString(runCtx, result)) {
      AutoJSStringRef value = JSValueToStringCopy (runCtx, result, exception);
      if (exception) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSValueToStringCopy() returned exception", ApHandlePrintf(hAp_)); }

      sResult.set((PWSTR) JSStringGetCharactersPtr(value), JSStringGetLength(value));

    } else if (JSValueIsNumber(runCtx, result)) {
      double value = JSValueToNumber(runCtx, result, exception);
      if (exception) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSValueToNumber() returned exception", ApHandlePrintf(hAp_)); }

      sResult.appendf("%g", value);

    } else if (JSValueIsBoolean(runCtx, result))  {
      bool value = JSValueToBoolean(runCtx, result);
      if (exception) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSValueToBoolean() returned exception", ApHandlePrintf(hAp_)); }

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
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " Msg_WebView_CallScriptFunction: %s", ApHandlePrintf(hAp_), _sz(msg.sComment)); }

  response.fromString(msg.sResult);
  if (response.length() > 0) {
    int nStatus = response.getInt(Srpc::Key::Status);
    if (nStatus != 1) {
      throw ApException(LOG_CONTEXT, "" ApHandleFormat " Status=%d Message=%s", ApHandlePrintf(hAp_), nStatus, _sz(response.getString(Srpc::Key::Message)));
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

JSValueRef View::JS_Apollo_echoString(JSContextRef ctx, JSObjectRef callableFunction, JSObjectRef thisObject, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception)
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

JSValueRef View::JS_Apollo_sendMessage(JSContextRef ctx, JSObjectRef callableFunction, JSObjectRef thisObject, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception)
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

  if (sType == "WebView_ModuleCall") {
    // Special type -> send a standard from-display-to-view message

    Msg_WebView_ModuleCall msg;
    msg.hView = pView->apHandle();
    srpc >> msg.srpc;

    if (msg.Request()) {
      if (msg.response.getString(Srpc::Key::Status)) {
        // Has response
      } else {
        msg.response.createResponse(msg.srpc);
      }
    } else {
      msg.response.createError(msg.srpc, msg.sComment);
    }

    sResponse = msg.response.toString();

  } else if (sType) {
    // Other message type -> send SRPC with custom type

    ApSRPCMessage msg(sType);
    srpc >> msg.srpc;

    // Custom message handlers just do the apStatus thing.
    // They rely on someone else (us here) to fill msg.response.
    if (msg.Call()) {
      msg.response.createResponse(msg.srpc);
    } else {
      msg.response.createError(msg.srpc, msg.sComment);
    }

    sResponse = msg.response.toString();

  } else {
    // Handle SRPC message via SrpcGate with SRPCGATE_HANDLER_TYPE type

    ApSRPCMessage msg(SRPCGATE_HANDLER_TYPE);
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

void View::MakeScriptObject(IWebFrame* pFrame)
{
  if (!pFrame) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pFrame== 0"); }

  JSGlobalContextRef runCtx = pFrame->globalContext();
  if (runCtx == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " pFrame->globalContext() failed", ApHandlePrintf(hAp_)); }

  JSObjectRef pGlobal = JSContextGetGlobalObject(runCtx);
  if (pGlobal == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSContextGetGlobalObject() failed", ApHandlePrintf(hAp_)); }

  JSClassRef apolloClass = JS_Apollo_class();
  if (apolloClass == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JS_Apollo_class() failed", ApHandlePrintf(hAp_)); }

  pScriptObject_ = JSObjectMake(runCtx, apolloClass, reinterpret_cast<void*>(this));
  if (pScriptObject_ == 0) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSObjectMake() failed", ApHandlePrintf(hAp_)); }

  AutoJSStringRef apolloName = JSStringCreateWithUTF8CString("apollo");
  if (!apolloName) { throw ApException(LOG_CONTEXT, "" ApHandleFormat " JSStringCreateWithUTF8CString() failed", ApHandlePrintf(hAp_)); }

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

  sUrl = GetUrlFrom(request);

exit:
  return sUrl;
}

String View::GetUrlFrom(IWebURLRequest *request)
{
  String sUrl;

  BSTR bstrUrl = 0;
  if (FAILED( request->URL(&bstrUrl) )) goto exit;
  sUrl = StringFromBSTR(bstrUrl);
  if (bstrUrl != 0) { ::SysFreeString(bstrUrl); }

exit:
  return sUrl;
}

HRESULT View::didFinishDocumentLoadForFrame(IWebView *webView, IWebFrame *frame)
{
  String sUrl = GetUrlFrom(frame);

  try {
    MakeScriptObject(frame);
  } catch (ApException& ex) {
    apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " MakeScriptObject() failed: %s", ApHandlePrintf(hAp_), _sz(ex.getText())));
  }

  if (pTopLoadingFrame_ == frame) {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " DocumentLoaded %s", ApHandlePrintf(hAp_), _sz(sUrl)));
    bLoaded_ = 1;

    ApAsyncMessage<Msg_WebView_Event_DocumentLoaded> msg;
    msg->hView = apHandle();
    msg.Post();
  } else {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " EmbeddedDocumentLoaded %s", ApHandlePrintf(hAp_), _sz(sUrl)));

    ApAsyncMessage<Msg_WebView_Event_EmbeddedDocumentLoaded> msg;
    msg->hView = apHandle();
    msg->sUrl = sUrl;
    msg.Post();
  }

  return S_OK;
}

HRESULT View::didFinishLoadForFrame(IWebView* webView, IWebFrame* frame)
{
  String sUrl = GetUrlFrom(frame);

  if (pTopLoadingFrame_ == frame) {
    apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " DocumentComplete %s", ApHandlePrintf(hAp_), _sz(sUrl)));

    ApAsyncMessage<Msg_WebView_Event_DocumentComplete> msg;
    msg->hView = apHandle();
    msg.Post();
  } else {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " EmbeddedDocumentLoaded %s", ApHandlePrintf(hAp_), _sz(sUrl)));

    ApAsyncMessage<Msg_WebView_Event_EmbeddedDocumentComplete> msg;
    msg->hView = apHandle();
    msg->sUrl = sUrl;
    msg.Post();
  }

  return S_OK;
}

HRESULT View::didFailLoadingWithError(IWebView *webView, unsigned long identifier, IWebError *error, IWebDataSource *dataSource)
{
  if (pWebView_ == webView) {

    int code = 0;
    error->code(&code);

    BSTR bstrUrl = 0;
    error->failingURL(&bstrUrl);
    String sUrl = StringFromBSTR(bstrUrl);
    if (bstrUrl != 0) { ::SysFreeString(bstrUrl); }

    bool bIsError = true;
    if (code == -999) {
      //WebFrame.cpp:
      //ResourceError WebFrame::cancelledError(const ResourceRequest& request)
      //{
      //    // FIXME: Need ChickenCat to include CFNetwork/CFURLError.h to get these values
      //    // Alternatively, we could create our own error domain/codes.
      //    return ResourceError(String(WebURLErrorDomain), -999, request.url().string(), String());
      //}
      bIsError = false;
      apLog_Info((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " %s: %s", ApHandlePrintf(hAp_), _sz("Load cancelled"), _sz(sUrl)));
    }

    if (bIsError) {
      BSTR bstrDescription = 0;
      error->localizedDescription(&bstrDescription);
      String sDescription = StringFromBSTR(bstrDescription);
      if (bstrDescription != 0) { ::SysFreeString(bstrDescription); }

      if (false) { // throws E_NOTIMPL
        BSTR bstrLocalizedFailureReason = 0;
        error->localizedFailureReason(&bstrLocalizedFailureReason);
        String sLocalizedFailureReason = StringFromBSTR(bstrLocalizedFailureReason);
        if (bstrLocalizedFailureReason != 0) { ::SysFreeString(bstrLocalizedFailureReason); }
      }

      apLog_Warning((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " %s: %s", ApHandlePrintf(hAp_), _sz(sDescription), _sz(sUrl)));

      ApAsyncMessage<Msg_WebView_Event_LoadError> msg;
      msg->hView = apHandle();
      msg->sUrl = sUrl;
      msg->sDescription = sDescription;
      msg.Post();
    }
  }

  return S_OK;
}

HRESULT View::willSendRequest(IWebView *webView, unsigned long identifier, IWebURLRequest *request, IWebURLResponse *redirectResponse, IWebDataSource *dataSource, IWebURLRequest **newRequest)
{
  int bChanged = 0;

  Msg_WebView_Event_BeforeRequest msg;

  if (pWebView_ == webView) {
    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " %s", ApHandlePrintf(hAp_), _sz(GetUrlFrom(request))));

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
      apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "" ApHandleFormat " deny %s", ApHandlePrintf(hAp_), _sz(GetUrlFrom(request))));
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

HRESULT View::webViewAddMessageToConsole(IWebView *webView, BSTR message, int lineNumber, BSTR url, BOOL isError)
{
  String sFile = StringFromBSTR(url);
  String sMessage = StringFromBSTR(message);

  if (isError) {
    apLog_Error((LOG_CHANNEL, "Javascript:", "" ApHandleFormat " %s line %d: %s", ApHandlePrintf(hAp_), _sz(sFile), lineNumber, _sz(sMessage)));
  } else {
    apLog_Warning((LOG_CHANNEL, "Javascript:", "" ApHandleFormat " %s line %d: %s", ApHandlePrintf(hAp_), _sz(sFile), lineNumber, _sz(sMessage)));
  }

  return S_OK;
}

HRESULT View::webViewReceivedFocus(IWebView *webView)
{
  if (pWebView_ == webView) {
    Msg_WebView_Event_ReceivedFocus msg;
    msg.hView = apHandle();
    msg.Send();
  }

  return E_NOTIMPL;
}

HRESULT View::webViewLostFocus(IWebView *webView, OLE_HANDLE loseFocusTo)
{
  if (pWebView_ == webView) {
    Msg_WebView_Event_LostFocus msg;
    msg.hView = apHandle();
    msg.Send();
  }

  return E_NOTIMPL;
}

HRESULT View::webViewClosing(IWebView *webView)
{
  // In View::Destroy after pWebView_ = 0
  // Do not check webView pointer, just trust WebKit that this makes sense.
  {
    Msg_WebView_Event_Closing msg;
    msg.hView = apHandle();
    msg.Send();
  }

  return E_NOTIMPL;
}

//HRESULT View::willCloseFrame(IWebView* webView, IWebFrame* frame)
//{
//  if (pTopLoadingFrame_ == frame) {
//    Msg_WebView_Event_Closing msg;
//    msg.hView = apHandle();
//    msg.Send();
//
//    apLog_Verbose((LOG_CHANNEL, LOG_CONTEXT, "%s", _sz(GetUrlFrom(frame))));
//  }
//
//  return S_OK;
//}
