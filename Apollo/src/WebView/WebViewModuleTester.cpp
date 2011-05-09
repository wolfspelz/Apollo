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
#include "View.h"

#if defined(AP_TEST)

static ApHandle g_LoadHtml_hView;

void WebViewModuleTester::On_LoadHtml_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg)
{
  if (pMsg->hView != g_LoadHtml_hView) { return; }

  //Msg_WebView_Destroy::_(pMsg->hView);  
  { Msg_WebView_Event_DocumentComplete msg; msg.UnHook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_LoadHtml_WebView_Event_DocumentComplete, 0); }
}

String WebViewModuleTester::LoadHtml()
{
  String s;

  ApHandle hView = Apollo::newHandle();
  g_LoadHtml_hView = hView;

  { Msg_WebView_Event_DocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_LoadHtml_WebView_Event_DocumentComplete, 0, ApCallbackPosNormal); }

  //if (!s) { if (!Msg_WebView_Create::_(hView, "<p style=\"background-color: #00FF00\">Testing " "\xe9\x87\x91" "</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul><iframe src=\"http://www.wolfspelz.de\" style=\"width:100%;height:300px\" />")) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_Create::_(hView)) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_SetScriptAccess::Allow(hView)) { s = "Msg_WebView_SetScriptAccess failed"; }}

  //if (!s) { if (!Msg_WebView_Load::_(hView, "http://www.wolfspelz.de")) { s = "Msg_WebView_Load failed"; }}

  if (!s) { if (!Msg_WebView_LoadHtml::_(hView, 
    "<html>\n"
    "<head>\n"
    "<style>\n"
    "* { margin:0; padding:0; }\n"
    "</style>\n"
    "<script>\n"
    "  function StartTimer() { window.setInterval('GetTime()', 1000); }\n"
    "  function GetTime() { var sResponse = apollo.sendMessage('Method=System_GetTime\\nhView=' + apollo.viewHandle + '\\n'); document.getElementById('iLog').innerHTML = sResponse; }\n"
    "  function Concat(a, b) { return a + b; }\n"
    "</script>\n"
    "</head>\n"
    "<body style='overflow:hidden;' onload=\"StartTimer()\">\n"
    "<div style='width:100%; height:100%; border:1px solid #000000; background-color:rgba(240,240,255,0.7);'>\n"
    "  <p style='width:100%; height:20px; border-bottom:1px solid #000000; background-color:#00ff00;'>Testing LoadHtml " "\xe9\x87\x91" " <a href='http://blog.wolfspelz.de'>Link</a></p>\n"
    "  <p id=\"iLog\">--</p>\n"
    "  <img src='http://webkit.org/images/icon-gold.png' />\n"
    "  <img src='test1.png' />\n"
  //"  <iframe src='http://www.wolfspelz.de'></iframe>\n"
    "</div>\n"
    "</body>\n"
    "</html>\n"
    , "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "html/test/LoadHtml"
    )) { s = "Msg_WebView_LoadHtml failed"; }}

  if (!s) { if (!Msg_WebView_Position::_(hView, 100, 200, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  if (0) {
    if (!s) { if (!Msg_WebView_Destroy::_(hView)) { s = "Msg_WebView_Destroy failed"; }}
  }

  return s;
}

//----------------------------------------------------------

static ApHandle g_CallJSEcho_hView;

void WebViewModuleTester::On_CallJSEcho_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg)
{
  if (pMsg->hView != g_CallJSEcho_hView) { return; }

  int ok = 0;
  String sIn = "Hello World" " \xe9\x87\x91";
  String sOutExpected = "Hello World" " \xe9\x87\x91" " (JS.TestEcho)" " (JS_Apollo_echoString)";
  Msg_WebView_CallScriptFunction msg;
  msg.hView = pMsg->hView;
  msg.sMethod = "TestEcho";
  msg.lArgs.AddLast(sIn);
  if (msg.Request()) {
    if (msg.sResult == sOutExpected) {
      ok = 1;
    }
  }
  String s; if (!ok) { s = "received=" + msg.sResult + " expected=" + sOutExpected; }
  AP_UNITTEST_RESULT(WebViewModuleTester::CallJSEcho_Result, ok, s);

  Msg_WebView_Destroy::_(pMsg->hView);  
  { Msg_WebView_Event_DocumentLoaded msg; msg.UnHook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_CallJSEcho_WebView_Event_DocumentLoaded, 0); }
}

String WebViewModuleTester::CallJSEcho()
{
  String s;

  ApHandle hView = Apollo::newHandle();
  g_CallJSEcho_hView = hView;

  { Msg_WebView_Event_DocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_CallJSEcho_WebView_Event_DocumentLoaded, 0, ApCallbackPosNormal); }

  if (!s) { if (!Msg_WebView_Create::_(hView)) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_SetScriptAccess::Allow(hView)) { s = "Msg_WebView_SetScriptAccess failed"; }}
  if (!s) { if (!Msg_WebView_LoadHtml::_(hView, "<script>function TestEcho(s) { return apollo.echoString(s + ' (JS.TestEcho)'); }</script>" , "")) { s = "Msg_WebView_LoadHtml failed"; }}
  if (!s) { if (!Msg_WebView_Position::_(hView, 100, 500, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  return s;
}

//----------------------------------------------------------

String WebViewModuleTester::Dev()
{
  String s;

  ApHandle hView = Apollo::newHandle();

  { Msg_WebView_Event_DocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_LoadHtml_WebView_Event_DocumentComplete, 0, ApCallbackPosNormal); }

  if (!s) { if (!Msg_WebView_Create::_(hView)) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_SetScriptAccess::Allow(hView)) { s = "Msg_WebView_SetScriptAccess failed"; }}
  //if (!s) { if (!Msg_WebView_Load::_(hView, "http://www.wolfspelz.de")) { s = "Msg_WebView_Load failed"; }}

  String sFile = Apollo::getModuleResourcePath(MODULE_NAME) + "html/test/dev.html";
  String sHtml;
  Apollo::loadFile(sFile, sHtml);
  if (!s) { if (!Msg_WebView_LoadHtml::_(hView, sHtml, "file://" + sFile)) { s = "Msg_WebView_LoadHtml failed"; }}

  if (!s) { if (!Msg_WebView_Position::_(hView, 500, 200, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  if (0) {
    if (!s) { if (!Msg_WebView_Destroy::_(hView)) { s = "Msg_WebView_Destroy failed"; }}
  }

  return s;
}

//----------------------------------------------------------

void WebViewModuleTester::Begin()
{
  AP_UNITTEST_REGISTER(WebViewModuleTester::LoadHtml);
  AP_UNITTEST_REGISTER(WebViewModuleTester::CallJSEcho);
  AP_UNITTEST_REGISTER(WebViewModuleTester::CallJSEcho_Result);
}

void WebViewModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(WebViewModuleTester::LoadHtml);
  AP_UNITTEST_EXECUTE(WebViewModuleTester::CallJSEcho);
  (void) WebViewModuleTester::Dev();
}

void WebViewModuleTester::End()
{
  //{ Msg_WebView_MouseEvent msg; msg.UnHook(MODULE_NAME, (ApCallback) WebViewModuleTester::On_WebView_MouseEvent, 0); }

  //if (ApIsHandle(hView_)) {
  //  Msg_WebView_Destroy::_(hView_);
  //  hView_ = ApNoHandle;
  //}
}

#endif // #if defined(AP_TEST)
