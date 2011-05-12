// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(WebView_H_INCLUDED)
#define WebView_H_INCLUDED

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

  inline T* get() const { return p_; }
  inline T& operator*() const { return *p_; }
  inline T* operator->() const { return p_; }
  inline operator T* () { return p_; }
  inline operator T** () { return &p_; }
  inline operator void** () { return (void**) &p_; }
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

class View : public IWebUIDelegate, IWebUIDelegatePrivate, IWebFrameLoadDelegate, IWebFrameLoadDelegatePrivate, IWebResourceLoadDelegate, IWebPolicyDelegate
{
public:
  View::View(const ApHandle& hView)
    :hAp_(hView)
    ,bVisible_(0)
    ,nLeft_(100)
    ,nTop_(100)
    ,nWidth_(600)
    ,nHeight_(400)
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
  virtual ~View();

  inline ApHandle apHandle() { return hAp_; }

  void Create();
  void Destroy();

  void LoadHtml(const String& sHtml, const String& sBase) throw(ApException);
  void Load(const String& sUrl) throw(ApException);
  void Reload() throw(ApException);
  String CallJsFunction(const String& sFunction, List& lArgs) throw(ApException);
  void CallJsSrpc(const String& sFunction, Apollo::SrpcMessage& srpc, Apollo::SrpcMessage& response) throw(ApException);

  void SetPosition(int nX, int nY, int nW, int nH);
  void SetVisibility(int bVisible);
  void SetScriptAccessPolicy(const String& sPolicy);
  void SetNavigationPolicy(const String& sPolicy);
  void MoveBy(int nX, int nY);
  void SizeBy(int nW, int nH, int nDirection);
  void MouseCapture();
  void MouseRelease();
  void GetPosition(int& nX, int& nY, int& nW, int& nH);
  void GetVisibility(int& bVisible);

  static JSValueRef JS_Apollo_getSharedValue(JSContextRef ctx, JSObjectRef thisObject, JSStringRef propertyName, JSValueRef* exception);
  static JSValueRef JS_Apollo_echoString(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception);
  static JSValueRef JS_Apollo_sendMessage(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception);
  int HasScriptAccess() { return bScriptAccessEnabled_; }
  int HasNavigation() { return bNavigationEnabled_; }

protected:
  void MakeScriptObject() throw(ApException);
  static String StringFromBSTR(BSTR bStr);
  static String GetUrlFrom(IWebFrame *frame);
  static String GetUrlFrom(IWebURLRequest *request);

protected:
  ApHandle hAp_;
  int bVisible_;
  int nLeft_;
  int nTop_;
  int nWidth_;
  int nHeight_;

  String sUrl_;
  int bScriptAccessEnabled_;
  int bNavigationEnabled_;

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
  virtual HRESULT STDMETHODCALLTYPE didFailLoadingWithError(IWebView *webView, unsigned long identifier, IWebError *error, IWebDataSource *dataSource) { return S_OK; }
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
  virtual HRESULT STDMETHODCALLTYPE webViewScrolled( /* [in] */ IWebView *sender) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewAddMessageToConsole(/* [in] */ IWebView *sender, /* [in] */ BSTR message, /* [in] */ int lineNumber, /* [in] */ BSTR url, /* [in] */ BOOL isError);// { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewShouldInterruptJavaScript(/* [in] */ IWebView *sender, /* [retval][out] */ BOOL *result) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewReceivedFocus(/* [in] */ IWebView *sender) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewLostFocus(/* [in] */ IWebView *sender, /* [in] */ OLE_HANDLE loseFocusTo) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE doDragDrop(/* [in] */ IWebView *sender, /* [in] */ IDataObject *dataObject, /* [in] */ IDropSource *dropSource, /* [in] */ DWORD okEffect, /* [retval][out] */ DWORD *performedEffect) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewGetDlgCode(/* [in] */ IWebView *sender, /* [in] */ UINT keyCode, /* [retval][out] */ LONG_PTR *code) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewPainted(/* [in] */ IWebView *sender) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE exceededDatabaseQuota(/* [in] */ IWebView *sender, /* [in] */ IWebFrame *frame, /* [in] */ IWebSecurityOrigin *origin, /* [in] */ BSTR databaseIdentifier) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE embeddedViewWithArguments(/* [in] */ IWebView *sender, /* [in] */ IWebFrame *frame, /* [in] */ IPropertyBag *arguments, /* [retval][out] */ IWebEmbeddedView **view) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewClosing(/* [in] */ IWebView *sender) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewSetCursor(/* [in] */ IWebView *sender, /* [in] */ OLE_HANDLE cursor) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE webViewDidInvalidate(/* [in] */ IWebView *sender) { return E_NOTIMPL; }
  virtual HRESULT STDMETHODCALLTYPE desktopNotificationsDelegate(/* [out] */ IWebDesktopNotificationsDelegate** result) { return E_NOTIMPL; }

};

#endif // WebView_H_INCLUDED
