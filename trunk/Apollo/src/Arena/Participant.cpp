// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "ApLog.h"
#include "MsgVpView.h"
#include "MsgAnimation.h"
#include "Local.h"
#include "Participant.h"

Participant::Participant(ApHandle hParticipant)
:hAp_(hParticipant)
,hAnimatedItem_(ApNoHandle)
{
  avatarMimeTypes_.add("avatar/gif");
  avatarMimeTypes_.add("image/gif");
  avatarMimeTypes_.add("image/png");
}

void Participant::SubscribeAndGetDetail(const String& sKey)
{
  Msg_VpView_SubscribeParticipantDetail msg;
  msg.hParticipant = hAp_;
  msg.sKey = sKey;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
    msg.vlMimeTypes = avatarMimeTypes_;
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
  } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
    GetDetailData(sKey, avatarMimeTypes_);
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
      sNickname_ = msg.sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_Message) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_Position) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_Condition) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_ProfileUrl) {
      //= sValue;

    }
  }
}

void Participant::GetDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailData msg;
  msg.hParticipant = hAp_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {

      if (!ApIsHandle(hAnimatedItem_)) {
        Msg_Animation_Create msgAC;
        msgAC.hItem = Apollo::newHandle();
        msgAC.sMimeType = msg.sMimeType;
        if (!msgAC.Request()) {
          apLog_Error((LOG_CHANNEL, "Participant::GetDetailData", "Msg_Animation_Create failed: participant=" ApHandleFormat "", ApHandleType(hAp_)));
        } else {
          hAnimatedItem_ = msgAC.hItem;
        }
      }

      if (ApIsHandle(hAnimatedItem_)) {
        Msg_Animation_SetRate msgASR;
        msgASR.hItem = hAnimatedItem_;
        msgASR.nMaxRate = 10;
        if (!msgASR.Request()) {
          apLog_Error((LOG_CHANNEL, "Participant::GetDetailData", "Msg_Animation_SetRate failed: participant=" ApHandleFormat "", ApHandleType(hAp_)));
        }
      }

      if (ApIsHandle(hAnimatedItem_)) {
        Msg_Animation_SetData msgASD;
        msgASD.hItem = hAnimatedItem_;
        msgASD.sbData = msg.sbData;

        String sSource = msg.sSource;
        String sSourceType;
        sSource.nextToken(Msg_VpView_ParticipantDetail_SourceSeparator, sSourceType);
        if (sSourceType == Msg_VpView_ParticipantDetail_SourcePrefix_IdentityItemUrl) {
          String sUrl = sSource;
          msgASD.sSourceUrl = sUrl;
        }

        if (!msgASD.Request()) {
          apLog_Error((LOG_CHANNEL, "Participant::GetDetailData", "Msg_Animation_SetData failed: participant=" ApHandleFormat " key=%s data:%d bytes, source=%s", ApHandleType(hAp_), StringType(sKey), msg.sbData.Length(), StringType(sSource)));
        }
      }

      if (ApIsHandle(hAnimatedItem_)) {
        Msg_Animation_Start msgAS;
        msgAS.hItem = hAnimatedItem_;
        if (!msgAS.Request()) {
          apLog_Error((LOG_CHANNEL, "Participant::GetDetailData", "Msg_Animation_Start failed: participant=" ApHandleFormat "", ApHandleType(hAp_)));
        }
      }

    }
  }
}

void Participant::UnSubscribeDetail(const String& sKey)
{
}

void Participant::Show()
{
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Nickname);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_avatar);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Message);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Position);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Condition);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_ProfileUrl);
}

void Participant::Hide()
{
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Nickname);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_avatar);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Message);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Position);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Condition);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_ProfileUrl);

  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_Destroy msg;
    msg.hItem = hAnimatedItem_;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Participant::Hide", "Msg_Animation_Destroy failed: participant=" ApHandleFormat "", ApHandleType(hAp_)));
    }
  }
}

void Participant::DetailsChanged(Apollo::ValueList& vlKeys)
{
  for (Apollo::ValueElem* e = 0; e = vlKeys.nextElem(e); ) {
    String sKey = e->getString();
    GetDetail(sKey);
  }
}
