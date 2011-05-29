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
#include "ArenaModule.h"

Avatar::Avatar(ArenaModule* pModule, Display* pDisplay, const ApHandle& hParticipant)
:pModule_(pModule)
,pDisplay_(pDisplay)
,hParticipant_(hParticipant)
,hAnimatedItem_(ApNoHandle)
,nX_(300)
,nPositionConfirmed_(0)
,bMoving_(0)
{
  avatarMimeTypes_.add("avatar/gif");
  //avatarMimeTypes_.add("image/gif");
  //avatarMimeTypes_.add("image/png");
}

void Avatar::Create(int bSelf)
{
  DisplaySrpcMessage dsm(pDisplay_, "AddAvatar");
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.srpc.set("bSelf", bSelf);
  dsm.srpc.set("nX", nX_);
  dsm.Request();

  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Nickname);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Avatar);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  //SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Message);
  SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Position);
  //SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_Condition);
  //SubscribeAndGetDetail(Msg_VpView_ParticipantDetail_ProfileUrl);
  SubscribeAndGetDetail("CommunityTag");
  SubscribeAndGetDetail("CommunityName");
  SubscribeAndGetDetail("CommunityPage");
  SubscribeAndGetDetail("AttachedIcon");
  SubscribeAndGetDetail("AttachedIconName");
}

void Avatar::Destroy()
{
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_Nickname);
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_Avatar);
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_OnlineStatus);
  //UnsubscribeDetail(Msg_VpView_ParticipantDetail_Message);
  UnsubscribeDetail(Msg_VpView_ParticipantDetail_Position);
  //UnsubscribeDetail(Msg_VpView_ParticipantDetail_Condition);
  //UnsubscribeDetail(Msg_VpView_ParticipantDetail_ProfileUrl);
  UnsubscribeDetail("CommunityTag");
  UnsubscribeDetail("CommunityName");
  UnsubscribeDetail("CommunityPage");
  UnsubscribeDetail("AttachedIcon");
  UnsubscribeDetail("AttachedIconName");

  if (ApIsHandle(hAnimatedItem_)) {
    if (pModule_) {
      pModule_->DeleteContextOfHandle(hAnimatedItem_, pDisplay_->GetContext());
      pModule_->DeleteParticipantOfAnimation(hAnimatedItem_, hParticipant_);
    }

    Msg_Animation_Stop msg;
    msg.hItem = hAnimatedItem_;
    if (!msg.Request()) {
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
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.Request();
}

void Avatar::SubscribeAndGetDetail(const String& sKey)
{
  Msg_VpView_SubscribeParticipantDetail msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_Avatar) {
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
  } else if (sKey == Msg_VpView_ParticipantDetail_Avatar) {
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
      if (sNickname_ != msg.sValue) {
        sNickname_ = msg.sValue;
        DisplaySetNickname(sNickname_);
      }

    } else if (sKey == Msg_VpView_ParticipantDetail_OnlineStatus) {
      if (sOnlineStatus_ != msg.sValue) {
        sOnlineStatus_ = msg.sValue;

        if (sOnlineStatus_ == "away" || sOnlineStatus_ == "xa" || sOnlineStatus_ == "dnd") {
          SuspendAnimation();
        } else {
          ResumeAnimation();
        }

        DisplaySetOnlineStatus(sOnlineStatus_);
      }

    } else if (sKey == Msg_VpView_ParticipantDetail_Message) {

    } else if (sKey == Msg_VpView_ParticipantDetail_Position) {
      List lCoords;
      KeyValueLfBlob2List(msg.sValue, lCoords);
      Elem* e = lCoords.FindByNameCase("x");
      if (e) {
        int nX = String::atoi(e->getString());
        if (nX_ != nX) {

          if (!nPositionConfirmed_) {
            nPositionConfirmed_ = 1;
            DisplaySetAvatarPosition(nX);
          } else {
            BeginMove(nX);
          }
          nX_ = nX;

        }
      }

    } else if (sKey == Msg_VpView_ParticipantDetail_Condition) {

    } else if (sKey == Msg_VpView_ParticipantDetail_ProfileUrl) {

    } else if (sKey == "CommunityTag") {
      if (sCommunityTag_ != msg.sValue) {
        sCommunityTag_ = msg.sValue;
        if (sCommunityTag_) {
          DisplaySetCommunityAttachment(sCommunityTag_, sCommunityName_, sCommunityPage_);
        }
      }

    } else if (sKey == "CommunityName") {
      if (sCommunityName_ != msg.sValue) {
        sCommunityName_ = msg.sValue;
        if (sCommunityTag_) {
          DisplaySetCommunityAttachment(sCommunityTag_, sCommunityName_, sCommunityPage_);
        }
      }

    } else if (sKey == "CommunityPage") {
      if (sCommunityPage_ != msg.sValue) {
        sCommunityPage_ = msg.sValue;
        if (sCommunityTag_) {
          DisplaySetCommunityAttachment(sCommunityTag_, sCommunityName_, sCommunityPage_);
        }
      }

    } else if (sKey == "AttachedIcon") {
      if (sAttachedIcon_ != msg.sValue) {
        sAttachedIcon_ = msg.sValue;
        if (sAttachedIcon_) {
          DisplaySetIconAttachment(sAttachedIcon_, sAttachedIconName_);
        }
      }

    } else if (sKey == "AttachedIconName") {
      if (sAttachedIconName_ != msg.sValue) {
        sAttachedIconName_ = msg.sValue;
        if (sAttachedIcon_) {
          DisplaySetIconAttachment(sAttachedIcon_, sAttachedIconName_);
        }
      }

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
    } else if (sKey == Msg_VpView_ParticipantDetail_Avatar) {
      DisplaySetImage(msg.sUrl);
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
    } else if (sKey == Msg_VpView_ParticipantDetail_Avatar) {
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

void Avatar::SuspendAnimation()
{
  if (ApIsHandle(hAnimatedItem_)) {
    //{
    //  Msg_Animation_Static msg;
    //  msg.hItem = hAnimatedItem_;
    //  msg.bState = 1;
    //  if (!msg.Request()) {
    //    apLog_Error((LOG_CHANNEL, "Avatar::SuspendAnimation", "%s failed: participant=" ApHandleFormat "", StringType(msg.Type()), ApHandleType(hParticipant_)));
    //  }
    //}

    {
      Msg_Animation_Activity msg;
      msg.hItem = hAnimatedItem_;
      msg.sActivity = Msg_Animation_Activity_Idle;
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, "Avatar::SuspendAnimation", "%s failed: participant=" ApHandleFormat " activity=%s", StringType(msg.Type()), ApHandleType(hParticipant_), StringType(msg.sActivity)));
      }
    }

    {
      Msg_Animation_Stop msg;
      msg.hItem = hAnimatedItem_;
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, "Avatar::SuspendAnimation", "%s failed: participant=" ApHandleFormat "", StringType(msg.Type()), ApHandleType(hParticipant_)));
      }
    }
  }
}

void Avatar::ResumeAnimation()
{
  if (ApIsHandle(hAnimatedItem_)) {
    //{
    //  Msg_Animation_Static msg;
    //  msg.hItem = hAnimatedItem_;
    //  msg.bState = 0;
    //  if (!msg.Request()) {
    //    apLog_Error((LOG_CHANNEL, "Avatar::ResumeAnimation", "%s failed: participant=" ApHandleFormat "", StringType(msg.Type()), ApHandleType(hParticipant_)));
    //  }
    //}

    {
      Msg_Animation_Activity msg;
      msg.hItem = hAnimatedItem_;
      msg.sActivity = "";
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, "Avatar::ResumeAnimation", "%s failed: participant=" ApHandleFormat "", StringType(msg.Type()), ApHandleType(hParticipant_)));
      }
    }

    {
      Msg_Animation_Start msg;
      msg.hItem = hAnimatedItem_;
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, "Avatar::ResumeAnimation", "%s failed: participant=" ApHandleFormat "", StringType(msg.Type()), ApHandleType(hParticipant_)));
      }
    }
  }
}

void Avatar::BeginMove(int nDestX)
{
  bMoving_ = 1;

  if (ApIsHandle(hAnimatedItem_) && nX_ != nDestX) {

    Msg_Animation_Activity msg;
    msg.hItem = hAnimatedItem_;
    msg.sActivity = (nDestX > nX_ ? Msg_Animation_Activity_MoveRight : Msg_Animation_Activity_MoveLeft);
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::BeginMove", "%s failed: participant=" ApHandleFormat " activity=%s", StringType(msg.Type()), ApHandleType(hParticipant_), StringType(msg.sActivity)));
    }

  }

  DisplayMoveAvatarPosition(nDestX);
}

void Avatar::EndMove(int nDestX)
{
  bMoving_ = 0;

  if (ApIsHandle(hAnimatedItem_)) {

    Msg_Animation_Activity msg;
    msg.hItem = hAnimatedItem_;
    msg.sActivity = "";
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::EndMove", "%s failed: participant=" ApHandleFormat "", StringType(msg.Type()), ApHandleType(hParticipant_)));
    }

  }
}

void Avatar::UnsubscribeDetail(const String& sKey)
{
  Msg_VpView_UnsubscribeParticipantDetail msg;
  msg.hParticipant = hParticipant_;
  msg.sKey = sKey;

  if (0) {
  } else if (sKey == Msg_VpView_ParticipantDetail_Avatar) {
    msg.vlMimeTypes = avatarMimeTypes_;
  } else {
    msg.vlMimeTypes = noMimeTypes_;
  }

  (void) msg.Request();
}


void Avatar::OnDetailsChanged(Apollo::ValueList& vlKeys)
{
  for (Apollo::ValueElem* e = 0; e = vlKeys.nextElem(e); ) {
    String sKey = e->getString();
    GetDetail(sKey);
  }
}

void Avatar::OnReceivePublicChat(const ApHandle& hChat, const String& sNickname, const String& sText, const Apollo::TimeValue& tvTimestamp)
{
  Apollo::TimeValue tv = tvTimestamp;
  if (tv.isNull()) {
    tv = Apollo::TimeValue::getTime();
  }

  Chatline* pChat = 0;
  chats_.Get(hChat, pChat);
  if (pChat) {
    if (pChat->sText_ != sText) {
      pChat->sText_ = sText;
      DisplaySetChatline(hChat, sText);
    }
    if (pChat->tv_ != tv) {
      pChat->tv_ = tv;
    }
  } else {
    pChat = new Chatline(sText, tv);
    chats_.Set(hChat, pChat);

    RemoveOldPublicChats(3);

    DisplayAddChatline(hChat, sText);
  }
}

void Avatar::OnReceivePublicAction(const String& sAction)
{
  if (ApIsHandle(hAnimatedItem_)) {
    Msg_Animation_Event msg;
    msg.hItem = hAnimatedItem_;
    msg.sEvent = sAction;
    if (!msg.Request()) {
      apLog_Error((LOG_CHANNEL, "Avatar::OnReceivePublicAction", "Msg_Animation_Event failed: participant=" ApHandleFormat " event=%s", ApHandleType(hParticipant_), StringType(sAction)));
    }
  }
}

void Avatar::OnAnimationBegin(const String& sUrl)
{
  if (sUrl != sImage_) {
    //if (!IsMoving()) {
      DisplaySetImage(sUrl);
      sImage_ = sUrl;
    //}
  }
}

//----------------------------------------------------------

void Avatar::OnCallModuleSrpc(Apollo::SrpcMessage& request, Apollo::SrpcMessage& response)
{
  String sMethod = request.getString("Method");

  if (0){
  } else if (sMethod == "OnPublicChatTimedOut" || sMethod == "OnPublicChatClosed") {
    ApHandle hChat = Apollo::string2Handle(request.getString("hChat"));
    if (ApIsHandle(hChat)){
      OnPublicChatClosed(hChat);
    }

  } else if (sMethod == "OnIconAttachmentClicked") {
    String sLink = request.getString("sLink");
    if (sLink){
      OnIconAttachmentClicked(sLink);
    }

  } else if (sMethod == "OnAvatarPositionReached") {
    int nX = request.getInt("nX");
    OnAvatarPositionReached(nX);

  } else if (sMethod == "OnAvatarDraggedBy") {
    int nX = request.getInt("nX");
    int nY = request.getInt("nY");
    OnAvatarDraggedBy(nX, nY);

  } else {
    throw ApException("Avatar::OnCallModuleSrpc: Unknown Method=%s", StringType(sMethod));
  }
  
}

void Avatar::OnPublicChatClosed(const ApHandle& hChat)
{
  DeletePublicChat(hChat);
}

void Avatar::OnIconAttachmentClicked(const String& sLink)
{
  // Navigate external browser
}

void Avatar::OnAvatarPositionReached(int nX)
{
  EndMove(nX);
}

void Avatar::OnAvatarDraggedBy(int nDiffX, int nDiffY)
{
  int nNewX = nX_ + nDiffX;

  Msg_Vp_SendPosition msg;
  msg.hLocation = pDisplay_->GetLocation();
  msg.kvParams.add(Msg_Vp_Position_X, nNewX);
  if (!msg.Request()) {
    apLog_Warning(( MODULE_NAME, "Avatar::OnAvatarDraggedBy", "%s failed: %s", StringType(msg.Type()), StringType(msg.sComment) ));
  }
}

//----------------------------------------------------------

void Avatar::RemoveOldPublicChats(int nMax)
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
      DeletePublicChat(hOldestChat);
    }
  }
}

void Avatar::DeletePublicChat(const ApHandle& hChat)
{
  Chatline* pChat = 0;
  chats_.Get(hChat, pChat);
  if (pChat) {
    chats_.Unset(hChat);
    delete pChat;
    pChat = 0;

    DisplayRemoveChatline(hChat);
  }
}

//----------------------------------------------------------

void Avatar::DisplaySetNickname(const String& sNickname)
{
  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarNickname");
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.srpc.set("sNickname", sNickname);
  dsm.Request();
}

void Avatar::DisplaySetImage(const String& sUrl)
{
  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarImage");
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.srpc.set("sUrl", sUrl);
  dsm.Request();
}

void Avatar::DisplaySetCommunityAttachment(const String& sUrl, const String& sLabel, const String& sLink)
{
  DisplaySrpcMessage dsm(pDisplay_, "SetCommunityAttachment");
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.srpc.set("sUrl", sUrl);
  dsm.srpc.set("sLabel", sLabel);
  dsm.srpc.set("sLink", sLink);
  dsm.Request();
}

void Avatar::DisplaySetIconAttachment(const String& sUrl, const String& sLabel)
{
  DisplaySrpcMessage dsm(pDisplay_, "SetIconAttachment");
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.srpc.set("sUrl", sUrl);
  dsm.srpc.set("sLabel", sLabel);
  dsm.Request();
}

void Avatar::DisplaySetOnlineStatus(const String& sStatus)
{
  DisplaySrpcMessage dsm(pDisplay_, "SetOnlineStatus");
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.srpc.set("sStatus", sStatus);
  dsm.Request();
}

void Avatar::DisplayAddChatline(const ApHandle& hChat, const String& sText)
{
  DisplaySrpcMessage dsm(pDisplay_, "AddAvatarChat");
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.srpc.set("hChat", hChat);
  dsm.srpc.set("sText", sText);
  dsm.Request();
}

void Avatar::DisplaySetChatline(const ApHandle& hChat, const String& sText)
{
  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarChat");
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.srpc.set("hChat", hChat);
  dsm.srpc.set("sText", sText);
  dsm.Request();
}

void Avatar::DisplayRemoveChatline(const ApHandle& hChat)
{
  DisplaySrpcMessage dsm(pDisplay_, "RemoveAvatarChat");
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.srpc.set("hChat", hChat);
  dsm.Request();
}

//void Avatar::SetUnknownPosition()
//{
//  int nX = 300;
//  int nMin = 100;
//  int nMax = 500;
//
//  if (pDisplay_) {
//    nMax = pDisplay_->GetWidth();
//  }
//
//  nX = nMin + Apollo::getRandom(nMax - nMin);
//
//  nPositionConfirmed_ = 0;
//
//  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarPosition");
//  dsm.srpc.setString("hParticipant", hParticipant_);
//  dsm.srpc.setInt("nX", nX);
//  dsm.Request();
//}

void Avatar::DisplaySetAvatarPosition(int nDestX)
{
  DisplaySrpcMessage dsm(pDisplay_, "SetAvatarPosition");
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.srpc.set("nX", nDestX);
  dsm.Request();
}

void Avatar::DisplayMoveAvatarPosition(int nDestX)
{
  int nSpeedX = 50;

  if (ApIsHandle(hAnimatedItem_)) {
    String sSequence;

    {
      Msg_Animation_GetGroupSequences msg;
      msg.hItem = hAnimatedItem_;
      msg.sGroup = (nDestX > nX_ ? Msg_Animation_Activity_MoveRight : Msg_Animation_Activity_MoveLeft);
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, "Avatar::DisplayMoveAvatarPosition", "%s failed: participant=" ApHandleFormat " group=%s", StringType(msg.Type()), ApHandleType(hParticipant_), StringType(msg.sGroup)));
      } else {
        sSequence = msg.vlSequences.atIndex(0, (nDestX > nX_ ? Msg_Animation_Activity_MoveRight : Msg_Animation_Activity_MoveLeft));
      }
    }

    if (sSequence) {
      Msg_Animation_GetSequenceInfo msg;
      msg.hItem = hAnimatedItem_;
      msg.sSequence = sSequence;
      if (!msg.Request()) {
        apLog_Error((LOG_CHANNEL, "Avatar::DisplayMoveAvatarPosition", "%s failed: participant=" ApHandleFormat " sequence=%s", StringType(msg.Type()), ApHandleType(hParticipant_), StringType(msg.sSequence)));
      } else {
        if (msg.nDx != 0) {
          nSpeedX = abs(msg.nDx);
        }
      }
    }

  }

  DisplaySrpcMessage dsm(pDisplay_, "MoveAvatarPosition");
  dsm.srpc.set("hParticipant", hParticipant_);
  dsm.srpc.set("nX", nDestX);
  dsm.srpc.set("nSpeedX", nSpeedX);
  dsm.Request();
}
