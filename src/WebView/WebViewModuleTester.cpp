// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApTypes.h"
#include "Local.h"
#include "View.h"
#include "WebViewModuleTester.h"

#if defined(AP_TEST)

static ApHandle g_LoadHtml_hView;

void WebViewModuleTester_LoadHtml_On_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg)
{
  if (pMsg->hView != g_LoadHtml_hView) { return; }

  Msg_WebView_CallScriptFunction msg;
  msg.hView = pMsg->hView;
  msg.sFrame = "iFrame";
  msg.sFunction = "Concat";
  msg.lArgs.AddLast("a");
  msg.lArgs.AddLast("b");
  msg.Request();
  String sResult = msg.sResult;

  //Msg_WebView_Destroy::_(pMsg->hView);
  { Msg_WebView_Event_DocumentComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_LoadHtml_On_WebView_Event_DocumentComplete, 0); }
}

String WebViewModuleTester::LoadHtml()
{
  String s;

  ApHandle hView = Apollo::newHandle();
  g_LoadHtml_hView = hView;

  { Msg_WebView_Event_DocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_LoadHtml_On_WebView_Event_DocumentComplete, 0, ApCallbackPosNormal); }

  //if (!s) { if (!Msg_WebView_Create::_(hView, "<p style=\"background-color: #00FF00\">Testing " "\xe9\x87\x91" "</p><img src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul><iframe src=\"http://www.wolfspelz.de\" style=\"width:100%;height:300px\" />")) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_Create::_(hView, 100, 200, 300, 500)) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}

  //if (!s) { if (!Msg_WebView_Load::_(hView, "http://www.wolfspelz.de")) { s = "Msg_WebView_Load failed"; }}

  if (!s) { if (!Msg_WebView_LoadHtml::_(hView, 
    "<html>\n"
    "<head>\n"
    "<script type=\"text/javascript\" src=\"LoadHtml.js\"></script>"
    "<style>\n"
    "* { margin:0; padding:0; }\n"
    "</style>\n"
    "<script>\n"
    "  function StartTimer() { window.setInterval('GetTime()', 1000); }\n"
    "  function GetTime() { var sResponse = apollo.sendMessage('Method=System_GetTime\\nhView=' + apollo.viewHandle + '\\n'); document.getElementById('iLog').innerHTML = sResponse + ' ' + GetLinkedJavascriptText(); }\n"
    "  function Concat(a, b) { return a + b; }\n"
    "</script>\n"
    "</head>\n"
    "<body style='overflow:hidden;' onload=\"StartTimer()\">\n"
    "<div style='width:100%; height:100%; border:1px solid #000000; background-color:rgba(240,240,255,0.7);'>\n"
    "  <p style='width:100%; height:20px; border-bottom:1px solid #000000; background-color:#00ff00;'>Testing LoadHtml " "\xe9\x87\x91" " <a href='http://blog.wolfspelz.de'>http://blog.wolfspelz.de</a></p> <a href='https://www.xing.com'>https://www.xing.com</a></p>\n"
    "  <p id=\"iLog\">--</p>\n"
    "  <img src='http://webkit.org/images/icon-gold.png' />\n"
    "  <img src='test1.png' />\n"
    "  <input type=\"button\" value=\"Quit\" onclick=\"apollo.sendMessage('Method=MainLoop_EndLoop\\nhView=' + apollo.viewHandle + '\\n');\"/>\n"
    "  <div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div>"
  "    <iframe id=\"iFrame\" src=\"dev-iframe.html\" frameborder=\"0\" style=\"width:250px; height:100px;\"></iframe>\n"
  //"    <iframe src='http://www.wolfspelz.de'></iframe>\n"
    "</div>\n"
    "</body>\n"
    "</html>\n"
    , "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/LoadHtml"
    )) { s = "Msg_WebView_LoadHtml failed"; }}

  //if (!s) { if (!Msg_WebView_SetNavigationPolicy::Deny(hView)) { s = "Msg_WebView_SetNavigationPolicy failed"; }}
  //if (!s) { if (!Msg_WebView_Position::_(hView, 100, 200, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  if (0) {
    if (!s) { if (!Msg_WebView_Destroy::_(hView)) { s = "Msg_WebView_Destroy failed"; }}
  }

  return s;
}

//----------------------------------------------------------
// Load from non ASCII path

static ApHandle g_LoadFromNonAsciiPath_hView;

void WebViewModuleTester_LoadFromNonAsciiPath_On_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg)
{
  if (pMsg->hView != g_LoadFromNonAsciiPath_hView) { return; }

  Msg_WebView_Destroy::_(pMsg->hView);
  { Msg_WebView_Event_DocumentComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_LoadFromNonAsciiPath_On_WebView_Event_DocumentComplete, 0); }
}

String WebViewModuleTester::LoadFromNonAsciiPath()
{
  String s;

  ApHandle hView = Apollo::newHandle();
  g_LoadFromNonAsciiPath_hView = hView;

  String sPath = "file://" + Apollo::getModuleResourcePath(MODULE_NAME);
  //sPath += L"test/aホü/NonAsciiPath.html";
  sPath += "test/" "a" "\xef\xbe\x8e" "\xc3\xbc" "/NonAsciiPath.html";

  { Msg_WebView_Event_DocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_LoadFromNonAsciiPath_On_WebView_Event_DocumentComplete, 0, ApCallbackPosNormal); }

  if (!s) { if (!Msg_WebView_Create::_(hView, 100, 500, 400, 300)) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}
  if (!s) { if (!Msg_WebView_Load::_(hView, sPath)) { s = "Msg_WebView_Load failed"; }}
  //if (!s) { if (!Msg_WebView_Position::_(hView, 100, 500, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  return s;
}

//----------------------------------------------------------
// Check a test echo function in the shared apollo JS object

static ApHandle g_CallJSEcho_hView;

void WebViewModuleTester_CallJSEcho_On_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg)
{
  if (pMsg->hView != g_CallJSEcho_hView) { return; }

  int ok = 0;
  String sIn = "Hello World" " \xe9\x87\x91";
  String sOutExpected = "Hello World" " \xe9\x87\x91" " (JS.TestEcho)" " (JS_Apollo_echoString)";
  Msg_WebView_CallScriptFunction msg;
  msg.hView = pMsg->hView;
  msg.sFunction = "TestEcho";
  msg.lArgs.AddLast(sIn);
  if (msg.Request()) {
    if (msg.sResult == sOutExpected) {
      ok = 1;
    }
  }
  String s; if (!ok) { s = "received=" + msg.sResult + " expected=" + sOutExpected; }
  AP_UNITTEST_RESULT(WebViewModuleTester::CallJSEcho_Result, ok, s);

  Msg_WebView_Destroy::_(pMsg->hView);  
  { Msg_WebView_Event_DocumentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_CallJSEcho_On_WebView_Event_DocumentLoaded, 0); }
}

String WebViewModuleTester::CallJSEcho()
{
  String s;

  ApHandle hView = Apollo::newHandle();
  g_CallJSEcho_hView = hView;

  { Msg_WebView_Event_DocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_CallJSEcho_On_WebView_Event_DocumentLoaded, 0, ApCallbackPosNormal); }

  if (!s) { if (!Msg_WebView_Create::_(hView, 100, 500, 400, 300)) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}
  if (!s) { if (!Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/CallJSEcho.html")) { s = "Msg_WebView_Load failed"; }}
  //if (!s) { if (!Msg_WebView_Load::_(hView, L"file://C:\\temp\\Jürgen\\Apollo\\bin\\Win32\\Debug\\modules\\WebView\\test\\CallJSEcho.html")) { s = "Msg_WebView_Load failed"; }}
  //if (!s) { if (!Msg_WebView_Position::_(hView, 100, 500, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  return s;
}

//----------------------------------------------------------
// Check System_Echo via SrpcGate

static ApHandle g_CallSystemEcho_hView;

void WebViewModuleTester_CallSystemEcho_On_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg)
{
  if (pMsg->hView != g_CallSystemEcho_hView) { return; }

  int ok = 0;
  String sIn = "43";
  String sOutExpected = "Status=1\nnOut=43\n";
  Msg_WebView_CallScriptFunction msg;
  msg.hView = pMsg->hView;
  msg.sFunction = "TestEcho";
  msg.lArgs.AddLast(sIn);
  if (msg.Request()) {
    if (msg.sResult == sOutExpected) {
      ok = 1;
    }
  }
  String s; if (!ok) { s = "received=" + msg.sResult + " expected=" + sOutExpected; }
  AP_UNITTEST_RESULT(WebViewModuleTester::CallSystemEcho_Result, ok, s);

  Msg_WebView_Destroy::_(pMsg->hView);  
  { Msg_WebView_Event_DocumentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_CallSystemEcho_On_WebView_Event_DocumentLoaded, 0); }
}

String WebViewModuleTester::CallSystemEcho()
{
  String s;

  ApHandle hView = Apollo::newHandle();
  g_CallSystemEcho_hView = hView;

  { Msg_WebView_Event_DocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_CallSystemEcho_On_WebView_Event_DocumentLoaded, 0, ApCallbackPosNormal); }

  if (!s) { if (!Msg_WebView_Create::_(hView, 100, 500, 400, 300)) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}
  if (!s) { if (!Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/CallSystemEcho.html")) { s = "Msg_WebView_LoadHtml failed"; }}
  //if (!s) { if (!Msg_WebView_Position::_(hView, 100, 500, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  return s;
}

//----------------------------------------------------------
// Call custom echo function using ApType to go directly to the module not via SrpcGate

static ApHandle g_CallCustomEcho_hView;

void WebViewModuleTester_CallCustomEcho_On_WebView_ModuleCall(Msg_WebView_ModuleCall* pMsg)
{
  if (pMsg->hView != g_CallCustomEcho_hView) { return; }

  String sType = pMsg->Type();
  String sMethod = pMsg->srpc.getString(Srpc::Key::Method);
  String sIn = pMsg->srpc.getString("sIn");
  String sOut = sIn;
  if (sType != "WebView_ModuleCall") { sOut.appendf(" (Error: Expected Type=%s got=%s)", _sz("WebView_ModuleCall"), _sz(sType)); }
  if (sMethod != "CustomEchoMethod") { sOut.appendf(" (Error: Expected Method=%s got=%s)", _sz("CustomEchoMethod"), _sz(sMethod)); }
  pMsg->response.set("sOut", sOut);
  pMsg->apStatus = ApMessage::Ok;
}

void WebViewModuleTester_CallCustomEcho_On_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg)
{
  if (pMsg->hView != g_CallCustomEcho_hView) { return; }

  int ok = 0;
  String sIn = "43";
  String sOutExpected = "sOut=41\nStatus=1\n"; // Should check params, disregarding order, not just the string
  Msg_WebView_CallScriptFunction msg;
  msg.hView = pMsg->hView;
  msg.sFunction = "TestEcho";
  msg.lArgs.AddLast(sIn);
  if (msg.Request()) {
    if (msg.sResult == sOutExpected) {
      ok = 1;
    }
  }
  String s; if (!ok) { s = "received=" + msg.sResult + " expected=" + sOutExpected; }
  AP_UNITTEST_RESULT(WebViewModuleTester::CallCustomEcho_Result, ok, s);

  Msg_WebView_Destroy::_(pMsg->hView);
  { Msg_WebView_Event_DocumentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_CallCustomEcho_On_WebView_Event_DocumentLoaded, 0); }
  { Msg_WebView_ModuleCall msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_CallCustomEcho_On_WebView_ModuleCall, 0); }
}

String WebViewModuleTester::CallCustomEcho()
{
  String s;

  ApHandle hView = Apollo::newHandle();
  g_CallCustomEcho_hView = hView;

  { Msg_WebView_Event_DocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_CallCustomEcho_On_WebView_Event_DocumentLoaded, 0, ApCallbackPosNormal); }
  { Msg_WebView_ModuleCall msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_CallCustomEcho_On_WebView_ModuleCall, 0, ApCallbackPosNormal); }

  if (!s) { if (!Msg_WebView_Create::_(hView, 100, 500, 400, 300)) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}
  if (!s) { if (!Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/CallCustomEcho.html")) { s = "Msg_WebView_LoadHtml failed"; }}
  //if (!s) { if (!Msg_WebView_Position::_(hView, 100, 500, 400, 300)) { s = "Msg_WebView_Position failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  return s;
}

//----------------------------------------------------------
// Call custom echo for multiple nested frames

static ApHandle g_FrameIO_hView;

void WebViewModuleTester_FrameIO_On_WebView_ModuleCall(Msg_WebView_ModuleCall* pMsg)
{
  if (pMsg->hView != g_FrameIO_hView) { return; }

  String sType = pMsg->Type();
  String sMethod = pMsg->srpc.getString(Srpc::Key::Method);
  String sIn = pMsg->srpc.getString("sIn");
  String sOut = sIn;
  if (sType != "WebView_ModuleCall") { sOut.appendf(" (Error: Expected Type=%s got=%s)", _sz("WebView_ModuleCall"), _sz(sType)); }
  if (sMethod != "CustomEchoMethod") { sOut.appendf(" (Error: Expected Method=%s got=%s)", _sz("CustomEchoMethod"), _sz(sMethod)); }
  pMsg->response.set("sOut", sOut);
  pMsg->apStatus = ApMessage::Ok;
}

void WebViewModuleTester_FrameIO_On_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg)
{
  if (pMsg->hView != g_FrameIO_hView) { return; }

  {
    int ok = 0;
    Msg_WebView_CallScriptFunction msg;
    msg.hView = pMsg->hView;
    msg.sFrame = "/";
    msg.sFunction = "TestEcho0";
    msg.lArgs.AddLast("40");
    String sOutExpected = "sOut=Frame0-40\nStatus=1\n"; // Should check params, disregarding order, not just the string
    if (msg.Request()) {
      if (msg.sResult == sOutExpected) {
        ok = 1;
      }
    }
    String s; if (!ok) { s = "received=" + msg.sResult + " expected=" + sOutExpected; }
    AP_UNITTEST_RESULT(WebViewModuleTester::FrameIO_Result0, ok, s);
  }

  {
    int ok = 0;
    Msg_WebView_CallScriptFunction msg;
    msg.hView = pMsg->hView;
    msg.sFrame = "/iFrame1";
    msg.sFunction = "TestEcho1";
    msg.lArgs.AddLast("41");
    String sOutExpected = "sOut=Frame1-41\nStatus=1\n"; // Should check params, disregarding order, not just the string
    if (msg.Request()) {
      if (msg.sResult == sOutExpected) {
        ok = 1;
      }
    }
    String s; if (!ok) { s = "received=" + msg.sResult + " expected=" + sOutExpected; }
    AP_UNITTEST_RESULT(WebViewModuleTester::FrameIO_Result1, ok, s);
  }

  {
    int ok = 0;
    Msg_WebView_CallScriptFunction msg;
    msg.hView = pMsg->hView;
    msg.sFrame = "/iFrame1/iFrame2";
    msg.sFunction = "TestEcho2";
    msg.lArgs.AddLast("42");
    String sOutExpected = "sOut=Frame2-42\nStatus=1\n"; // Should check params, disregarding order, not just the string
    if (msg.Request()) {
      if (msg.sResult == sOutExpected) {
        ok = 1;
      }
    }
    String s; if (!ok) { s = "received=" + msg.sResult + " expected=" + sOutExpected; }
    AP_UNITTEST_RESULT(WebViewModuleTester::FrameIO_Result2, ok, s);
  }

  //{
  //  Msg_WebView_CallScriptFunction msg;
  //  msg.hView = pMsg->hView;
  //  msg.sFrame = "/iFrame1/iFrame2";
  //  msg.sFunction = "TestEcho2";
  //  msg.lArgs.AddLast("42");
  //  msg.Request();
  //}
  //{
  //  Msg_WebView_CallScriptFunction msg;
  //  msg.hView = pMsg->hView;
  //  msg.sFrame = "/iFrame1/iFrame2";
  //  msg.sFunction = "TestEcho2";
  //  msg.lArgs.AddLast("42");
  //  msg.Request();
  //}
  //{
  //  Msg_WebView_CallScriptFunction msg;
  //  msg.hView = pMsg->hView;
  //  msg.sFrame = "/iFrame1/iFrame2";
  //  msg.sFunction = "TestEcho2";
  //  msg.lArgs.AddLast("42");
  //  msg.Request();
  //}
  //{
  //  Msg_WebView_CallScriptFunction msg;
  //  msg.hView = pMsg->hView;
  //  msg.sFrame = "/iFrame1/iFrame2";
  //  msg.sFunction = "TestEcho2";
  //  msg.lArgs.AddLast("42");
  //  msg.Request();
  //}
  //{
  //  Msg_WebView_CallScriptFunction msg;
  //  msg.hView = pMsg->hView;
  //  msg.sFrame = "/iFrame1/iFrame2";
  //  msg.sFunction = "TestEcho2";
  //  msg.lArgs.AddLast("42");
  //  msg.Request();
  //}

  Msg_WebView_Destroy::_(pMsg->hView);
  { Msg_WebView_Event_DocumentComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_FrameIO_On_WebView_Event_DocumentComplete, 0); }
  { Msg_WebView_ModuleCall msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_FrameIO_On_WebView_ModuleCall, 0); }
}

String WebViewModuleTester::FrameIO()
{
  String s;

  ApHandle hView = Apollo::newHandle();
  g_FrameIO_hView = hView;

  { Msg_WebView_Event_DocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_FrameIO_On_WebView_Event_DocumentComplete, 0, ApCallbackPosNormal); }
  { Msg_WebView_ModuleCall msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_FrameIO_On_WebView_ModuleCall, 0, ApCallbackPosNormal); }

  if (!s) { if (!Msg_WebView_Create::_(hView, 100, 500, 400, 300)) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}
  if (!s) { if (!Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/FrameIO.html")) { s = "Msg_WebView_LoadHtml failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  return s;
}

//----------------------------------------------------------

static ApHandle g_StartManuallySerialized_hView1;
static ApHandle g_StartManuallySerialized_hView2;
static int g_StartManuallySerialized_bGot1 = 0;
static int g_StartManuallySerialized_bGot2 = 0;

void WebViewModuleTester_StartManuallySerialized_On_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg)
{
  if (pMsg->hView == g_StartManuallySerialized_hView1) {
    Msg_WebView_Load::_(g_StartManuallySerialized_hView2, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/StartManuallySerialized2.html");
  }
}

void WebViewModuleTester_StartManuallySerialized_On_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg)
{
  if (pMsg->hView == g_StartManuallySerialized_hView1) {
    g_StartManuallySerialized_bGot1 = 1;
    Msg_WebView_CallScriptFunction msg;
    msg.hView = pMsg->hView;
    msg.sFunction = "Test1";
    int ok = msg.Request();
    AP_UNITTEST_RESULT(WebViewModuleTester::StartManuallySerialized_Result1, ok, "1");
  }

  if (pMsg->hView == g_StartManuallySerialized_hView2) {
    g_StartManuallySerialized_bGot2 = 1;
    Msg_WebView_CallScriptFunction msg;
    msg.hView = pMsg->hView;
    msg.sFunction = "Test2";
    int ok = msg.Request();
    AP_UNITTEST_RESULT(WebViewModuleTester::StartManuallySerialized_Result2, ok, "2");
  }

  if (g_StartManuallySerialized_bGot1 && g_StartManuallySerialized_bGot2) {
    Msg_WebView_Destroy::_(g_StartManuallySerialized_hView1);
    Msg_WebView_Destroy::_(g_StartManuallySerialized_hView2);
    { Msg_WebView_Event_DocumentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_StartManuallySerialized_On_WebView_Event_DocumentLoaded, 0); }
    { Msg_WebView_Event_DocumentComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_StartManuallySerialized_On_WebView_Event_DocumentComplete, 0); }
  }
}

String WebViewModuleTester::StartManuallySerialized()
{
  String s;

  { Msg_WebView_Event_DocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_StartManuallySerialized_On_WebView_Event_DocumentLoaded, 0, ApCallbackPosNormal); }
  { Msg_WebView_Event_DocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_StartManuallySerialized_On_WebView_Event_DocumentComplete, 0, ApCallbackPosNormal); }

  {
    ApHandle hView = Apollo::newHandle();
    g_StartManuallySerialized_hView1 = hView;
    if (!s) { if (!Msg_WebView_Create::_(hView, 100, 100, 200, 100)) { s = "Msg_WebView_Create failed"; }}
    if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}
    if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}
    // Load now
    Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/StartManuallySerialized1.html");
  }

  {
    ApHandle hView = Apollo::newHandle();
    g_StartManuallySerialized_hView2 = hView;
    if (!s) { if (!Msg_WebView_Create::_(hView, 300, 100, 200, 100)) { s = "Msg_WebView_Create failed"; }}
    if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}
    if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}
    // Load later
    //Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/StartManuallySerialized2.html");
  }

  return s;
}

//----------------------------------------------------------

static ApHandle g_StartAutomaticallySerialized_hView1;
static ApHandle g_StartAutomaticallySerialized_hView2;
static int g_StartAutomaticallySerialized_bGot1 = 0;
static int g_StartAutomaticallySerialized_bGot2 = 0;

void WebViewModuleTester_StartAutomaticallySerialized_On_WebView_Event_DocumentComplete(Msg_WebView_Event_DocumentComplete* pMsg)
{
  if (pMsg->hView == g_StartAutomaticallySerialized_hView1) {
    g_StartAutomaticallySerialized_bGot1 = 1;
    Msg_WebView_CallScriptFunction msg;
    msg.hView = pMsg->hView;
    msg.sFunction = "Test1";
    int ok = msg.Request();
    AP_UNITTEST_RESULT(WebViewModuleTester::StartAutomaticallySerialized_Result1, ok, "1");
  }

  if (pMsg->hView == g_StartAutomaticallySerialized_hView2) {
    g_StartAutomaticallySerialized_bGot2 = 1;
    Msg_WebView_CallScriptFunction msg;
    msg.hView = pMsg->hView;
    msg.sFunction = "Test2";
    int ok = msg.Request();
    AP_UNITTEST_RESULT(WebViewModuleTester::StartAutomaticallySerialized_Result2, ok, "2");
  }

  if (g_StartAutomaticallySerialized_bGot1 && g_StartAutomaticallySerialized_bGot2) {
    Msg_WebView_Destroy::_(g_StartAutomaticallySerialized_hView1);
    Msg_WebView_Destroy::_(g_StartAutomaticallySerialized_hView2);
    { Msg_WebView_Event_DocumentComplete msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_StartAutomaticallySerialized_On_WebView_Event_DocumentComplete, 0); }
  }
}

String WebViewModuleTester::StartAutomaticallySerialized()
{
  String s;

  { Msg_WebView_Event_DocumentComplete msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_StartAutomaticallySerialized_On_WebView_Event_DocumentComplete, 0, ApCallbackPosNormal); }

  {
    ApHandle hView = Apollo::newHandle();
    g_StartAutomaticallySerialized_hView1 = hView;
    if (!s) { if (!Msg_WebView_Create::_(hView, 100, 200, 200, 100)) { s = "Msg_WebView_Create failed"; }}
    if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}
    if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}
    // Load now
    Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/StartAutomaticallySerialized1.html");
  }

  {
    ApHandle hView = Apollo::newHandle();
    g_StartAutomaticallySerialized_hView2 = hView;
    if (!s) { if (!Msg_WebView_Create::_(hView, 300, 200, 200, 100)) { s = "Msg_WebView_Create failed"; }}
    if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}
    if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}
    // Load now
    Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/StartAutomaticallySerialized2.html");
  }

  return s;
}

//----------------------------------------------------------
// Fetch value from DOM element

static ApHandle g_GetDomElement_hView;

void WebViewModuleTester_GetDomElement_On_WebView_Event_DocumentLoaded(Msg_WebView_Event_DocumentLoaded* pMsg)
{
  if (pMsg->hView != g_GetDomElement_hView) { return; }

  String s;

  if (!s) {
    String sExpected = "Div2Value";
    String sResult = Msg_WebView_GetElementValue::_(pMsg->hView, "", "#iDiv2", "innerHTML");
    if (sResult != sExpected) {
      s.appendf("got=%s expected=%s", _sz(sResult), _sz(sExpected));
    }
  }

  if (!s) {
    String sExpected = "http://webkit.org/images/icon-gold.png";
    String sResult = Msg_WebView_GetElementValue::_(pMsg->hView, "", "#iImg", "src");
    if (sResult != sExpected) {
      s.appendf("got=%s expected=%s", _sz(sResult), _sz(sExpected));
    }
  }

  AP_UNITTEST_RESULT(WebViewModuleTester::GetDomElement_Result, s.empty(), s);

  Msg_WebView_Destroy::_(pMsg->hView);  
  { Msg_WebView_Event_DocumentLoaded msg; msg.Unhook(MODULE_NAME, (ApCallback) WebViewModuleTester_GetDomElement_On_WebView_Event_DocumentLoaded, 0); }
}

String WebViewModuleTester::GetDomElement()
{
  String s;

  ApHandle hView = Apollo::newHandle();
  g_GetDomElement_hView = hView;

  { Msg_WebView_Event_DocumentLoaded msg; msg.Hook(MODULE_NAME, (ApCallback) WebViewModuleTester_GetDomElement_On_WebView_Event_DocumentLoaded, 0, ApCallbackPosNormal); }

  if (!s) { if (!Msg_WebView_Create::_(hView, 100, 500, 400, 300)) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}
  if (!s) { if (!Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/GetDomElement.html")) { s = "Msg_WebView_LoadHtml failed"; }}
  if (!s) { if (!Msg_WebView_Visibility::_(hView, 1)) { s = "Msg_WebView_Visibility failed"; }}

  return s;
}

//----------------------------------------------------------

String WebViewModuleTester::Dev()
{
  String s;

  ApHandle hView = Apollo::newHandle();

  if (!s) { if (!Msg_WebView_Create::_(hView, 500, 200, 400, 500)) { s = "Msg_WebView_Create failed"; }}
  if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}

  //if (!s) { if (!Msg_WebView_Load::_(hView, "http://www.wolfspelz.de")) { s = "Msg_WebView_Load failed"; }}
  if (!s) { if (!Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/dev.html")) { s = "Msg_WebView_LoadHtml failed"; }}

  //if (!s) { if (!Msg_WebView_LoadHtml::_(hView, ""
  //  "<html>\n"
  //  "<head>\n"
  //  "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/>\n"
  //  "<script type=\"text/javascript\" src=\"../apollo.js\"></script>\n"
  //  "</head>\n"
  //  "<body style=\"overflow:hidden;\">\n"
  //  "</body>\n"
  //  "</html>\n"
  //  "", "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "test/dev.html")) { s = "Msg_WebView_LoadHtml failed"; }}

  //if (!s) { if (!Msg_WebView_Position::_(hView, 500, 200, 400, 300)) { s = "Msg_WebView_Position failed"; }}
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
  AP_UNITTEST_REGISTER(WebViewModuleTester::LoadFromNonAsciiPath);
  //AP_UNITTEST_REGISTER(WebViewModuleTester::LoadFromNonAsciiPath_Result);
  AP_UNITTEST_REGISTER(WebViewModuleTester::CallJSEcho);
  AP_UNITTEST_REGISTER(WebViewModuleTester::CallJSEcho_Result);
  AP_UNITTEST_REGISTER(WebViewModuleTester::CallSystemEcho);
  AP_UNITTEST_REGISTER(WebViewModuleTester::CallSystemEcho_Result);
  AP_UNITTEST_REGISTER(WebViewModuleTester::CallCustomEcho);
  AP_UNITTEST_REGISTER(WebViewModuleTester::CallCustomEcho_Result);
  AP_UNITTEST_REGISTER(WebViewModuleTester::FrameIO);
  AP_UNITTEST_REGISTER(WebViewModuleTester::FrameIO_Result0);
  AP_UNITTEST_REGISTER(WebViewModuleTester::FrameIO_Result1);
  AP_UNITTEST_REGISTER(WebViewModuleTester::FrameIO_Result2);
  AP_UNITTEST_REGISTER(WebViewModuleTester::StartManuallySerialized);
  AP_UNITTEST_REGISTER(WebViewModuleTester::StartManuallySerialized_Result1);
  AP_UNITTEST_REGISTER(WebViewModuleTester::StartManuallySerialized_Result2);
  AP_UNITTEST_REGISTER(WebViewModuleTester::StartAutomaticallySerialized);
  AP_UNITTEST_REGISTER(WebViewModuleTester::StartAutomaticallySerialized_Result1);
  AP_UNITTEST_REGISTER(WebViewModuleTester::StartAutomaticallySerialized_Result2);
  AP_UNITTEST_REGISTER(WebViewModuleTester::GetDomElement);
  AP_UNITTEST_REGISTER(WebViewModuleTester::GetDomElement_Result);
}

void WebViewModuleTester::Execute()
{
  AP_UNITTEST_EXECUTE(WebViewModuleTester::LoadHtml);
  AP_UNITTEST_EXECUTE(WebViewModuleTester::LoadFromNonAsciiPath);
  AP_UNITTEST_EXECUTE(WebViewModuleTester::CallJSEcho);
  AP_UNITTEST_EXECUTE(WebViewModuleTester::CallSystemEcho);
  AP_UNITTEST_EXECUTE(WebViewModuleTester::CallCustomEcho);
  AP_UNITTEST_EXECUTE(WebViewModuleTester::FrameIO);
  AP_UNITTEST_EXECUTE(WebViewModuleTester::StartManuallySerialized);
  AP_UNITTEST_EXECUTE(WebViewModuleTester::StartAutomaticallySerialized);
  AP_UNITTEST_EXECUTE(WebViewModuleTester::GetDomElement);
  
  //(void) WebViewModuleTester::Dev();
}

void WebViewModuleTester::End()
{
}

#endif // #if defined(AP_TEST)
