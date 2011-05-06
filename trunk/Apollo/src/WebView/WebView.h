// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(WebView_H_INCLUDED)
#define WebView_H_INCLUDED

#include <WebKit/WebKit.h>
#include <WebKit/WebKitCOMAPI.h>

class WebView
{
public:
  WebView(const ApHandle& hWebView);
  virtual ~WebView();

  inline ApHandle apHandle() { return hAp_; }

  int Create(const String& sHtml, const String& sBase);
  void Destroy();
  void SetPosition(int nX, int nY, int nW, int nH);
  void SetVisibility(int bVisible);

protected:
  ApHandle hAp_;
  int bVisible_;
  int nX_;
  int nY_;
  int nW_;
  int nH_;

  IWebView* pWebKit_;
#if defined(WIN32)
  HWND hWnd_;
#endif // WIN32

#if defined(AP_TEST)
  friend class WebViewModuleTester;
#endif
};

#endif // WebView_H_INCLUDED
