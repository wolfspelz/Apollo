// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgVpView.h"
#include "Local.h"
#include "Participant.h"
#include "ChatModule.h"

Apollo::ValueList Participant::noMimeTypes_;
Apollo::ValueList Participant::imageMimeTypes_;

Participant::Participant(ChatModule* pModule, ChatWindow* pChatWindow, const ApHandle& hParticipant, int bSelf)
:pModule_(pModule)
,pChatWindow_(pChatWindow)
,hAp_(hParticipant)
,bSelf_(bSelf)
{
  if (imageMimeTypes_.length() == 0) {
    imageMimeTypes_.add("image/gif");
    imageMimeTypes_.add("image/png");
    imageMimeTypes_.add("image/jpeg");
    imageMimeTypes_.add("image/jpg");
  }
}

void Participant::Create()
{
  Msg_VpView_GetParticipantDetailString msg;
  msg.hParticipant = hAp_;
  msg.sKey = Msg_VpView_ParticipantDetail_Nickname;
  if (msg.Request()) {
    sNickname_ = msg.sValue;
  } else {
    sNickname_ = Apollo::getModuleConfig(MODULE_NAME, "DefaultNickname", "User") + hAp_.toString();
  }

  ViewCall vc(pChatWindow_, "AddParticipant");
  vc.srpc.set("hParticipant", hAp_);
  vc.srpc.set("sNickname", sNickname_);
  vc.srpc.set("bSelf", bSelf_);
  vc.Request();

  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Nickname);
  //SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Image);
  //SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
}

void Participant::Destroy()
{
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_Nickname);
  //UnsubscribeDetail(Msg_VpView_ParticipantDetail_Image);
  //UnsubscribeDetail(Msg_VpView_ParticipantDetail_OnlineStatus);

  ViewCall vc(pChatWindow_, "RemoveParticipant");
  vc.srpc.set("hParticipant", hAp_);
  vc.Request();
}

void Participant::SubscribeAndGetDetail(const String& sKey)
{
  Msg_VpView_SubscribeParticipantDetail msg;
  msg.hParticipant = hAp_;
  msg.sKey = sKey;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_Image) {
    msg.vlMimeTypes = imageMimeTypes_;
  } else {
    msg.vlMimeTypes = noMimeTypes_;
  }

  if (msg.Request()) {
    if (msg.bAvailable) {
      GetDetail(sKey);
    }
  }
}

void Participant::GetDetail(const String& sKey)
{
  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_Image) {
    GetDetailUrl(sKey, imageMimeTypes_);
  } else {
    GetDetailString(sKey, noMimeTypes_);
  }
}

void Participant::GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailString msg;
  msg.hParticipant = hAp_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_Nickname) {
      if (sNickname_ != msg.sValue) {
        sNickname_ = msg.sValue;

        ViewCall vc(pChatWindow_, "SetNickname");
        vc.srpc.set("hParticipant", hAp_);
        vc.srpc.set("sNickname", sNickname_);
        vc.Request();
      }

    } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) {
      if (sOnlineStatus_ != msg.sValue) {
        sOnlineStatus_ = msg.sValue;

        //ViewCall vc(pChatWindow_, "SetOnlineStatus");
        //vc.srpc.set("hParticipant", hAp_);
        //vc.srpc.set("sStatus", sOnlineStatus_);
        //vc.Request();
      }

    }
  }
}

void Participant::GetDetailUrl(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailUrl msg;
  msg.hParticipant = hAp_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_Image) {
      String sUrl = msg.sUrl;
      if (sUrl != sImage_) {
        sImage_ = sUrl;

        //ViewCall vc(pChatWindow_, "SetImage");
        //vc.srpc.set("hParticipant", hAp_);
        //vc.srpc.set("sUrl", sImage_);
        //vc.Request();
      }

    }
  }
}

void Participant::UnsubscribeDetail(const String& sKey)
{
  Msg_VpView_UnsubscribeParticipantDetail msg;
  msg.hParticipant = hAp_;
  msg.sKey = sKey;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_Image) {
    msg.vlMimeTypes = imageMimeTypes_;
  } else {
    msg.vlMimeTypes = noMimeTypes_;
  }

  (void) msg.Request();
}


void Participant::OnDetailsChanged(Apollo::ValueList& vlKeys)
{
  for (Apollo::ValueElem* e = 0; e = vlKeys.nextElem(e); ) {
    String sKey = e->getString();
    GetDetail(sKey);
  }
}

void Participant::OnReceivePublicChat(const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv)
{
  ViewCall vc(pChatWindow_, "AddChatline");
  vc.srpc.set("hParticipant", hAp_);
  vc.srpc.set("sNickname", sNickname);
  vc.srpc.set("sText", sText);
  vc.Request();
}
