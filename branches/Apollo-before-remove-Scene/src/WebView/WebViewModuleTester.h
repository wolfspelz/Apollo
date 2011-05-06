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

  static String Basic();

  static ApHandle hWebView_;
};
#endif

#endif // WebViewModuleTester_H_INCLUDED
