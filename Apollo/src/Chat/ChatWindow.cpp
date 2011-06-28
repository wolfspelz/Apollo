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
    msg.sCaption = Apollo::translate(MODULE_NAME, "", "Chat");
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
  {
    Msg_Vp_AddLocationContext msg;
    msg.hLocation = hLocation;
    msg.hContext = hContext_;
    if (!msg.Request()) { throw ApException("ChatWindow::AttachToLocation() %s failed: %s" ApHandleFormat "", StringType(msg.Type()), StringType(msg.sComment)); }
  }

  {
    Msg_VpView_GetLocationContexts msg;
    msg.hLocation = hLocation;
    if (msg.Request()) {
      for (Apollo::ValueElem* e = 0; (e = msg.vlContexts.nextElem(e)) != 0; ) {
        Msg_VpView_GetContextDetail msgVVGCD;
        msgVVGCD.hContext = e->getHandle();
        msgVVGCD.sKey = Msg_VpView_ContextDetail_DocumentUrl;
        if (msgVVGCD.Request()) {
          if (msgVVGCD.sValue) {
            Msg_Dialog_SetCaption msgDSC;
            msgDSC.hDialog = hAp_;
            msgDSC.sCaption = Apollo::translate(MODULE_NAME, "", "Chat") + " " + msgVVGCD.sValue;
            msgDSC.Request();
            break;
          }
        }
      }
    }
  }

  hLocation_ = hLocation;
}

//---------------------------------------------------

void ChatWindow::OnLoaded()
{
  { Msg_VpView_SubscribeLocationDetail msg; msg.hLocation = hLocation_; msg.sKey = Msg_VpView_LocationDetail_State; msg.Request(); }
  { Msg_VpView_GetLocationDetail msg; msg.hLocation = hLocation_; msg.sKey = Msg_VpView_LocationDetail_State; if (msg.Request()) { ShowLocationDetailState(msg.sValue); } }

  {
    Msg_VpView_GetParticipants msg;
    msg.hLocation = hLocation_;
    if (!msg.Request()) {
      apLog_Warning((LOG_CHANNEL, "ChatWindow::OnLoaded", "% failed: %s", StringType(msg.Type()), StringType(msg.sComment)));
    } else {
      for (Apollo::ValueElem* e = 0; e = msg.vlParticipants.nextElem(e); ) {
        ApHandle hParticipant = e->getHandle();
        OnParticipantAdded(hParticipant, hParticipant == msg.hSelf);
      }
    }
  }
}

//---------------------------------------------------

void ChatWindow::OnUnload()
{
  Msg_Vp_DestroyContext msg;
  msg.hContext = hContext_;
  if (!msg.Request()) { throw ApException("ChatWindow::OnUnload() %s failed: %s" ApHandleFormat "", StringType(msg.Type()), StringType(msg.sComment)); }
}

void ChatWindow::OnModuleCall(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response)
{
  String sMethod = request.getString(Srpc::Key::Method);

  if (0){
  } else if (sMethod == "SendChat") {
    String sText = request.getString("sText");
    if (sText){
      Msg_Vp_SendPublicChat msg;
      msg.hLocation = hLocation_;
      msg.sText = sText;
      if (!msg.Request()) { throw ApException("ChatWindow::OnModuleCall() %s failed  loc=" ApHandleFormat ": %s", StringType(msg.Type()), ApHandleType(hLocation_), StringType(msg.sComment)); }
    }

  } else {
    throw ApException("ChatWindow::OnModuleCall() Unknown Method=%s", StringType(sMethod));
  }
}

void ChatWindow::OnParticipantAdded(const ApHandle& hParticipant, int bSelf)
{
  if (participants_.Find(hParticipant) == 0) {
    Participant* pParticipant = new Participant(pModule_, this, hParticipant, bSelf);
    if (pParticipant) {
      participants_.Set(hParticipant, pParticipant);
      pParticipant->Create();
    }
  }
}

void ChatWindow::OnParticipantRemoved(const ApHandle& hParticipant)
{
  ParticipantListNode* pNode = participants_.Find(hParticipant);
  if (pNode != 0) {
    Participant* pParticipant = pNode->Value();
    participants_.Unset(hParticipant);
    if (pParticipant) {
      pParticipant->Destroy();
      delete pParticipant;
      pParticipant = 0;
    }
  }
}

void ChatWindow::OnParticipantDetailsChanged(const ApHandle& hParticipant, Apollo::ValueList& vlKeys)
{
  ParticipantListNode* pNode = participants_.Find(hParticipant);
  if (pNode) {
    Participant* pParticipant = pNode->Value();
    pParticipant->OnDetailsChanged(vlKeys);
  }
}

void ChatWindow::OnReceivePublicChat(const ApHandle& hParticipant, const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv)
{
  ParticipantListNode* pNode = participants_.Find(hParticipant);
  if (pNode) {
    Participant* pParticipant = pNode->Value();
    pParticipant->OnReceivePublicChat(hChat, sNickname, sText, tv);
  }
}

void ChatWindow::OnLocationDetailsChanged(Apollo::ValueList& vlKeys)
{
  Msg_VpView_GetLocationDetail msg;
  msg.hLocation = hLocation_;

  for (Apollo::ValueElem* e = 0; e = vlKeys.nextElem(e); ) {
    msg.sKey = e->getString();

    if (0) {
    } else if (e->getString() == Msg_VpView_LocationDetail_State) {
      if (msg.Request()) {
        ShowLocationDetailState(msg.sValue);
      }
    }
  }
}

// ----------------------------------

void ChatWindow::ShowLocationDetailState(const String& sValue)
{
  //ViewCall vc(this, "SetLocationState");
}

//---------------------------------------------------

ViewCall::ViewCall(ChatWindow* pChatWindow, const String& sMethod)
{
  srpc.set(Srpc::Key::Method, sMethod);
  hDialog = pChatWindow->apHandle();
  sFunction = Apollo::getModuleConfig(MODULE_NAME, "CallScriptSrpcFunctionName", "ReceiveMessageFromModule");
}

int ViewCall::Request()
{
  if (ApIsHandle(hDialog)) {
    return base::Request();
  } else {
    return 0;
  }
}
