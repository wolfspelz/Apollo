// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(View_H_INCLUDED)
#define View_H_INCLUDED

//------------------------------------

class View
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

  int HasScriptAccess() { return bScriptAccessEnabled_; }
  int HasNavigation() { return bNavigationEnabled_; }

protected:
  void MoveWindowToCurrentRect();

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

  QWebView* pQWebView_;
  HWND hWnd_;
  int nRefCount_;

#if defined(AP_TEST)
  friend class QWebViewModuleTester;
#endif

};

#endif // View_H_INCLUDED
