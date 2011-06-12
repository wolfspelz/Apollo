// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "Local.h"
#include "ChatWindow.h"

ChatWindow::~ChatWindow()
{
}

void ChatWindow::Open()
{
  hContext_ = Apollo::newHandle();

  {
    Msg_Vp_CreateContext msg;
    msg.hContext = hContext_;
    if (!msg.Request()) { throw ApException("ChatWindow::Open() %s failed" ApHandleFormat "", StringType(msg.Type())); }
  }

  {
    // Use ChatWindow handle as Dialog handle
    // So we can hook Dialog_OnClosed events easily.

    Msg_Dialog_Create msg;
    msg.hDialog = hAp_; 
    msg.nLeft = Apollo::getModuleConfig(MODULE_NAME, "Left", 200);
    msg.nTop = Apollo::getModuleConfig(MODULE_NAME, "Top", 200);
    msg.nWidth = Apollo::getModuleConfig(MODULE_NAME, "Width", 500);
    msg.nHeight = Apollo::getModuleConfig(MODULE_NAME, "Height", 300);
    msg.bVisible = 1;
    msg.sCaption = "Chat";
    msg.sIconUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "icon.png";
    msg.sContentUrl = "file://" + Apollo::getModuleResourcePath(MODULE_NAME) + "Chat.html";
    if (!msg.Request()) { throw ApException("%s failed: %s", StringType(msg.Type()), StringType(msg.sComment)); }
  }

}

void ChatWindow::Close()
{
  Msg_Dialog_Destroy msg;
  msg.hDialog = hAp_;
  if (!msg.Request()) { throw ApException("%s failed: %s", StringType(msg.Type()), StringType(msg.sComment)); }
}

void ChatWindow::AttachToLocation(const ApHandle& hLocation)
{
  Msg_Vp_AddLocationContext msg;
  msg.hLocation = hLocation;
  msg.hContext = hContext_;
  if (!msg.Request()) { throw ApException("ChatWindow::AttachToLocation() %s failed: %s" ApHandleFormat "", StringType(msg.Type()), StringType(msg.sComment)); }

  hLocation_ = hLocation;
}

void ChatWindow::OnUnload()
{
  Msg_Vp_DestroyContext msg;
  msg.hContext = hContext_;
  if (!msg.Request()) { throw ApException("ChatWindow::OnUnload() %s failed: %s" ApHandleFormat "", StringType(msg.Type()), StringType(msg.sComment)); }
}

void ChatWindow::OnCallModule(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response)
{
  String sMethod = request.getString("Method");

  if (0){
  } else if (sMethod == "SendChat") {
    String sText = request.getString("sText");
    if (sText){
      Msg_Vp_SendPublicChat msg;
      msg.hLocation = hLocation_;
      msg.sText = sText;
      if (!msg.Request()) { throw ApException("ChatWindow::OnCallModule() %s failed  loc=" ApHandleFormat ": %s", StringType(msg.Type()), ApHandleType(hLocation_), StringType(msg.sComment)); }
    }

  } else {
    throw ApException("ChatWindow::OnCallModule() Unknown Method=%s", StringType(sMethod));
  }
}

