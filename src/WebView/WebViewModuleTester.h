// ============================================================================
//
// Apollo
//
// ============================================================================

#if !defined(WebViewModuleTester_H_INCLUDED)
#define WebViewModuleTester_H_INCLUDED

#include "Apollo.h"
#include "SString.h"

#if defined(AP_TEST)
class WebViewModuleTester
{
public:
  static void Begin();
  static void Execute();
  static void End();

  static String LoadHtml();
  static String LoadFromNonAsciiPath();
  static String CallJSEcho();
  static String CallSystemEcho();
  static String CallCustomEcho();
  static String FrameIO();
  static String StartManuallySerialized();
  static String StartAutomaticallySerialized();
  static String GetDomElement();
  static String Dev();
};
#endif

#endif // WebViewModuleTester_H_INCLUDED
