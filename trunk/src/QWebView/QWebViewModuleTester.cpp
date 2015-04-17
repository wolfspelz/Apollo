// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApTypes.h"
#include "Local.h"
#include "View.h"
#include "QWebViewModuleTester.h"

#if defined(AP_TEST)

String QWebViewModuleTester::Dev()
{
  String s;

  ApHandle hView = Apollo::newHandle();

  if (!s) { if (!Msg_WebView_Create::_(hView, 500, 200, 400, 500)) { s = "Msg_WebView_Create failed"; }}
  //if (!s) { if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { s = "Msg_WebView_SetScriptAccessPolicy failed"; }}

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

void QWebViewModuleTester::Begin()
{
}

void QWebViewModuleTester::Execute()
{
  (void) QWebViewModuleTester::Dev();
}

void QWebViewModuleTester::End()
{
}

#endif // #if defined(AP_TEST)
