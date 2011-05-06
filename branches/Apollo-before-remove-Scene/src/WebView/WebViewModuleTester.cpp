// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Local.h"
#include "ApTypes.h"
#include "MsgWebView.h"
#include "MsgUnitTest.h"
#include "WebViewModuleTester.h"
#include "WebView.h"

#if defined(AP_TEST)

ApHandle WebViewModuleTester::hWebView_;

String WebViewModuleTester::Basic()
{
  String s;

  ApHandle hWebView = Apollo::newHandle();
  hWebView_ = hWebView;

  //if (!s) { if (!Msg_WebView_Create::_(hWebView, "<p style=\"background-color: #00FF00\">Testing " "\xe9\x87\x91" "</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul><iframe src=\"http://www.wolfspelz.de\" style=\"width:100%;height:300px\" />")) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_Create::_(hWebView, 
    "<html>"
    "<head>"
    "<style>"
    "* { margin:0; padding:0; }"
    "</style>"
    "</head>"
    "<body style='overflow:hidden; padding:2px;'>"
    "<div style='width:100%; height:100%; border:1px solid #000000;'>"
    "<p style='background-color:#00FF00;'>Testing " "\xe9\x87\x91" "</p>"
    "<img src='http://webkit.org/images/icon-gold.png'>"
    "<img src='test/test1.png'>"
    "<div>"
    "</body>"
    "</html>"
    , "file://" + Apollo::getAppBasePath()
    )) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_Position::_(hWebView, 200, 200, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hWebView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  // ------------------------

  // ------------------------

  if (0) {
    if (!s) { if (!Msg_WebView_Destroy::_(hWebView)) { s = "Msg_WebView_Destroy failed"; }}
  }

  return s;
}

//----------------------------------------------------------

void WebViewModuleTester::Begin()
{
  //{ Msg_WebView_MouseEvent msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_WebView_MouseEvent, 0, ApCallbackPosNormal); }

  AP_UNITTEST_REGISTER(WebViewModuleTester::Basic);
}

void WebViewModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(WebViewModuleTester::Basic);
}

void WebViewModuleTester::End()
{
  //{ Msg_WebView_MouseEvent msg; msg.UnHook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_WebView_MouseEvent, 0); }
}

#endif // #if defined(AP_TEST)
