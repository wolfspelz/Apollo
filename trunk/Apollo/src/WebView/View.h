// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(View_H_INCLUDED)
#define View_H_INCLUDED

#include <WebKit/WebKit.h>
#include <WebKit/WebKitCOMAPI.h>
#include <JavaScriptCore/JavaScriptCore.h>

//------------------------------------

template<typename T> class AutoComPtr
{
public:
  AutoComPtr() : p_(0) { }
  AutoComPtr(T* p) : p_(p) { if (p_) p_->AddRef(); }
  AutoComPtr(const AutoComPtr& o) : p_(o.p_) { if (T* p = p_) p->AddRef(); }
  ~AutoComPtr() { if (p_) p_->Release(); }

  // Handover
  inline AutoComPtr& operator=(AutoComPtr& o)
  {
    // Decrement old if already occupied
    if (p_) { p_->Release(); }
    p_ = o.p_;
    // Increment because additional pointer
    p_->AddRef();
    return *this;
  }

  inline T* get() const { return p_; }
  inline T& operator*() const { return *p_; }
  inline T* operator->() const { return p_; }
  inline operator T* () { return p_; }

  // These 2 are dangerous, because they allow overwriting the pointer 
  // without Release. They must not be used if already occupied. 
  inline operator T** () { if (p_) { AP_DEBUG_BREAK(); } return &p_; }
  inline operator void** () { if (p_) { AP_DEBUG_BREAK(); } return (void**) &p_; }

  inline bool operator!() const { return !p_; }
  inline bool operator==(const AutoComPtr<T>& o) { return get() == o.get(); }

private:
  T* p_;
};

class AutoJSStringRef
{
public:
  AutoJSStringRef(JSStringRef p) : p_(p) {}
  ~AutoJSStringRef() { if (p_) JSStringRelease(p_); }
  inline operator JSStringRef () { return p_; }
private:
  JSStringRef p_;
};

//------------------------------------

class View;

class SerializedTask : public Elem
{
public:
  SerializedTask(View* pView)
    :pView_(pView)
  {}
  virtual void Execute() = 0;
  virtual String Description() = 0;

  View* pView_;
};

class SerializedLoadTask : public SerializedTask
{
public:
  SerializedLoadTask(View* pView, const String& sUrl)
    :SerializedTask(pView)
    ,sUrl_(sUrl)
  {}
  void Execute();
  String Description() { return String("URL:") + sUrl_; }

protected:
  String sUrl_;
};

class SerializedLoadHtmlTask : public SerializedTask
{
public:
  SerializedLoadHtmlTask(View* pView, const String& sHtml, const String& sBase)
    :SerializedTask(pView)
    ,sHtml_(sHtml)
    ,sBase_(sBase)
  {}
  void Execute();
  String Description() { return String("HTML:") + sBase_; }

protected:
  String sHtml_;
  String sBase_;
};

class SerializedLoadTaskList : public ListT<SerializedTask, Elem>
{
public:
  SerializedLoadTaskList()
    :bLocked_(0)
  {}

  void Load(View* pView, const String& sUrl);
  void LoadHtml(View* pView, const String& sHtml, const String& sBase);
  void TryLoad();
  void LoadDone();
  void ViewDeleted(View* pView);

protected:
  int bLocked_;
  Apollo::TimeValue tvLocked_;
  String sCurrent_;
};

//------------------------------------

class View : public IWebUIDelegate, IWebUIDelegatePrivate, IWebFrameLoadDelegate, IWebFrameLoadDelegatePrivate, IWebResourceLoadDelegate, IWebPolicyDelegate
{
public:
  View(const ApHandle& hView);
  virtual ~View();

  inline ApHandle apHandle() { return hAp_; }

  void Create(int nLeft, int nTop, int nWidth, int nHeight);
  void Destroy();

  void LoadHtml(const String& sHtml, const String& sBase) throw(ApException);
  void Load(const String& sUrl) throw(ApException);
  void Reload() throw(ApException);
  String CallScriptFunction(const String& sFramePath, const String& sFunction, List& lArgs) throw(ApException);
  String GetElementValue(const String& sFramePath, const String& sElement, const String& sProperty) throw(ApException);
  void CallJsSrpc(const String& sFunction, Apollo::SrpcMessage& srpc, Apollo::SrpcMessage& response) throw(ApException);

  static void LoadDone();
  static void TryLoad();

  void SetPosition(int nLeft, int nTop, int nWidth, int nHeight);
  void SetVisibility(int bVisible);
  void SetWindowFlags(int nFlags);
  void SetScriptAccessPolicy(const String& sPolicy);
  void SetNavigationPolicy(const String& sPolicy);
  void MoveBy(int nX, int nY);
  void SizeBy(int nX, int nY, int nDirection);
  void MakeFrontWindow();
  void MouseCapture();
  void MouseRelease();
  void GetPosition(int& nLeft, int& nTop, int& nWidth, int& nHeight);
  void GetVisibility(int& bVisible);
  void GetWin32Window(HWND& hWnd);

  static JSValueRef JS_Apollo_getSharedValue(JSContextRef ctx, JSObjectRef thisObject, JSStringRef propertyName, JSValueRef* exception);
  static JSValueRef JS_Apollo_echoString(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception);
  static JSValueRef JS_Apollo_sendMessage(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception);
  int HasScriptAccess() { return bScriptAccessEnabled_; }
  int HasNavigation() { return bNavigationEnabled_; }

protected:
  void SerializedLoadHtml(const String& sHtml, const String& sBase) throw(ApException);
  void SerializedLoad(const String& sUrl) throw(ApException);
  void MakeScriptObject(IWebFrame* pFrame) throw(ApException);
  IWebFrame* GetFrameByPath(const String& sFramePath) throw(ApException);
  String CallScriptFunction(IWebFrame* pFrame, const String& sFunction, List& lArgs) throw(ApException);
  String GetElementValue(IWebFrame* pFrame, const String& sElement, const String& sProperty) throw(ApException);

  void MoveWindowToCurrentRect();
  static String StringFromBSTR(BSTR bStr);
  static String GetUrlFrom(IWebFrame *frame);
  static String GetUrlFrom(IWebURLRequest *request);

protected:
  ApHandle hAp_;
  int bLoaded_;
  int bVisible_;
  int nLeft_;
  int nTop_;
  int nWidth_;
  int nHeight_;

  int nFlags_;
  String sUrl_;
  int bScriptAccessEnabled_;
  int bNavigationEnabled_;

  static SerializedLoadTaskList serializedLoads_;
  friend class SerializedLoadTask;
  friend class SerializedLoadHtmlTask;

  IWebView* pWebView_;
  IWebFrame* pWebFrame_;
  IWebViewPrivate* pWebViewPrivate_;
  HWND hWnd_;
  int nRefCount_;

  IWebFrame* pTopLoadingFrame_;
  JSObjectRef pScriptObject_;

#if defined(AP_TEST)
  friend class WebViewModuleTester;
#endif

public:

  // IUnknown
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
  virtual ULONG STDMETHODCALLTYPE AddRef(void);
  virtual ULONG STDMETHODCALLTYPE Release(void);

  // IWebUIDelegate
  virtual HRESULT STDMETHODCALLTYPE createWebViewWithRequest(IWebView*, IWebURLRequest*, IWebView**) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewShow(IWebView*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewClose(IWebView*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewFocus(IWebView*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewUnfocus(IWebView*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewFirstResponder(IWebView*, OLE_HANDLE*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE makeFirstResponder(IWebView*, OLE_HANDLE) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE setStatusText(IWebView*, BSTR) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewStatusText(IWebView*, BSTR*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewAreToolbarsVisible(IWebView*, BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE setToolbarsVisible(IWebView*, BOOL) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewIsStatusBarVisible(IWebView*, BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE setStatusBarVisible(IWebView*, BOOL) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewIsResizable(IWebView*, BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE setResizable(IWebView*, BOOL) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE setFrame(IWebView*, RECT*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewFrame(IWebView*, RECT*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE setContentRect(IWebView*, RECT*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewContentRect(IWebView*, RECT*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE runJavaScriptAlertPanelWithMessage(IWebView* sender, BSTR message) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE runJavaScriptConfirmPanelWithMessage(IWebView*, BSTR, BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE runJavaScriptTextInputPanelWithPrompt(IWebView*, BSTR, BSTR, BSTR*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE runBeforeUnloadConfirmPanelWithMessage(IWebView*, BSTR, IWebFrame*, BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE runOpenPanelForFileButtonWithResultListener(IWebView*, IWebOpenPanelResultListener*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE mouseDidMoveOverElement(IWebView*, IPropertyBag*, UINT) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE contextMenuItemsForElement(IWebView*, IPropertyBag*, OLE_HANDLE, OLE_HANDLE*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE validateUserInterfaceItem(IWebView*, UINT, BOOL, BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE shouldPerformAction(IWebView*, UINT, UINT) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE dragDestinationActionMaskForDraggingInfo(IWebView*, IDataObject*, WebDragDestinationAction*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE willPerformDragDestinationAction(IWebView*, WebDragDestinationAction, IDataObject*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE dragSourceActionMaskForPoint(IWebView*, LPPOINT, WebDragSourceAction*) { return E_NOTIMPL; };
  virtual HRESULT STDMETHODCALLTYPE willPerformDragSourceAction(IWebView*, WebDragSourceAction, LPPOINT, IDataObject*, IDataObject**);// { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE contextMenuItemSelected(IWebView*, void*, IPropertyBag*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE hasCustomMenuImplementation(BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE trackCustomPopupMenu(IWebView*, OLE_HANDLE, LPPOINT) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE measureCustomMenuItem(IWebView*, void*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE drawCustomMenuItem(IWebView*, void*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE addCustomMenuDrawingData(IWebView*, OLE_HANDLE) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE cleanUpCustomMenuDrawingData(IWebView*, OLE_HANDLE) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE canTakeFocus(IWebView*, BOOL, BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE takeFocus(IWebView*, BOOL) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE registerUndoWithTarget(IWebUndoTarget*, BSTR, IUnknown*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE removeAllActionsWithTarget(IWebUndoTarget*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE setActionTitle(BSTR) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE undo() { return E_NOTIMPL; }    
  virtual HRESULT STDMETHODCALLTYPE redo() { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE canUndo(BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE canRedo(BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE printFrame(IWebView*, IWebFrame*) { return E_NOTIMPL; }    
  virtual HRESULT STDMETHODCALLTYPE ftpDirectoryTemplatePath(IWebView*, BSTR*) { return E_NOTIMPL; }    
  virtual HRESULT STDMETHODCALLTYPE webViewHeaderHeight(IWebView*, float*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewFooterHeight(IWebView*, float*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE drawHeaderInRect(IWebView*, RECT*, OLE_HANDLE) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE drawFooterInRect(IWebView*, RECT*, OLE_HANDLE, UINT, UINT) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewPrintingMarginRect(IWebView*, RECT*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE canRunModal(IWebView*, BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE createModalDialog(IWebView*, IWebURLRequest*, IWebView**) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE runModal(IWebView*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE isMenuBarVisible(IWebView*, BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE setMenuBarVisible(IWebView*, BOOL) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE runDatabaseSizeLimitPrompt(IWebView*, BSTR, IWebFrame*, BOOL*) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE paintCustomScrollbar(IWebView*, HDC, RECT, WebScrollBarControlSize, WebScrollbarControlState, WebScrollbarControlPart, BOOL, float, float, WebScrollbarControlPartMask) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE paintCustomScrollCorner(IWebView*, HDC, RECT) { return E_NOTIMPL; }

  // IWebFrameLoadDelegate
  virtual HRESULT STDMETHODCALLTYPE didStartProvisionalLoadForFrame(IWebView* webView, IWebFrame* frame);// { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didReceiveServerRedirectForProvisionalLoadForFrame(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didFailProvisionalLoadWithError(IWebView *webView, IWebError *error, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didCommitLoadForFrame(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didReceiveTitle(IWebView *webView, BSTR title, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didChangeIcons(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didReceiveIcon(IWebView *webView, OLE_HANDLE hBitmap, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didFinishLoadForFrame(IWebView* webView, IWebFrame* frame);// { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didFailLoadWithError(IWebView *webView, IWebError *error, IWebFrame *forFrame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didChangeLocationWithinPageForFrame(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE willPerformClientRedirectToURL(IWebView *webView, BSTR url, double delaySeconds, DATE fireDate, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didCancelClientRedirectForFrame(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE willCloseFrame(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE windowScriptObjectAvailable(IWebView *webView, JSContextRef context, JSObjectRef windowScriptObject) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didClearWindowObject(IWebView *webView, JSContextRef context, JSObjectRef windowScriptObject, IWebFrame *frame) { return S_OK; }

  // IWebFrameLoadDelegatePrivate
  virtual HRESULT STDMETHODCALLTYPE didFinishDocumentLoadForFrame(IWebView *sender, IWebFrame *frame);// { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didFirstLayoutInFrame(IWebView *sender, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didHandleOnloadEventsForFrame(IWebView *sender, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didFirstVisuallyNonEmptyLayoutInFrame(IWebView *sender, IWebFrame *frame) { return S_OK; }

  // IWebResourceLoadDelegate
  virtual HRESULT STDMETHODCALLTYPE identifierForInitialRequest(IWebView *webView, IWebURLRequest *request, IWebDataSource *dataSource, unsigned long identifier) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE willSendRequest(IWebView *webView, unsigned long identifier, IWebURLRequest *request, IWebURLResponse *redirectResponse, IWebDataSource *dataSource, IWebURLRequest **newRequest);// { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didReceiveAuthenticationChallenge(IWebView *webView, unsigned long identifier, IWebURLAuthenticationChallenge *challenge, IWebDataSource *dataSource) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didCancelAuthenticationChallenge(IWebView *webView, unsigned long identifier, IWebURLAuthenticationChallenge *challenge, IWebDataSource *dataSource) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didReceiveResponse(IWebView *webView, unsigned long identifier, IWebURLResponse *response, IWebDataSource *dataSource) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didReceiveContentLength(IWebView *webView, unsigned long identifier, UINT length, IWebDataSource *dataSource) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didFinishLoadingFromDataSource(IWebView *webView, unsigned long identifier, IWebDataSource *dataSource) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didFailLoadingWithError(IWebView *webView, unsigned long identifier, IWebError *error, IWebDataSource *dataSource);// { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE plugInFailedWithError(IWebView *webView, IWebError *error, IWebDataSource *dataSource) { return S_OK; }

  // IWebPolicyDelegate
  virtual HRESULT STDMETHODCALLTYPE decidePolicyForNavigationAction(IWebView *webView, IPropertyBag *actionInformation, IWebURLRequest *request, IWebFrame *frame, IWebPolicyDecisionListener *listener);// { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE decidePolicyForNewWindowAction(IWebView *webView, IPropertyBag *actionInformation, IWebURLRequest *request, BSTR frameName, IWebPolicyDecisionListener *listener) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE decidePolicyForMIMEType(IWebView *webView, BSTR type, IWebURLRequest *request, IWebFrame *frame, IWebPolicyDecisionListener *listener) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE unableToImplementPolicyWithError(IWebView *webView, IWebError *error, IWebFrame *frame) { return E_NOTIMPL; }

  // IWebUIDelegatePrivate
  virtual HRESULT STDMETHODCALLTYPE unused1() { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE unused2() { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE unused3() { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewScrolled(IWebView *sender) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewAddMessageToConsole(IWebView *sender, BSTR message, int lineNumber, BSTR url, BOOL isError);// { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewShouldInterruptJavaScript(IWebView *sender, /* [retval][out] */ BOOL *result) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewReceivedFocus(IWebView *sender);// { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewLostFocus(IWebView *sender, OLE_HANDLE loseFocusTo);// { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE doDragDrop(IWebView *sender, IDataObject *dataObject, IDropSource *dropSource, DWORD okEffect, /* [retval][out] */ DWORD *performedEffect) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewGetDlgCode(IWebView *sender, UINT keyCode, /* [retval][out] */ LONG_PTR *code) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewPainted(IWebView *sender) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE exceededDatabaseQuota(IWebView *sender, IWebFrame *frame, IWebSecurityOrigin *origin, BSTR databaseIdentifier) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE embeddedViewWithArguments(IWebView *sender, IWebFrame *frame, IPropertyBag *arguments, /* [retval][out] */ IWebEmbeddedView **view) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewClosing(IWebView *sender);// { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewSetCursor(IWebView *sender, OLE_HANDLE cursor) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewDidInvalidate(IWebView *sender) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE desktopNotificationsDelegate(/* [out] */ IWebDesktopNotificationsDelegate** result) { return E_NOTIMPL; }

};

#endif // View_H_INCLUDED
