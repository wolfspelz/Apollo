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

static ApHandle g_LoadHtml_hWebView;

void WebViewModuleTester::On_LoadHtml_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg)
{
  if (pMsg->hWebView != g_LoadHtml_hWebView) { return; }

  //Msg_WebView_Destroy::_(pMsg->hWebView);  
  { Msg_WebView_Event_DocumentComplete msg; msg.UnHook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_LoadHtml_WebView_Event_DocumentComplete, 0); }
}

String WebViewModuleTester::LoadHtml()
{
  String s;

  ApHandle hWebView = Apollo::newHandle();
  g_LoadHtml_hWebView = hWebView;

  { Msg_WebView_Event_DocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_LoadHtml_WebView_Event_DocumentComplete, 0, ApCallbackPosNormal); }

  //if (!s) { if (!Msg_WebView_Create::_(hWebView, "<p style=\"background-color: #00FF00\">Testing " "\xe9\x87\x91" "</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul><iframe src=\"http://www.wolfspelz.de\" style=\"width:100%;height:300px\" />")) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_Create::_(hWebView)) { s = "Msg_WebView_Create failed"; }}

  //if (!s) { if (!Msg_WebView_Load::_(hWebView, "http://www.wolfspelz.de")) { s = "Msg_WebView_Load failed"; }}

  if (!s) { if (!Msg_WebView_LoadHtml::_(hWebView, 
    "<html>"
    "<head>"
    "<style>"
    "* { margin:0; padding:0; }"
    "</style>"
    "<script>"
    "function safsdfad(a, b) { return a + b; }"
    "</script>"
    "</head>"
    "<body style='overflow:hidden;'>"
    "<div style='width:100%; height:100%; border:1px solid #000000;'>"
    "<p style='background-color:#00FF00;'>Testing LoadHtml " "\xe9\x87\x91" " <a href='http://blog.wolfspelz.de'>Link</a></p>"
    "<img src='http://webkit.org/images/icon-gold.png' />"
    "<img src='test/test1.png' />"
    //"<iframe src='http://www.wolfspelz.de'></iframe>"
    "</div>"
    "</body>"
    "</html>"
    , "file://" + Apollo::getAppBasePath() + "LoadHtml"
    )) { s = "Msg_WebView_LoadHtml failed"; }}

  if (!s) { if (!Msg_WebView_Position::_(hWebView, 100, 200, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hWebView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  if (0) {
    if (!s) { if (!Msg_WebView_Destroy::_(hWebView)) { s = "Msg_WebView_Destroy failed"; }}
  }

  return s;
}

//----------------------------------------------------------

static ApHandle g_CallJSEcho_hWebView;

void WebViewModuleTester::On_CallJSEcho_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg)
{
  if (pMsg->hWebView != g_CallJSEcho_hWebView) { return; }

  int ok = 0;
  String sIn = "Hello World";
  String sOutExpected = "Hello World" " (JS.test_echo)" " (JS_apollo_echo)";
  Msg_WebView_CallJavaScriptFunction msg;
  msg.hWebView = pMsg->hWebView;
  msg.sMethod = "test_echo";
  msg.lArgs.AddLast(sIn);
  if (msg.Request()) {
    if (msg.sResult == sOutExpected) {
      ok = 1;
    }
  }
  String s; if (!ok) { s = "received=" + msg.sResult + " expected=" + sOutExpected; }
  AP_UNITTEST_RESULT(WebViewModuleTester::CallJSEcho_Result, ok, s);

  Msg_WebView_Destroy::_(pMsg->hWebView);  
  { Msg_WebView_Event_DocumentLoaded msg; msg.UnHook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_CallJSEcho_WebView_Event_DocumentLoaded, 0); }
}

String WebViewModuleTester::CallJSEcho()
{
  String s;

  ApHandle hWebView = Apollo::newHandle();
  g_CallJSEcho_hWebView = hWebView;

  { Msg_WebView_Event_DocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_CallJSEcho_WebView_Event_DocumentLoaded, 0, ApCallbackPosNormal); }

  if (!s) { if (!Msg_WebView_Create::_(hWebView)) { s = "Msg_WebView_Create failed"; }}
  //if (!s) { if (!Msg_WebView_LoadHtml::_(hWebView, "<script>function test_echo(s) { return apollo.echo(s + ' (JS.test_echo)'); }</script>", "file://" + Apollo::getAppBasePath())) { s = "Msg_WebView_LoadHtml failed"; }}

  if (!s) { if (!Msg_WebView_LoadHtml::_(hWebView, 
    "<html>"
    "<head>"
    "<style>"
    "* { margin:0; padding:0; }"
    "</style>"
    "<script>"
    "function test_echo(s) { return apollo.echo(s + ' (JS.test_echo)'); }"
    "</script>"
    "</head>"
    "<body style='overflow:hidden;'>"
    "<div style='width:100%; height:100%; border:1px solid #000000;'>"
    "<p style='background-color:#00FF00;'>Testing CallJSEcho " "\xe9\x87\x91" "</p>"
    "<img src='http://webkit.org/images/icon-gold.png' />"
    "<img src='test/test1.png' />"
    //"<iframe src='http://www.wolfspelz.de'></iframe>"
    "</div>"
    "</body>"
    "</html>"
    , "file://" + Apollo::getAppBasePath() + "CallJSEcho"
    )) { s = "Msg_WebView_LoadHtml failed"; }}

  if (!s) { if (!Msg_WebView_Position::_(hWebView, 100, 500, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hWebView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  return s;
}

//----------------------------------------------------------

String WebViewModuleTester::JSSrpc()
{
  String s;

  ApHandle hWebView = Apollo::newHandle();

  if (!s) { if (!Msg_WebView_Create::_(hWebView)) { s = "Msg_WebView_Create failed"; }}
  //if (!s) { if (!Msg_WebView_LoadHtml::_(hWebView, "<script>function test_echo(s) { return apollo.echo(s + ' (JS.test_echo)'); }</script>", "file://" + Apollo::getAppBasePath())) { s = "Msg_WebView_LoadHtml failed"; }}

  if (!s) { if (!Msg_WebView_LoadHtml::_(hWebView, 
    "<html>"
    "<head>"
    "<style>"
    "* { margin:0; padding:0; }"
    "</style>"
    "<script>"
    "function StartTimer() { window.setInterval('GetTime()', 1000); }"
    "function GetTime() { var sResponse = apollo.send('Method=System_GetTime'); document.getElementById('iResponse').innerHTML = sResponse; }"
    "</script>"
    "</head>"
    "<body style=\"overflow:hidden;\" onload=\"StartTimer()\">"
    "<div style=\"width:100%; height:100%; border:1px solid #000000;\">"
    "<p id=\"iResponse\" style=\"background-color:#00FF00;\"></p>"
    "</div>"
    "</body>"
    "</html>"
    , "file://" + Apollo::getAppBasePath() + "JSSrpc"
    )) { s = "Msg_WebView_LoadHtml failed"; }}

  if (!s) { if (!Msg_WebView_Position::_(hWebView, 100, 200, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hWebView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  return s;
}

//----------------------------------------------------------

void WebViewModuleTester::Begin()
{
  //AP_UNITTEST_REGISTER(WebViewModuleTester::LoadHtml);
  //AP_UNITTEST_REGISTER(WebViewModuleTester::CallJSEcho);
  //AP_UNITTEST_REGISTER(WebViewModuleTester::CallJSEcho_Result);
  AP_UNITTEST_REGISTER(WebViewModuleTester::JSSrpc);
}

void WebViewModuleTester::Execute()
{
  //AP_UNITTEST_EXECUTE(WebViewModuleTester::LoadHtml);
  //AP_UNITTEST_EXECUTE(WebViewModuleTester::CallJSEcho);
  AP_UNITTEST_EXECUTE(WebViewModuleTester::JSSrpc);
}

void WebViewModuleTester::End()
{
  //{ Msg_WebView_MouseEvent msg; msg.UnHook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_WebView_MouseEvent, 0); }

  //if (ApIsHandle(hWebView_)) {
  //  Msg_WebView_Destroy::_(hWebView_);
  //  hWebView_ = ApNoHandle;
  //}
}

#endif // #if defined(AP_TEST)
