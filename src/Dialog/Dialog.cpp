// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "Dialog.h"
#include "MsgWebView.h"

Dialog::~Dialog()
{
}

void Dialog::Create(int nLeft, int nTop, int nWidth, int nHeight, int bVisible, const String& sCaption, const String& sIconUrl, const String& sContentUrl)
{
  //ApHandle hView = Apollo::newHandle();
  ApHandle hView = apHandle(); // Use dialog handle as view handle

  sCaption_ = sCaption;
  sIconUrl_ = sIconUrl;
  sContentUrl_ = sContentUrl;

  if (!Msg_WebView_Create::_(hView, nLeft, nTop, nWidth, nHeight)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Create failed"); }

  try {
    //if (!Msg_WebView_Position::_(hView, nLeft, nTop, nWidth, nHeight)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Position failed"); }
    if (!Msg_WebView_Visibility::_(hView, bVisible)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Visibility failed"); }
    if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { throw ApException(LOG_CONTEXT, "Msg_WebView_SetScriptAccessPolicy failed"); }

    sUrl_ = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "theme" + String::filenamePathSeparator() + Apollo::getModuleConfig(MODULE_NAME, "Theme", "WhiteWin") + "/Dialog.html";
    if (!Msg_WebView_Load::_(hView, sUrl_)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Load failed"); }

    //if (!Msg_WebView_SetNavigationPolicy::Deny(hView)) { throw ApException(LOG_CONTEXT, "Msg_WebView_SetNavigationPolicy failed"); }

  } catch (ApException& ex) {
    Msg_WebView_Destroy::_(hView);
    throw ex;
  }

  hView_ = hView;
}

void Dialog::Destroy()
{
  if (!Msg_WebView_Destroy::_(hView_)) { throw ApException(LOG_CONTEXT, "Msg_WebView_Destroy failed"); }
  hView_ = ApNoHandle;
}

void Dialog::SetCaption(const String& sCaption)
{
  sCaption_ = sCaption;

  if (bDocumentLoaded_) {
    Msg_WebView_CallScriptFunction msg;
    msg.hView = hView_;
    msg.sFunction = "ApSetCaption";
    msg.lArgs.AddLast(sCaption_);
    if (!msg.Request()) { apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s(%s) failed: %s", _sz(msg.Type()), _sz(msg.sFunction), _sz(msg.sComment))); }
  }
}

void Dialog::SetIcon(const String& sIconUrl)
{
  sIconUrl_ = sIconUrl;

  if (bDocumentLoaded_){
    Msg_WebView_CallScriptFunction msg;
    msg.hView = hView_;
    msg.sFunction = "ApSetIcon";
    msg.lArgs.AddLast(sIconUrl_);
    if (!msg.Request()) { apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s(%s) failed: %s", _sz(msg.Type()), _sz(msg.sFunction), _sz(msg.sComment))); }
  }
}

void Dialog::GetContentRect(int& nLeft, int& nTop ,int& nWidth, int& nHeight)
{
  nLeft = 12;
  nTop = 32;

  if (bDocumentLoaded_){
    Msg_WebView_CallScriptFunction msg;
    msg.hView = hView_;
    msg.sFunction = "ApGetContentRect";
    if (!msg.Request()) { apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s(%s) failed: %s", _sz(msg.Type()), _sz(msg.sFunction), _sz(msg.sComment))); }
    Apollo::SrpcMessage srpc;
    srpc.fromString(msg.sResult);
    nLeft = srpc.getInt("nLeft");
    nTop = srpc.getInt("nTop");
    nWidth = srpc.getInt("nWidth");
    nHeight = srpc.getInt("nHeight");
  }
}

String Dialog::CallScriptFunction(const String& sFunction, List& lArgs)
{
  // document.getElementById('iApContent').contentWindow.eval("SetText('zz')")

  Msg_WebView_CallScriptFunction msg;
  msg.hView = GetView();
  msg.sFrame = "iApContent";
  msg.sFunction = sFunction;
  for (Elem* e = 0; (e = lArgs.Next(e)) != 0; ) {
    msg.lArgs.AddLast(e->getName());
  }

  //msg.sFunction = "ApContentEval";
  //int nCnt = 0;
  //String sArglist = sFunction + "(";
  //for (Elem* e = 0; (e = lArgs.Next(e)) != 0; ) {
  //  if (nCnt > 0) { sArglist += ", "; }
  //  nCnt++;
  //  String sArg = e->getName();
  //  sArg.escape(String::EscapeQuotes);
  //  sArg.escape(String::EscapeCRLF);
  //  sArglist += "'" + sArg + "'" ;
  //}
  //sArglist += ")";
  //msg.lArgs.AddLast(sArglist);

  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }

  return msg.sResult;
}

void Dialog::ContentCall(const String& sFunction, Apollo::SrpcMessage& srpc, Apollo::SrpcMessage& response)
{
  Msg_Dialog_CallScriptFunction msg;
  msg.hDialog = apHandle();
  msg.sFunction = sFunction;
  msg.lArgs.AddLast(srpc.toString());
  if (!msg.Request()) { throw ApException(LOG_CONTEXT, "Msg_WebView_CallScriptFunction: %s", _sz(msg.sComment)); }

  response.fromString(msg.sResult);
  if (response.length() > 0) {
    int nStatus = response.getInt(Srpc::Key::Status);
    if (nStatus != 1) {
      throw ApException(LOG_CONTEXT, "Status=%d Message=%s", nStatus, _sz(response.getString(Srpc::Key::Message)));
    }
  }

  //Msg_Dialog_CallScriptFunction msg;
  //msg.hDialog = hAp_;
  //msg.sFunction = sFunction;
  //msg.lArgs;

  //if (!msg.Request()) { throw ApException(LOG_CONTEXT, "%s failed: %s", _sz(msg.Type()), _sz(msg.sComment)); }
  ////sResult;
}

void Dialog::OnDocumentLoaded()
{
  bDocumentLoaded_ = 1;

  {
    Msg_WebView_CallScriptFunction msg;
    msg.hView = hView_;
    msg.sFunction = "ApSetContent";
    msg.lArgs.AddLast(sContentUrl_);
    if (!msg.Request()) { apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "%s(%s) failed: %s", _sz(msg.Type()), _sz(msg.sFunction), _sz(msg.sComment))); }
  }

  if (sCaption_) { SetCaption(sCaption_); }
  if (sIconUrl_) { SetIcon(sIconUrl_); }
}

void Dialog::OnContentLoaded(const String& sUrl)
{
  // Poor man's canonicalize
  String sUrl1 = String::toLower(Apollo::canonicalizeUrl(sUrl));
  String sUrl2 = String::toLower(Apollo::canonicalizeUrl(sContentUrl_));

  if (sUrl1 == sUrl2) {
    Msg_Dialog_OnOpened msg;
    msg.hDialog = hAp_;
    msg.Send();
  }
}

void Dialog::OnLoadError(const String& sUrl, const String& sError)
{
  //String sHtml = "<body style='background-color:#FFFFFF;'>Error<body>";
  //if (!Msg_WebView_LoadHtml::_(hView_, sHtml, Apollo::getModuleResourcePath(MODULE_NAME))) {
  //  apLog_Error((LOG_CHANNEL, LOG_CONTEXT, "Loading error message failed"));
  //}

  if (sUrl.startsWith("file:") && sUrl_.startsWith("file:")) {
    String sCanonicalizedErrorUrl = Apollo::canonicalizePath(sUrl);
    String sCanonicalizedDialogUrl = Apollo::canonicalizePath(sUrl_);

    if (sCanonicalizedErrorUrl == sCanonicalizedDialogUrl) {
      // If dialog HTML failed, then tell and close
      // Ignore failing components like images

      apLog_Alert((LOG_CHANNEL, LOG_CONTEXT, "%s %s", _sz(sError), _sz(sUrl)));

      ApAsyncMessage<Msg_Dialog_Destroy> msg;
      msg->hDialog = hView_;
      msg.Post();
    }
  }
}

void Dialog::OnReceivedFocus()
{
}

void Dialog::OnLostFocus()
{
}

void Dialog::OnUnload()
{
  Msg_Dialog_OnClosed msg;
  msg.hDialog = hAp_;
  msg.Send();
}

