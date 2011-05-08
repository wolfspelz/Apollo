// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(WebView_H_INCLUDED)
#define WebView_H_INCLUDED

#include <WebKit/WebKit.h>
#include <WebKit/WebKitCOMAPI.h>

//template<typename T> class SComPtr
//{
//public:
//  SComPtr() : p_(0) { }
//  SComPtr(T* p) : p_(p) { if (p_) p_->AddRef(); }
//  SComPtr(const SComPtr& o) : p_(o.p_) { if (T* p = p_) p->AddRef(); }
//  ~SComPtr() { if (p_) p_->Release(); }
//
//  inline T* get() const { return p_; }
//  inline T& operator*() const { return *p_; }
//  inline T* operator->() const { return p_; }
//  inline T** operator&() { return &p_; }
//  inline bool operator!() const { return !p_; }
//  inline bool operator==(const SComPtr<T>& o) { return get() == o.get(); }
//
//private:
//  T* p_;
//};

//------------------------------------

class WebView : public IWebUIDelegate, IWebFrameLoadDelegate, IWebFrameLoadDelegatePrivate
{
public:
  WebView::WebView(const ApHandle& hWebView)
    :hAp_(hWebView)
    ,bVisible_(0)
    ,nX_(100)
    ,nY_(100)
    ,nW_(600)
    ,nH_(400)
    #if defined(WIN32)
    ,pWebView_(0)
    ,pWebFrame_(0)
    ,pWebViewPrivate_(0)
    ,hWnd_(NULL)
    ,nRefCount_(0)
    ,pTopLoadingFrame_(0)
    ,bDocumentLoaded_(0)
    ,pScriptObject_(0)
    #endif // WIN32
  {}
  virtual ~WebView();

  inline ApHandle apHandle() { return hAp_; }

  int Create();
  void Destroy();

  void SetPosition(int nX, int nY, int nW, int nH);
  void SetVisibility(int bVisible);

  int LoadHtml(const String& sHtml, const String& sBase);
  int Load(const String& sUrl);

  String CallJSFunction(const String& sFunction, List& lArgs);

protected:
  void MakeScriptObject();
  static String StringFromBSTR(BSTR bStr);

protected:
  ApHandle hAp_;
  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;

#if defined(WIN32)
  IWebView* pWebView_;
  IWebFrame* pWebFrame_;
  IWebViewPrivate* pWebViewPrivate_;
  HWND hWnd_;
  int nRefCount_;

  IWebFrame* pTopLoadingFrame_;
  int bDocumentLoaded_;
  JSObjectRef pScriptObject_;
#endif // WIN32

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
  virtual HRESULT STDMETHODCALLTYPE willPerformDragSourceAction(IWebView*, WebDragSourceAction, LPPOINT, IDataObject*, IDataObject**) { return E_NOTIMPL; }
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
  virtual HRESULT STDMETHODCALLTYPE didStartProvisionalLoadForFrame(IWebView* webView, IWebFrame* frame);
  virtual HRESULT STDMETHODCALLTYPE didReceiveServerRedirectForProvisionalLoadForFrame(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didFailProvisionalLoadWithError(IWebView *webView, IWebError *error, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didCommitLoadForFrame(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didReceiveTitle(IWebView *webView, BSTR title, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didChangeIcons(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didReceiveIcon(IWebView *webView, OLE_HANDLE hBitmap, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didFinishLoadForFrame(IWebView* webView, IWebFrame* frame);
  virtual HRESULT STDMETHODCALLTYPE didFailLoadWithError(IWebView *webView, IWebError *error, IWebFrame *forFrame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didChangeLocationWithinPageForFrame(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE willPerformClientRedirectToURL(IWebView *webView, BSTR url, double delaySeconds, DATE fireDate, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didCancelClientRedirectForFrame(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE willCloseFrame(IWebView *webView, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE windowScriptObjectAvailable(IWebView *webView, JSContextRef context, JSObjectRef windowScriptObject)  { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didClearWindowObject(IWebView *webView, JSContextRef context, JSObjectRef windowScriptObject, IWebFrame *frame) { return S_OK; }

  // IWebFrameLoadDelegatePrivate
  virtual HRESULT STDMETHODCALLTYPE didFinishDocumentLoadForFrame(IWebView *sender, IWebFrame *frame);
  virtual HRESULT STDMETHODCALLTYPE didFirstLayoutInFrame(IWebView *sender, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didHandleOnloadEventsForFrame(IWebView *sender, IWebFrame *frame) { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE didFirstVisuallyNonEmptyLayoutInFrame(IWebView *sender, IWebFrame *frame) { return S_OK; }

};

#endif // WebView_H_INCLUDED