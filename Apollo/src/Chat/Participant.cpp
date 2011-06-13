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
//  ViewSrpcMessage vsm(pDisplay_, "AddParticipant");

  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Nickname);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Image);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
}

void Participant::Destroy()
{
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_Nickname);
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_Image);
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_OnlineStatus);

//  ViewSrpcMessage vsm(pDisplay_, "RemoveParticipant");
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
        //DisplaySetNickname(sNickname_);
      }

    } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) {
      if (sOnlineStatus_ != msg.sValue) {
        sOnlineStatus_ = msg.sValue;
        //DisplaySetOnlineStatus(sOnlineStatus_);
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
        //DisplaySetImage(sUrl);
        sImage_ = sUrl;
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
  //
}

//----------------------------------------------------------

void Participant::DisplaySetNickname(const String& sNickname)
{
  //ViewSrpcMessage vsm(pDisplay_, "SetParticipantNickname");
}

void Participant::DisplaySetImage(const String& sUrl)
{
  //ViewSrpcMessage vsm(pDisplay_, "SetParticipantImage");
}

void Participant::DisplaySetOnlineStatus(const String& sStatus)
{
  //ViewSrpcMessage vsm(pDisplay_, "SetOnlineStatus");
}

void Participant::DisplayAddChatline(const ApHandle& hChat, const String& sText)
{
  //ViewSrpcMessage vsm(pDisplay_, "AddParticipantChat");
}
