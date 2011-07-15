// ============================================================================
//
// Apollo
//
// ============================================================================

#include "Apollo.h"
#include "MsgVpView.h"
#include "MsgAnimation.h"
#include "ColorString.h"
#include "Local.h"
#include "Avatar.h"
#include "WebArenaModule.h"

Avatar::Avatar(WebArenaModule* pModule, Display* pDisplay, const ApHandle& hParticipant)
:pModule_(pModule)
,pDisplay_(pDisplay)
,hParticipant_(hParticipant)
,hAnimatedItem_(ApNoHandle)
,nX_(300)
,nPositionConfirmed_(0)
{
  avatarMimeTypes_.add("avatar/gif");
  avatarMimeTypes_.add("image/gif");
  avatarMimeTypes_.add("image/png");
}

void Avatar::SubscribeAndGetDetail(const String& sKey)
{
  Msg_VpView_SubscribeParticipantDetail msg;
  msg.hParticipant = hParticipant_;
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

void Avatar::GetDetail(const String& sKey)
{
  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
    GetDetailData(sKey, avatarMimeTypes_);
//Ref    GetDetailRef(sKey, avatarMimeTypes_);
  } else {
    GetDetailString(sKey, noMimeTypes_);
  }
}

void Avatar::GetDetailString(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailString msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_Nickname) {
      SetNickname(msg.sValue);

    } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_Message) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_Position) {
      List lCoords;
      KeyValueLfBlob2List(msg.sValue, lCoords);
      Elem* e = lCoords.FindByNameCase("x");
      if (e) {
        int nX = String::atoi(e->getString());
        SetPosition(nX);
      }

    } else if (sKey == Msg_VpView_ParticipantDetail_Condition) {
      //= sValue;

    } else if (sKey == Msg_VpView_ParticipantDetail_ProfileUrl) {
      //= sValue;

    }
  }
}

void Avatar::GetDetailRef(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailRef msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
      SetImage(msg.sUrl);
    }
  }
}

void Avatar::GetDetailData(const String& sKey, Apollo::ValueList& vlMimeTypes)
{
  Msg_VpView_GetParticipantDetailData msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;
  msg.vlMimeTypes = vlMimeTypes;
  if (msg.Request()) {

    if (0) {
    } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
      HandleImageData(msg.sMimeType, msg.sSource, msg.sbData);
    }
  }
}

void Avatar::HandleImageData(const String& sMimeType, const String& sSource, Buffer& sbData)
{
  if (!ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_Create msgAC;
    msgAC.hItem = Apollo::newHandle();
    msgAC.sMimeType = sMimeType;
    if (!msgAC.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::HandleImageData", "Msg_Animation_Create failed: participant=" ApHandleFormat "", ApHandleType(hParticipant_)));
    } else {
      hAnimatedItem_ = msgAC.hItem;
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_SetRate msgASR;
    msgASR.hItem = hAnimatedItem_;
    msgASR.nMaxRate = 10;
    if (!msgASR.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::HandleImageData", "Msg_Animation_SetRate failed: participant=" ApHandleFormat "", ApHandleType(hParticipant_)));
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_SetData msgASD;
    msgASD.hItem = hAnimatedItem_;
    msgASD.sbData = sbData;

    String sSourceTokenizer = sSource;
    String sSourceType;
    sSourceTokenizer.nextToken(Msg_VpView_ParticipantDetail_SourceSeparator, sSourceType);
    if (sSourceType == Msg_VpView_ParticipantDetail_SourcePrefix_IdentityItemUrl) {
      String sUrl = sSourceTokenizer;
      msgASD.sSourceUrl = sUrl;
    }

    if (!msgASD.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::HandleImageData", "Msg_Animation_SetData failed: participant=" ApHandleFormat " data:%d bytes, source=%s", ApHandleType(hParticipant_), sbData.Length(), StringType(sSource)));
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    if (pModule_) {
      pModule_->SetContextOfHandle(hAnimatedItem_, pDisplay_->GetContext());
      pModule_->SetParticipantOfAnimation(hAnimatedItem_, hParticipant_);
    }

    Msg_Animation_Start msgAS;
    msgAS.hItem = hAnimatedItem_;
    if (!msgAS.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::HandleImageData", "Msg_Animation_Start failed: participant=" ApHandleFormat "", ApHandleType(hParticipant_)));
    }
  }
}

void Avatar::UnSubscribeDetail(const String& sKey)
{
  Msg_VpView_UnsubscribeParticipantDetail msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_avatar) {
    msg.vlMimeTypes = avatarMimeTypes_;
  } else {
    msg.vlMimeTypes = noMimeTypes_;
  }

  (void) msg.Request();
}


void Avatar::Create(int bSelf)
{
  DisplaySrpcMessage dsm(pDisplay_, "AddAvatar");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setInt("bSelf", bSelf);
  dsm.srpc.setInt("nX", nX_);
  dsm.Request();

  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Nickname);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_avatar);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Message);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Position);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Condition);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_ProfileUrl);
}

void Avatar::Destroy()
{
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Nickname);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_avatar);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Message);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Position);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_Condition);
  UnSubscribeDetail(Msg_VpView_ParticipantDetail_ProfileUrl);

  if (ApIsHandle(hAnimatedItem_)) {
    if (pModule_) {
      pModule_->DeleteContextOfHandle(hAnimatedItem_, pDisplay_->GetContext());
      pModule_->DeleteParticipantOfAnimation(hAnimatedItem_, hParticipant_);
    }

    Msg_Animation_Stop msgAS;
    msgAS.hItem = hAnimatedItem_;
    if (!msgAS.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::Hide", "Msg_Animation_Stop failed: participant=" ApHandleFormat "", ApHandleType(hParticipant_)));
    }
  }

  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_Destroy msg;
    msg.hItem = hAnimatedItem_;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::Hide", "Msg_Animation_Destroy failed: participant=" ApHandleFormat "", ApHandleType(hParticipant_)));
    }
  }

  DisplaySrpcMessage dsm(pDisplay_, "RemoveAvatar");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.Request();
}

void Avatar::OnDetailsChanged(Apollo::ValueList& vlKeys)
{
  for (Apollo::ValueElem* e = 0; e = vlKeys.nextElem(e); ) {
    String sKey = e->getString();
    GetDetail(sKey);
  }
}

void Avatar::OnReceivePublicChat(const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tv)
{
  Chatline* pChat = 0;
  chats_.Get(hChat, pChat);
  if (pChat) {
    if (pChat->sText_ != sText) {
      pChat->sText_ = sText;
      SetChatline(hChat, sText);
    }
    if (pChat->tv_ != tv) {
      pChat->tv_ = tv;
    }
  } else {
    pChat = new Chatline(sText, tv);
    chats_.Set(hChat, pChat);

    RemoveOldChats(3);

    if (tvNewestChat_ < tv) {
      tvNewestChat_ = tv;
      AddChatline(hChat, sText);
    }
  }
}

void Avatar::OnAnimationBegin(const String& sUrl)
{
  if (sUrl != sImage_) {
    SetImage(sUrl);
    sImage_ = sUrl;
  }
}

//----------------------------------------------------------

void Avatar::RemoveOldChats(int nMax)
{
  while (chats_.Count() > nMax) {
    Apollo::TimeValue tvOldestChat;
    ApHandle hOldestChat;
    ChatlineListNode* node = 0;
    for (ChatlineListIterator iter(chats_); (node = iter.Next()); ) {
      if (tvOldestChat.isNull() || node->Value()->tv_ < tvOldestChat) {
        tvOldestChat = node->Value()->tv_;
        hOldestChat = node->Key();
      }
    }
    if (ApIsHandle(hOldestChat)) {
      Chatline* pOldestChat = 0;
      chats_.Get(hOldestChat, pOldestChat);
      chats_.Unset(hOldestChat);
      delete pOldestChat;
      pOldestChat = 0;
      RemoveChatline(hOldestChat);
    }
  }
}

//----------------------------------------------------------

void Avatar::SetNickname(const String& sNickname)
{
  sNickname_ = sNickname;

  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarNickname");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setString("sNickname", sNickname_);
  dsm.Request();
}

void Avatar::SetImage(const String& sUrl)
{
  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarImage");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setString("sUrl", sUrl);
  dsm.Request();
}

void Avatar::CreateChatContainer(const String& sContainer)
{
}

void Avatar::DeleteAllChatBubbles(const String& sContainer)
{
}

void Avatar::AddChatline(const ApHandle& hChat, const String& sText)
{
  DisplaySrpcMessage dsm(pDisplay_, "AddAvatarChat");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setString("hChat", hChat.toString());
  dsm.srpc.setString("sText", sText);
  dsm.Request();
}

void Avatar::SetChatline(const ApHandle& hChat, const String& sText)
{
  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarChat");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setString("hChat", hChat.toString());
  dsm.srpc.setString("sText", sText);
  dsm.Request();
}

void Avatar::RemoveChatline(const ApHandle& hChat)
{
  DisplaySrpcMessage dsm(pDisplay_, "RemoveAvatarChat");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setString("hChat", hChat.toString());
  dsm.Request();
}

void Avatar::SetUnknownPosition()
{
  int nX = 300;
  int nMin = 100;
  int nMax = 500;

  if (pDisplay_) {
    nMax = pDisplay_->GetWidth();
  }

  nX = nMin + Apollo::getRandom(nMax - nMin);

  nPositionConfirmed_ = 0;

  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarPosition");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setInt("nX", nX);
  dsm.Request();
}

void Avatar::SetPosition(int nX)
{
  nX_ = nX;
  nPositionConfirmed_ = 1;

  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarPosition");
  dsm.srpc.setString("hParticipant", hParticipant_.toString());
  dsm.srpc.setInt("nX", nX);
  dsm.Request();
}
