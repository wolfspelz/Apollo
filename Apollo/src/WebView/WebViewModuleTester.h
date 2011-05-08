// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(WebViewModuleTester_H_INCLUDED)
#define WebViewModuleTester_H_INCLUDED

#include "Apollo.h"
#include "SString.h"
#include "MsgWebView.h"

#if defined(AP_TEST)
class WebViewModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String LoadHtml();
  static String CallJSEcho();

  static void On_LoadHtml_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg);
  static void On_CallJSEcho_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg);
  static void On_CallJSEcho_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg);
};
#endif

#endif // WebViewModuleTester_H_INCLUDED
