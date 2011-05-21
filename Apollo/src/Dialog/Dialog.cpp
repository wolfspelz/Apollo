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

  if (!Msg_WebView_Create::_(hView)) { throw ApException("Msg_WebView_Create failed"); }

  try {
    if (!Msg_WebView_Position::_(hView, nLeft, nTop, nWidth, nHeight)) { throw ApException("Msg_WebView_Position failed"); }
    if (!Msg_WebView_Visibility::_(hView, bVisible)) { throw ApException("Msg_WebView_Visibility failed"); }
    if (!Msg_WebView_SetScriptAccessPolicy::Allow(hView)) { throw ApException("Msg_WebView_SetScriptAccessPolicy failed"); }
    if (!Msg_WebView_Load::_(hView, "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "Dialog.html")) { throw ApException("Msg_WebView_Load failed"); }

    //if (!Msg_WebView_SetNavigationPolicy::Deny(hView)) { throw ApException("Msg_WebView_SetNavigationPolicy failed"); }

  } catch (ApException& ex) {
    Msg_WebView_Destroy::_(hView);
    throw ex;
  }

  hView_ = hView;
}

void Dialog::Destroy()
{
  if (!Msg_WebView_Destroy::_(hView_)) { throw ApException("Msg_WebView_Destroy failed"); }
  hView_ = ApNoHandle;
}

void Dialog::OnDocumentLoaded()
{
  {
    Msg_WebView_CallScriptFunction msg;
    msg.hView = hView_;
    msg.sFunction = "SetContent";
    msg.lArgs.AddLast(sContentUrl_);
    if (!msg.Request()) { apLog_Error((LOG_CHANNEL, "Dialog::OnDocumentLoaded", "%s(%s) failed: %s", StringType(msg.Type()), StringType(msg.sFunction), StringType(msg.sComment))); }
  }

  if (sCaption_) {
    Msg_WebView_CallScriptFunction msg;
    msg.hView = hView_;
    msg.sFunction = "SetCaption";
    msg.lArgs.AddLast(sCaption_);
    if (!msg.Request()) { apLog_Error((LOG_CHANNEL, "Dialog::OnDocumentLoaded", "%s(%s) failed: %s", StringType(msg.Type()), StringType(msg.sFunction), StringType(msg.sComment))); }
  }

  if (sIconUrl_) {
    Msg_WebView_CallScriptFunction msg;
    msg.hView = hView_;
    msg.sFunction = "SetIcon";
    msg.lArgs.AddLast(sIconUrl_);
    if (!msg.Request()) { apLog_Error((LOG_CHANNEL, "Dialog::OnDocumentLoaded", "%s(%s) failed: %s", StringType(msg.Type()), StringType(msg.sFunction), StringType(msg.sComment))); }
  }
}

void Dialog::OnReceivedFocus()
{
}

void Dialog::OnLostFocus()
{
}

